#include "KFRankShardModule.hpp"

namespace KFrame
{
    void KFRankShardModule::BeforeRun()
    {
        // 1分钟执行一次, 并保证00秒的时候执行
        auto delaytime = ( KFGlobal::Instance()->_real_time % KFTimeEnum::OneMinuteSecond );
        delaytime = ( KFTimeEnum::OneMinuteSecond - delaytime ) * KFTimeEnum::OneSecondMicSecond;
        __LOOP_TIMER_0__( KFTimeEnum::OneMinuteMicSecond, delaytime, &KFRankShardModule::OnTimerRefreshRankData );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_route->RegisterConnectionFunction( this, &KFRankShardModule::OnRouteConnectCluster );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_NOTICE_RANK_WORKER_REQ, &KFRankShardModule::HandleNoticeRankWorkerReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_SYNC_REFRESH_RANK, &KFRankShardModule::HandleSyncRefreshRank );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_UPDATE_RANK_DATA_REQ, &KFRankShardModule::HandleUpdateRankDataReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_QUERY_RANK_LIST_REQ, &KFRankShardModule::HandleQueryRanklistReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &KFRankShardModule::HandleQueryFriendRanklistReq );
    }

    void KFRankShardModule::BeforeShut()
    {
        __UN_TIMER_0__();
        _kf_route->UnRegisterConnectionFunction( this );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        __UN_MESSAGE__( KFMsg::S2S_NOTICE_RANK_WORKER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_SYNC_REFRESH_RANK );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_RANK_DATA_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ );
    }

    void KFRankShardModule::PrepareRun()
    {
        _rank_redis_driver = _kf_redis->Create( __STRING__( logic ) );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRankShardModule::OnRouteConnectCluster( uint64 server_id )
    {
        // 把worker id广播给所有的rank shard
        KFMsg::S2SNoticeRankWorkerReq req;
        req.set_workerid( KFGlobal::Instance()->_app_id->GetWorkId() );
        _kf_route->RepeatToAll( __STRING__( rank ), KFMsg::S2S_NOTICE_RANK_WORKER_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleNoticeRankWorkerReq, KFMsg::S2SNoticeRankWorkerReq )
    {
        if ( kfmsg->worker_id() < _max_rank_worker_id )
        {
            return;
        }

        _refresh_rank_id_list.clear();
        _max_rank_worker_id = kfmsg->worker_id();
        for ( auto& iter : KFRankConfig::Instance()->_settings._objects )
        {
            auto setting = iter.second;
            auto value = setting->_id % _max_rank_worker_id;
            if ( value == 0u )
            {
                value = _max_rank_worker_id;
            }

            if ( value == KFGlobal::Instance()->_app_id->GetWorkId() )
            {
                _refresh_rank_id_list.push_back( setting->_id );
            }
        }
    }

    __KF_TIMER_FUNCTION__( KFRankShardModule::OnTimerRefreshRankData )
    {
        for ( auto rankid : _refresh_rank_id_list )
        {
            SyncRefreshRankData( rankid );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string& KFRankShardModule::FormatRankDataKey( uint32 rankid, uint32 zone_id )
    {
        static std::string rankdatakey = "";
        rankdatakey = __DATABASE_KEY_3__( __STRING__( rankdata ), rankid, zone_id );
        return rankdatakey;
    }

    std::string& KFRankShardModule::FormatRankSortKey( uint32 rankid, uint32 zone_id )
    {
        static std::string ranksortkey = "";
        ranksortkey = __DATABASE_KEY_3__( __STRING__( ranksort ), rankid, zone_id );
        return ranksortkey;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRankShardModule::SaveRankData( KFRankData* kfrankdata )
    {
        auto strrankdata = KFProto::Serialize( &kfrankdata->_rank_datas, KFCompressEnum::ZSTD, 5u, true );

        StringMap rankdata;
        rankdata[ __STRING__( id ) ] = __TO_STRING__( kfrankdata->_rank_id );
        rankdata[ __STRING__( zone_id ) ] = __TO_STRING__( kfrankdata->_zone_id );
        rankdata[ __STRING__( time ) ] = __TO_STRING__( kfrankdata->_next_refresh_time );
        rankdata[ __STRING__( minrankscore ) ] = __TO_STRING__( kfrankdata->_min_rank_score );
        rankdata[ __STRING__( rankdata ) ] = strrankdata;
        auto kfresult = _rank_redis_driver->HMSet( __DATABASE_KEY_3__( __STRING__( rank ), kfrankdata->_rank_id, kfrankdata->_zone_id ), rankdata );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "rank=[{}:{}] save failed", kfrankdata->_rank_id, kfrankdata->_zone_id );
        }
    }

    KFRankData* KFRankShardModule::LoadRankData( uint32 rankid, uint32 zone_id )
    {
        auto kfrankdata = _kf_rank_data.Create( RankKey( rankid, zone_id ) );
        kfrankdata->_rank_id = rankid;
        kfrankdata->_zone_id = zone_id;

        auto querydata = _rank_redis_driver->HGetAll( __DATABASE_KEY_3__( __STRING__( rank ), rankid, zone_id ) );
        if ( !querydata->_value.empty() )
        {
            auto strrankata = querydata->_value[ __STRING__( rankdata ) ];
            KFProto::Parse( &kfrankdata->_rank_datas, strrankata, KFCompressEnum::ZSTD, true );

            kfrankdata->_next_refresh_time = __TO_UINT64__( querydata->_value[ __STRING__( time ) ] );
            kfrankdata->_min_rank_score = __TO_UINT64__( querydata->_value[ __STRING__( minrankscore ) ] );
        }

        return kfrankdata;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRankShardModule::SyncRefreshRankData( uint32 rankid )
    {
        auto ok = RefreshRankData( rankid );
        if ( ok )
        {
            // 返回结果
            KFMsg::S2SSyncRefreshRank sync;
            sync.set_rankid( rankid );
            auto ok = _kf_route->RepeatToAll( __STRING__( rank ), KFMsg::S2S_SYNC_REFRESH_RANK, &sync );
            if ( !ok )
            {
                __LOG_ERROR__( "rank=[{}] refresh failed", rankid );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleSyncRefreshRank, KFMsg::S2SSyncRefreshRank )
    {
        // 这里只删除, 防止同一时间加载, 造成数据压力
        std::set< RankKey > removes;
        for ( auto& iter : _kf_rank_data._objects )
        {
            auto kfdata = iter.second;
            if ( kfdata->_rank_id )
            {
                removes.insert( iter.first );
            }
        }

        for ( auto& iter : removes )
        {
            _kf_rank_data.Remove( iter );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRankShardModule::RefreshRankData( uint32 rankid )
    {
        auto kfranksetting = KFRankConfig::Instance()->FindSetting( rankid );
        if ( kfranksetting == nullptr )
        {
            return false;
        }

        // 时间配置
        auto kftimesetting = KFTimeLoopConfig::Instance()->FindSetting( kfranksetting->_refresh_time_id );
        if ( kftimesetting == nullptr )
        {
            return false;
        }

        // 获得排行榜列表
        auto refreshok = false;
        auto time_data = &kftimesetting->_time_data;
        auto queryzonelist = _rank_redis_driver->SMembers( __DATABASE_KEY_2__( __STRING__( ranksortlist ), rankid ) );
        for ( auto& strzoneid : queryzonelist->_value )
        {
            auto zone_id = __TO_UINT32__( strzoneid );
            refreshok |= RefreshRankData( kfranksetting, zone_id, time_data );
        }

        return refreshok;
    }

    bool KFRankShardModule::RefreshRankData( const KFRankSetting* setting, uint32 zone_id, const KFTimeData* time_data )
    {
        auto kfrankdata = _kf_rank_data.Create( RankKey( setting->_id, zone_id ) );
        if ( !KFDate::CheckPassTime(  KFGlobal::Instance()->_real_time, kfrankdata->_next_refresh_time ) )
        {
            return false;
        }

        kfrankdata->_rank_datas.Clear();
        kfrankdata->_rank_id = setting->_id;
        kfrankdata->_zone_id = zone_id;
        kfrankdata->_min_rank_score = 0;
        kfrankdata->_next_refresh_time = KFDate::CalcTimeData( time_data, KFGlobal::Instance()->_real_time, 1 );

        auto& ranksortkey = FormatRankSortKey( setting->_id, zone_id );
        auto& rankdatakey = FormatRankDataKey( setting->_id, zone_id );

        auto queryidlist = _rank_redis_driver->ZRevRange( ranksortkey, 0, setting->_max_count - 1 );
        if ( queryidlist->_value.empty() )
        {
            return false;
        }

        auto rankindex = 0u;
        for ( auto& idpair : queryidlist->_value )
        {
            // 读取排行榜信息
            auto playerid = __TO_UINT64__( idpair.first );
            auto queryrankdata = _rank_redis_driver->HGet( rankdatakey, playerid );
            if ( queryrankdata->_value.empty() )
            {
                continue;
            }

            StringMap basicdata;
            _kf_basic_database->QueryBasicAttribute( playerid, basicdata );
            if ( basicdata.empty() )
            {
                continue;
            }

            auto pbrankdata = kfrankdata->_rank_datas.add_rankdata();
            KFProto::Parse( pbrankdata, queryrankdata->_value, KFCompressEnum::None, true );
            pbrankdata->set_rankindex( ++rankindex );

            auto& pbplayer = *pbrankdata->mutable_pbplayer();
            for ( auto& showname : KFRankConfig::Instance()->_show_data_list )
            {
                auto iter = basicdata.find( showname );
                if ( iter == basicdata.end() )
                {
                    continue;
                }

                pbplayer[ showname ] = iter->second;
            }
        }

        // 保存排行榜信息
        SaveRankData( kfrankdata );

        // 清除排行榜数据
        ClearRankData( rankdatakey, ranksortkey, setting );
        return true;
    }

    void KFRankShardModule::ClearRankData( const std::string& rankdatakey, const std::string& ranksortkey, const KFRankSetting* setting )
    {
        switch ( setting->_reset_data_type )
        {
        case KFRankEnum::DataClearRank:
        {
            // 最小积分
            //auto playerid = idlist.back();
            //std::string minscore = "";
            //_rank_driver->StringExecute( minscore, "zscore {}:{}:{} {}", __STRING__( ranksort ), rankid, zone_id, playerid );
            //kfrankdata->_min_rank_score = __TO_UINT64__( minscore );

            // 删除指定数量以后的排序
            auto maxcount = setting->_max_count + 1u;
            auto rankcount = _rank_redis_driver->ZCard( ranksortkey );
            if ( rankcount->_value > maxcount )
            {
                _rank_redis_driver->ZRemRangeByRank( ranksortkey, 0, rankcount->_value - maxcount );
            }
        }
        break;
        case KFRankEnum::DataClearAll:	// 清除所有
        {
            StringList values;
            values.emplace_back( rankdatakey );
            values.emplace_back( ranksortkey );
            _rank_redis_driver->Del( ranksortkey );
        }
        break;
        default:
            break;
        }
    }

    // 处理打榜请求
    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleUpdateRankDataReq, KFMsg::S2SUpdateRankDataReq )
    {
        auto& ranksortkey = FormatRankSortKey( kfmsg->rankid(), kfmsg->zone_id() );
        auto& rankdatakey = FormatRankDataKey( kfmsg->rankid(), kfmsg->zone_id() );

        auto pbrankdata = &kfmsg->pbrankdata();

        // 排行显示属性
        auto strrankdata = KFProto::Serialize( pbrankdata, KFCompressEnum::None, 0u, true );

        _rank_redis_driver->WriteMulti();
        _rank_redis_driver->HSet( rankdatakey, kfmsg->playerid(), strrankdata );

        // 判断最低的分数
        // if ( IsNeedUpdateRankData( kfmsg->rankid(), kfmsg->zone_id(), kfmsg->rankscore() ) )
        {
            // 添加到排行榜的zone列表
            _rank_redis_driver->SAdd(  __DATABASE_KEY_2__( __STRING__( ranksortlist ), kfmsg->rankid() ), kfmsg->zone_id() );

            // 积分排行列表
            _rank_redis_driver->ZAdd( ranksortkey, kfmsg->playerid(), pbrankdata->rankscore() );
        }

        _rank_redis_driver->WriteExec();
    }

    bool KFRankShardModule::IsNeedUpdateRankData( uint32 rankid, uint32 zone_id, uint64 rankscore )
    {
        auto kfrankdata = _kf_rank_data.Find( RankKey( rankid, zone_id ) );
        if ( kfrankdata == nullptr )
        {
            return true;
        }

        return rankscore > kfrankdata->_min_rank_score;
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryRanklistReq, KFMsg::S2SQueryRankListReq )
    {
        auto kfrankdata = _kf_rank_data.Find( RankKey( kfmsg->rankid(), kfmsg->zone_id() ) );
        if ( kfrankdata == nullptr || KFDate::CheckPassTime( KFGlobal::Instance()->_real_time, kfrankdata->_next_refresh_time ) )
        {
            kfrankdata = LoadRankData( kfmsg->rankid(), kfmsg->zone_id() );
        }

        KFMsg::MsgQueryRankListAck ack;
        ack.set_rankid( kfmsg->rankid() );
        ack.set_selfindex( QueryPlayerRank( __ROUTE_SEND_ID__, kfmsg->rankid(), kfmsg->zone_id() ) );

        if ( kfmsg->start() == 1u && kfmsg->count() >= ( uint32 )kfrankdata->_rank_datas.rankdata_size() )
        {
            ack.mutable_rankdatas()->CopyFrom( kfrankdata->_rank_datas );
        }
        else
        {
            auto startindex = kfmsg->start() - 1u;
            auto maxindex = kfmsg->start() + kfmsg->count();
            for ( auto i = startindex; i < ( uint32 )kfrankdata->_rank_datas.rankdata_size(); ++i )
            {
                auto& pbrankdata = kfrankdata->_rank_datas.rankdata( i );
                if ( pbrankdata.rankindex() < kfmsg->start() || pbrankdata.rankindex() > maxindex )
                {
                    continue;
                }

                ack.mutable_rankdatas()->add_rankdata()->CopyFrom( pbrankdata );
            }
        }

        _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_RANK_LIST_ACK, &ack );
    }

    uint32 KFRankShardModule::QueryPlayerRank( uint64 playerid, uint32 rankid, uint32 zone_id )
    {
        auto& ranksortkey = FormatRankSortKey( rankid, zone_id );
        auto kfresult = _rank_redis_driver->ZRank( ranksortkey, playerid );
        return ( uint32 )kfresult->_value;
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryFriendRanklistReq, KFMsg::S2SQueryFriendRankListReq )
    {
        auto setting = KFRankConfig::Instance()->FindSetting( kfmsg->rankid() );
        if ( setting == nullptr )
        {
            return;
        }

        // 查询
        _rank_redis_driver->ReadMulti();
        for ( auto i = 0; i < kfmsg->friendid_size(); ++i )
        {
            auto friendid = kfmsg->friendid( i );

            auto& rankdatakey = FormatRankDataKey( kfmsg->rankid(), CalcRankZoneId( friendid, setting ) );
            _rank_redis_driver->HGet( rankdatakey, friendid );
        }
        auto rankdatalist = _rank_redis_driver->ReadListExec();

        KFMsg::MsgQueryFriendRankListAck ack;
        ack.set_rankid( kfmsg->rankid() );
        auto pbrankdatas = ack.mutable_rankdatas();
        for ( auto& strrankdata : rankdatalist->_value )
        {
            if ( !strrankdata.empty() )
            {
                KFProto::Parse( pbrankdatas->add_rankdata(), strrankdata, KFCompressEnum::None, true );
            }
        }

        _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_FRIEND_RANK_LIST_ACK, &ack );
    }

    uint32 KFRankShardModule::CalcRankZoneId( uint64 playerid, const KFRankSetting* setting )
    {
        auto zone_id = _invalid_int;
        if ( setting->_zone_type == KFMsg::ZoneRank )
        {
            zone_id = KFGlobal::Instance()->STUuidZoneId( __STRING__( player ), playerid );
        }

        return zone_id;
    }
}