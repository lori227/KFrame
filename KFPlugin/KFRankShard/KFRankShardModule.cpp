#include "KFRankShardModule.hpp"

namespace KFrame
{
    void KFRankShardModule::BeforeRun()
    {
        _kf_route->RegisterConnectionFunction( this, &KFRankShardModule::OnRouteConnectCluster );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_NOTICE_RANK_WORKER_REQ, &KFRankShardModule::HandleNoticeRankWorkerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SYNC_REFRESH_RANK, &KFRankShardModule::HandleSyncRefreshRank );

        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_RANK_DATA_REQ, &KFRankShardModule::HandleUpdateRankDataReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_REQ, &KFRankShardModule::HandleQueryRanklistReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &KFRankShardModule::HandleQueryFriendRanklistReq );
    }

    void KFRankShardModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_SCHEDULE__();
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
    void KFRankShardModule::OnRouteConnectCluster( uint64 serverid )
    {
        // 把worker id广播给所有的rank shard
        KFMsg::S2SNoticeRankWorkerReq req;
        req.set_workerid( KFGlobal::Instance()->_app_id->GetWorkId() );
        _kf_route->RepeatToAll( __STRING__( rank ), KFMsg::S2S_NOTICE_RANK_WORKER_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string& KFRankShardModule::FormatRankDataKey( uint32 rankid, uint32 zoneid )
    {
        static std::string rankdatakey = "";
        rankdatakey = __REDIS_KEY_3__( __STRING__( rankdata ), rankid, zoneid );
        return rankdatakey;
    }

    std::string& KFRankShardModule::FormatRankSortKey( uint32 rankid, uint32 zoneid )
    {
        static std::string ranksortkey = "";
        ranksortkey = __REDIS_KEY_3__( __STRING__( ranksort ), rankid, zoneid );
        return ranksortkey;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFRankData* KFRankShardModule::LoadRankData( uint32 rankid, uint32 zoneid )
    {
        auto kfrankdata = _kf_rank_data.Create( RankKey( rankid, zoneid ) );
        kfrankdata->_rank_id = rankid;
        kfrankdata->_zone_id = zoneid;

        auto querydata = _rank_redis_driver->HGetAll( __REDIS_KEY_3__( __STRING__( rank ), rankid, zoneid ) );
        if ( !querydata->_value.empty() )
        {
            auto strrankata = querydata->_value[ __STRING__( rankdata ) ];
            KFProto::Parse( &kfrankdata->_rank_datas, strrankata, KFCompressEnum::ZSTD, true );

            kfrankdata->_min_rank_score = __TO_UINT64__( querydata->_value[ __STRING__( minrankscore ) ] );
        }

        return kfrankdata;
    }

    void KFRankShardModule::SaveRankData( KFRankData* kfrankdata )
    {
        auto strrankdata = KFProto::Serialize( &kfrankdata->_rank_datas, KFCompressEnum::ZSTD, 5u, true );

        StringMap rankdata;
        rankdata[ __STRING__( id ) ] = __TO_STRING__( kfrankdata->_rank_id );
        rankdata[ __STRING__( zoneid ) ] = __TO_STRING__( kfrankdata->_zone_id );
        rankdata[ __STRING__( minrankscore ) ] = __TO_STRING__( kfrankdata->_min_rank_score );
        rankdata[ __STRING__( rankdata ) ] = strrankdata;
        auto kfresult = _rank_redis_driver->HMSet( __REDIS_KEY_3__( __STRING__( rank ), kfrankdata->_rank_id, kfrankdata->_zone_id ), rankdata );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "rank=[{}:{}] save failed", kfrankdata->_rank_id, kfrankdata->_zone_id );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleNoticeRankWorkerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SNoticeRankWorkerReq );
        if ( kfmsg.workerid() < _max_rank_worker_id )
        {
            return;
        }

        __UN_TIMER_0__();
        __UN_SCHEDULE__();
        _max_rank_worker_id = kfmsg.workerid();
        for ( auto& iter : KFRankConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto value = kfsetting->_id % _max_rank_worker_id;
            if ( value == 0u )
            {
                value = _max_rank_worker_id;
            }

            if ( value == KFGlobal::Instance()->_app_id->GetWorkId() )
            {
                StartRefreshRankDataTimer( kfsetting );
            }
        }
    }

    void KFRankShardModule::StartRefreshRankDataTimer( const KFRankSetting* kfsetting )
    {
        if ( kfsetting->_refresh_type == KFTimeEnum::Minute )
        {
            // 每5分钟 就在5分钟就在0, 5, 10... 整除关系
            auto passtime = KFGlobal::Instance()->_real_time % kfsetting->_refresh_time;
            auto delaytime = ( passtime == 0u ? 0u : kfsetting->_refresh_time - passtime );

            // 配置_refresh_time 单位:秒
            __LOOP_TIMER_1__( kfsetting->_id, kfsetting->_refresh_time * 1000, delaytime * 1000, &KFRankShardModule::OnTimerRefreshRankData );
        }
        else
        {
            auto kfschedulesetting = _kf_schedule->CreateScheduleSetting();
            kfschedulesetting->SetData( kfsetting->_id, nullptr, 0 );
            switch ( kfsetting->_refresh_type )
            {
            case KFTimeEnum::Week:
                kfschedulesetting->SetDayOfWeek( KFScheduleEnum::Loop, kfsetting->_refresh_minute, kfsetting->_refresh_hour, kfsetting->_refresh_time );
                break;
            default:
                kfschedulesetting->SetDate( KFScheduleEnum::Loop, kfsetting->_refresh_minute, kfsetting->_refresh_hour, kfsetting->_refresh_time );
                break;
            }

            __REGISTER_SCHEDULE__( kfschedulesetting, &KFRankShardModule::OnScheduleRefreshRankData );
        }
    }

    __KF_TIMER_FUNCTION__( KFRankShardModule::OnTimerRefreshRankData )
    {
        SyncRefreshRankData( static_cast< uint32 >( objectid ) );
    }

    __KF_SCHEDULE_FUNCTION__( KFRankShardModule::OnScheduleRefreshRankData )
    {
        SyncRefreshRankData( static_cast< uint32 >( objectid ) );
    }

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

    bool KFRankShardModule::RefreshRankData( uint32 rankid )
    {
        auto kfsetting = KFRankConfig::Instance()->FindSetting( rankid );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        // 获得排行榜列表
        auto queryzonelist = _rank_redis_driver->SMembers( __REDIS_KEY_2__( __STRING__( ranksortlist ), rankid ) );
        for ( auto& strzoneid : queryzonelist->_value )
        {
            auto zoneid = __TO_UINT32__( strzoneid );

            auto kfrankdata = _kf_rank_data.Create( RankKey( rankid, zoneid ) );
            kfrankdata->_rank_id = rankid;
            kfrankdata->_zone_id = zoneid;
            kfrankdata->_min_rank_score = 0;
            kfrankdata->_rank_datas.Clear();

            auto& ranksortkey = FormatRankSortKey( rankid, zoneid );
            auto& rankdatakey = FormatRankDataKey( rankid, zoneid );

            auto queryidlist = _rank_redis_driver->ZRevRange( ranksortkey, 0, kfsetting->_max_count - 1 );
            if ( queryidlist->_value.empty() )
            {
                continue;
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
            ClearRankData( rankdatakey, ranksortkey, kfsetting );
        }

        return true;
    }

    void KFRankShardModule::ClearRankData( const std::string& rankdatakey, const std::string& ranksortkey, const KFRankSetting* kfsetting )
    {
        switch ( kfsetting->_reset_data_type )
        {
        case KFRankEnum::DataClearRank:
        {
            // 最小积分
            //auto playerid = idlist.back();
            //std::string minscore = "";
            //_rank_driver->StringExecute( minscore, "zscore {}:{}:{} {}", __STRING__( ranksort ), rankid, zoneid, playerid );
            //kfrankdata->_min_rank_score = __TO_UINT64__( minscore );

            // 删除指定数量以后的排序
            auto maxcount = kfsetting->_max_count + 1u;
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

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleSyncRefreshRank )
    {
        __PROTO_PARSE__( KFMsg::S2SSyncRefreshRank );

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

    // 处理打榜请求
    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleUpdateRankDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateRankDataReq );

        auto& ranksortkey = FormatRankSortKey( kfmsg.rankid(), kfmsg.zoneid() );
        auto& rankdatakey = FormatRankDataKey( kfmsg.rankid(), kfmsg.zoneid() );

        auto pbrankdata = &kfmsg.pbrankdata();

        // 排行显示属性
        auto strrankdata = KFProto::Serialize( pbrankdata, KFCompressEnum::None, 0u, true );

        _rank_redis_driver->WriteMulti();
        _rank_redis_driver->HSet( rankdatakey, kfmsg.playerid(), strrankdata );

        // 判断最低的分数
        // if ( IsNeedUpdateRankData( kfmsg.rankid(), kfmsg.zoneid(), kfmsg.rankscore() ) )
        {
            // 添加到排行榜的zone列表
            _rank_redis_driver->SAdd(  __REDIS_KEY_2__( __STRING__( ranksortlist ), kfmsg.rankid() ), kfmsg.zoneid() );

            // 积分排行列表
            _rank_redis_driver->ZAdd( ranksortkey, kfmsg.playerid(), pbrankdata->rankscore() );
        }

        _rank_redis_driver->WriteExec();
    }

    bool KFRankShardModule::IsNeedUpdateRankData( uint32 rankid, uint32 zoneid, uint64 rankscore )
    {
        auto kfrankdata = _kf_rank_data.Find( RankKey( rankid, zoneid ) );
        if ( kfrankdata == nullptr )
        {
            return true;
        }

        return rankscore > kfrankdata->_min_rank_score;
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryRanklistReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRankListReq );

        auto kfrankdata = _kf_rank_data.Find( RankKey( kfmsg.rankid(), kfmsg.zoneid() ) );
        if ( kfrankdata == nullptr )
        {
            kfrankdata = LoadRankData( kfmsg.rankid(), kfmsg.zoneid() );
        }

        KFMsg::MsgQueryRankListAck ack;
        ack.set_rankid( kfmsg.rankid() );
        ack.set_selfindex( QueryPlayerRank( __ROUTE_SEND_ID__, kfmsg.rankid(), kfmsg.zoneid() ) );

        if ( kfmsg.start() == 1u && kfmsg.count() >= ( uint32 )kfrankdata->_rank_datas.rankdata_size() )
        {
            ack.mutable_rankdatas()->CopyFrom( kfrankdata->_rank_datas );
        }
        else
        {
            auto startindex = kfmsg.start() - 1u;
            auto maxindex = kfmsg.start() + kfmsg.count();
            for ( auto i = startindex; i < ( uint32 )kfrankdata->_rank_datas.rankdata_size(); ++i )
            {
                auto& pbrankdata = kfrankdata->_rank_datas.rankdata( i );
                if ( pbrankdata.rankindex() < kfmsg.start() || pbrankdata.rankindex() > maxindex )
                {
                    continue;
                }

                ack.mutable_rankdatas()->add_rankdata()->CopyFrom( pbrankdata );
            }
        }

        _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_RANK_LIST_ACK, &ack );
    }

    uint32 KFRankShardModule::QueryPlayerRank( uint64 playerid, uint32 rankid, uint32 zoneid )
    {
        auto& ranksortkey = FormatRankSortKey( rankid, zoneid );
        auto kfresult = _rank_redis_driver->ZRank( ranksortkey, playerid );
        return ( uint32 )kfresult->_value;
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryFriendRanklistReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendRankListReq );

        auto kfsetting = KFRankConfig::Instance()->FindSetting( kfmsg.rankid() );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 查询
        _rank_redis_driver->ReadMulti();
        for ( auto i = 0; i < kfmsg.friendid_size(); ++i )
        {
            auto friendid = kfmsg.friendid( i );

            auto& rankdatakey = FormatRankDataKey( kfmsg.rankid(), CalcRankZoneId( friendid, kfsetting ) );
            _rank_redis_driver->HGet( rankdatakey, friendid );
        }
        auto rankdatalist = _rank_redis_driver->ReadListExec();

        KFMsg::MsgQueryFriendRankListAck ack;
        ack.set_rankid( kfmsg.rankid() );
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

    uint32 KFRankShardModule::CalcRankZoneId( uint64 playerid, const KFRankSetting* kfsetting )
    {
        auto zoneid = _invalid_int;
        if ( kfsetting->_zone_type == KFMsg::ZoneRank )
        {
            zoneid = KFGlobal::Instance()->STUuidZoneId( __STRING__( player ), playerid );
        }

        return zoneid;
    }
}