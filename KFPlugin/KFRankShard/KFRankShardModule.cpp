#include "KFRankShardModule.hpp"

namespace KFrame
{
    void KFRankShardModule::BeforeRun()
    {
        // 1分钟执行一次, 并保证00秒的时候执行
        auto delay_time = ( KFGlobal::Instance()->_real_time % KFTimeEnum::OneMinuteSecond );
        delay_time = ( KFTimeEnum::OneMinuteSecond - delay_time ) * KFTimeEnum::OneSecondMicSecond;
        __LOOP_TIMER_0__( KFTimeEnum::OneMinuteMicSecond, delay_time, &KFRankShardModule::OnTimerRefreshRankData );
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
        _redis_driver = _kf_redis->Create( __STRING__( logic ) );
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
        if ( kfmsg->workerid() < _max_rank_worker_id )
        {
            return;
        }

        _refresh_rank_id_list.clear();
        _max_rank_worker_id = kfmsg->workerid();
        for ( auto& iter : KFRankConfig::Instance()->_setting_list._objects )
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
        for ( auto rank_id : _refresh_rank_id_list )
        {
            SyncRefreshRankData( rank_id );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string& KFRankShardModule::FormatRankDataKey( uint32 rank_id, uint32 zone_id )
    {
        static std::string rank_data_key = "";
        rank_data_key = __DATABASE_KEY_3__( __STRING__( rankdata ), rank_id, zone_id );
        return rank_data_key;
    }

    std::string& KFRankShardModule::FormatRankSortKey( uint32 rank_id, uint32 zone_id )
    {
        static std::string rank_sort_key = "";
        rank_sort_key = __DATABASE_KEY_3__( __STRING__( ranksort ), rank_id, zone_id );
        return rank_sort_key;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRankShardModule::SaveRankData( std::shared_ptr<KFRankData> rank_data )
    {
        auto str_rank_data = KFProto::Serialize( &rank_data->_rank_datas, KFCompressEnum::ZSTD, 5u, true );

        StringMap rank_data_values;
        rank_data_values[ __STRING__( id ) ] = __TO_STRING__( rank_data->_rank_id );
        rank_data_values[ __STRING__( zoneid ) ] = __TO_STRING__( rank_data->_zone_id );
        rank_data_values[ __STRING__( time ) ] = __TO_STRING__( rank_data->_next_refresh_time );
        rank_data_values[ __STRING__( minrankscore ) ] = __TO_STRING__( rank_data->_min_rank_score );
        rank_data_values[ __STRING__( rankdata ) ] = str_rank_data;
        auto result = _redis_driver->HMSet( __DATABASE_KEY_3__( __STRING__( rank ), rank_data->_rank_id, rank_data->_zone_id ), rank_data_values );
        if ( !result->IsOk() )
        {
            __LOG_ERROR__( "rank=[{}:{}] save failed", rank_data->_rank_id, rank_data->_zone_id );
        }
    }

    std::shared_ptr<KFRankData> KFRankShardModule::LoadRankData( uint32 rank_id, uint32 zone_id )
    {
        auto rank_data = _kf_rank_data.Create( RankKey( rank_id, zone_id ) );
        rank_data->_rank_id = rank_id;
        rank_data->_zone_id = zone_id;

        auto query_data_result = _redis_driver->HGetAll( __DATABASE_KEY_3__( __STRING__( rank ), rank_id, zone_id ) );
        if ( !query_data_result->_value.empty() )
        {
            auto str_rank_ata = query_data_result->_value[ __STRING__( rankdata ) ];
            KFProto::Parse( &rank_data->_rank_datas, str_rank_ata, KFCompressEnum::ZSTD, true );

            rank_data->_next_refresh_time = __TO_UINT64__( query_data_result->_value[ __STRING__( time ) ] );
            rank_data->_min_rank_score = __TO_UINT64__( query_data_result->_value[ __STRING__( minrankscore ) ] );
        }

        return rank_data;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRankShardModule::SyncRefreshRankData( uint32 rank_id )
    {
        auto ok = RefreshRankData( rank_id );
        if ( ok )
        {
            // 返回结果
            KFMsg::S2SSyncRefreshRank sync;
            sync.set_rankid( rank_id );
            auto ok = _kf_route->RepeatToAll( __STRING__( rank ), KFMsg::S2S_SYNC_REFRESH_RANK, &sync );
            if ( !ok )
            {
                __LOG_ERROR__( "rank=[{}] refresh failed", rank_id );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleSyncRefreshRank, KFMsg::S2SSyncRefreshRank )
    {
        // 这里只删除, 防止同一时间加载, 造成数据压力
        std::set<RankKey> remove_list;
        for ( auto& iter : _kf_rank_data._objects )
        {
            auto data = iter.second;
            if ( data->_rank_id )
            {
                remove_list.insert( iter.first );
            }
        }

        for ( auto& iter : remove_list )
        {
            _kf_rank_data.Remove( iter );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRankShardModule::RefreshRankData( uint32 rank_id )
    {
        auto rank_setting = KFRankConfig::Instance()->FindSetting( rank_id );
        if ( rank_setting == nullptr )
        {
            return false;
        }

        // 时间配置
        auto time_setting = KFTimeLoopConfig::Instance()->FindSetting( rank_setting->_refresh_time_id );
        if ( time_setting == nullptr )
        {
            return false;
        }

        // 获得排行榜列表
        auto refresh_ok = false;
        auto time_data = &time_setting->_time_data;
        auto query_zone_list_result = _redis_driver->SMembers( __DATABASE_KEY_2__( __STRING__( ranksortlist ), rank_id ) );
        for ( auto& str_zone_id : query_zone_list_result->_value )
        {
            auto zone_id = __TO_UINT32__( str_zone_id );
            refresh_ok |= RefreshRankData( rank_setting, zone_id, time_data );
        }

        return refresh_ok;
    }

    bool KFRankShardModule::RefreshRankData( std::shared_ptr<const KFRankSetting> setting, uint32 zone_id, const KFTimeData* time_data )
    {
        auto rank_data = _kf_rank_data.Create( RankKey( setting->_id, zone_id ) );
        if ( !KFDate::CheckPassTime(  KFGlobal::Instance()->_real_time, rank_data->_next_refresh_time ) )
        {
            return false;
        }

        rank_data->_rank_datas.Clear();
        rank_data->_rank_id = setting->_id;
        rank_data->_zone_id = zone_id;
        rank_data->_min_rank_score = 0;
        rank_data->_next_refresh_time = KFDate::CalcTimeData( time_data, KFGlobal::Instance()->_real_time, 1 );

        auto& rank_sort_key = FormatRankSortKey( setting->_id, zone_id );
        auto& rank_data_key = FormatRankDataKey( setting->_id, zone_id );

        auto query_id_list_result = _redis_driver->ZRevRange( rank_sort_key, 0, setting->_max_count - 1 );
        if ( query_id_list_result->_value.empty() )
        {
            return false;
        }

        auto rank_index = 0u;
        for ( auto& id_pair : query_id_list_result->_value )
        {
            // 读取排行榜信息
            auto player_id = __TO_UINT64__( id_pair.first );
            auto query_rank_data_result = _redis_driver->HGet( rank_data_key, player_id );
            if ( query_rank_data_result->_value.empty() )
            {
                continue;
            }

            StringMap basic_data;
            _kf_basic_database->QueryBasicAttribute( player_id, basic_data );
            if ( basic_data.empty() )
            {
                continue;
            }

            auto proto_rank_data = rank_data->_rank_datas.add_rankdata();
            KFProto::Parse( proto_rank_data, query_rank_data_result->_value, KFCompressEnum::None, true );
            proto_rank_data->set_rankindex( ++rank_index );

            auto& proto_player = *proto_rank_data->mutable_pbplayer();
            for ( auto& show_name : KFRankConfig::Instance()->_show_data_list )
            {
                auto iter = basic_data.find( show_name );
                if ( iter == basic_data.end() )
                {
                    continue;
                }

                proto_player[show_name] = iter->second;
            }
        }

        // 保存排行榜信息
        SaveRankData( rank_data );

        // 清除排行榜数据
        ClearRankData( rank_data_key, rank_sort_key, setting );
        return true;
    }

    void KFRankShardModule::ClearRankData( const std::string& rank_data_key, const std::string& rank_sort_key, std::shared_ptr<const KFRankSetting> setting )
    {
        switch ( setting->_reset_data_type )
        {
        case KFRankEnum::DataClearRank:
        {
            // 最小积分
            //auto player_id = idlist.back();
            //std::string minscore = "";
            //_rank_driver->StringExecute( minscore, "zscore {}:{}:{} {}", __STRING__( ranksort ), rank_id, zone_id, player_id );
            //rank_data->_min_rank_score = __TO_UINT64__( minscore );

            // 删除指定数量以后的排序
            auto max_count = setting->_max_count + 1u;
            auto rank_count = _redis_driver->ZCard( rank_sort_key );
            if ( rank_count->_value > max_count )
            {
                _redis_driver->ZRemRangeByRank( rank_sort_key, 0, rank_count->_value - max_count );
            }
        }
        break;
        case KFRankEnum::DataClearAll:	// 清除所有
        {
            StringList values;
            values.emplace_back( rank_data_key );
            values.emplace_back( rank_sort_key );
            _redis_driver->Del( rank_sort_key );
        }
        break;
        default:
            break;
        }
    }

    // 处理打榜请求
    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleUpdateRankDataReq, KFMsg::S2SUpdateRankDataReq )
    {
        auto& rank_sort_key = FormatRankSortKey( kfmsg->rankid(), kfmsg->zoneid() );
        auto& rank_data_key = FormatRankDataKey( kfmsg->rankid(), kfmsg->zoneid() );

        auto proto_rank_data = &kfmsg->pbrankdata();

        // 排行显示属性
        auto str_rank_data = KFProto::Serialize( proto_rank_data, KFCompressEnum::None, 0u, true );

        _redis_driver->WriteMulti();
        _redis_driver->HSet( rank_data_key, kfmsg->playerid(), str_rank_data );

        // 判断最低的分数
        // if ( IsNeedUpdateRankData( kfmsg->rank_id(), kfmsg->zone_id(), kfmsg->rankscore() ) )
        {
            // 添加到排行榜的zone列表
            _redis_driver->SAdd(  __DATABASE_KEY_2__( __STRING__( ranksortlist ), kfmsg->rankid() ), kfmsg->zoneid() );

            // 积分排行列表
            _redis_driver->ZAdd( rank_sort_key, kfmsg->playerid(), proto_rank_data->rankscore() );
        }

        _redis_driver->WriteExec();
    }

    bool KFRankShardModule::IsNeedUpdateRankData( uint32 rank_id, uint32 zone_id, uint64 rankscore )
    {
        auto rank_data = _kf_rank_data.Find( RankKey( rank_id, zone_id ) );
        if ( rank_data == nullptr )
        {
            return true;
        }

        return rankscore > rank_data->_min_rank_score;
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryRanklistReq, KFMsg::S2SQueryRankListReq )
    {
        auto rank_data = _kf_rank_data.Find( RankKey( kfmsg->rankid(), kfmsg->zoneid() ) );
        if ( rank_data == nullptr || KFDate::CheckPassTime( KFGlobal::Instance()->_real_time, rank_data->_next_refresh_time ) )
        {
            rank_data = LoadRankData( kfmsg->rankid(), kfmsg->zoneid() );
        }

        KFMsg::MsgQueryRankListAck ack;
        ack.set_rankid( kfmsg->rankid() );
        ack.set_selfindex( QueryPlayerRank( __ROUTE_SEND_ID__, kfmsg->rankid(), kfmsg->zoneid() ) );

        if ( kfmsg->start() == 1u && kfmsg->count() >= ( uint32 )rank_data->_rank_datas.rankdata_size() )
        {
            ack.mutable_rankdatas()->CopyFrom( rank_data->_rank_datas );
        }
        else
        {
            auto start_index = kfmsg->start() - 1u;
            auto max_index = kfmsg->start() + kfmsg->count();
            for ( auto i = start_index; i < ( uint32 )rank_data->_rank_datas.rankdata_size(); ++i )
            {
                auto& proto_rank_data = rank_data->_rank_datas.rankdata( i );
                if ( proto_rank_data.rankindex() < kfmsg->start() || proto_rank_data.rankindex() > max_index )
                {
                    continue;
                }

                ack.mutable_rankdatas()->add_rankdata()->CopyFrom( proto_rank_data );
            }
        }

        _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_RANK_LIST_ACK, &ack );
    }

    uint32 KFRankShardModule::QueryPlayerRank( uint64 player_id, uint32 rank_id, uint32 zone_id )
    {
        auto& rank_sort_key = FormatRankSortKey( rank_id, zone_id );
        auto result = _redis_driver->ZRank( rank_sort_key, player_id );
        return ( uint32 )result->_value;
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryFriendRanklistReq, KFMsg::S2SQueryFriendRankListReq )
    {
        auto setting = KFRankConfig::Instance()->FindSetting( kfmsg->rankid() );
        if ( setting == nullptr )
        {
            return;
        }

        // 查询
        _redis_driver->ReadMulti();
        for ( auto i = 0; i < kfmsg->friendid_size(); ++i )
        {
            auto friend_id = kfmsg->friendid( i );

            auto& rank_data_key = FormatRankDataKey( kfmsg->rankid(), CalcRankZoneId( friend_id, setting ) );
            _redis_driver->HGet( rank_data_key, friend_id );
        }
        auto rank_data_list_result = _redis_driver->ReadListExec();

        KFMsg::MsgQueryFriendRankListAck ack;
        ack.set_rankid( kfmsg->rankid() );
        auto proto_rank_data_list = ack.mutable_rankdatas();
        for ( auto& str_rank_data : rank_data_list_result->_value )
        {
            if ( !str_rank_data.empty() )
            {
                KFProto::Parse( proto_rank_data_list->add_rankdata(), str_rank_data, KFCompressEnum::None, true );
            }
        }

        _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_FRIEND_RANK_LIST_ACK, &ack );
    }

    uint32 KFRankShardModule::CalcRankZoneId( uint64 player_id, std::shared_ptr<const KFRankSetting> setting )
    {
        auto zone_id = _invalid_int;
        if ( setting->_zone_type == KFMsg::ZoneRank )
        {
            zone_id = KFGlobal::Instance()->STUuidZoneId( __STRING__( player ), player_id );
        }

        return zone_id;
    }
}