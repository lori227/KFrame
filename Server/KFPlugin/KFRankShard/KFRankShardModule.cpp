#include "KFRankShardModule.h"

namespace KFrame
{
    KFRankShardModule::KFRankShardModule()
    {
        _rank_driver = nullptr;
    }

    KFRankShardModule::~KFRankShardModule()
    {

    }

    void KFRankShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_rank_config, true );
    }

    void KFRankShardModule::BeforeRun()
    {
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFRankShardModule::OnServerDiscoverClient );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_RANK_DATA_REQ, &KFRankShardModule::HandleUpdateRankDataReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_REQ, &KFRankShardModule::HandleQueryRanklistReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &KFRankShardModule::HandleQueryFriendRanklistReq );
    }

    void KFRankShardModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_TIMER__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////////////////

        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_RANK_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ );
    }

    void KFRankShardModule::OnceRun()
    {
        _rank_driver = _kf_redis->CreateExecute( __KF_STRING__( rank ) );

        // 加载所有排行榜数据
        LoadTotalRankData();

        // 启动刷新排行榜定时器
        StartRefreshRankDataTimer();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_DISCOVER_FUNCTION__( KFRankShardModule::OnServerDiscoverClient )
    {
        std::list< uint64 > ranklist;

        for ( auto& iter : _kf_rank_config->_kf_rank_setting._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_shard_id == KFGlobal::Instance()->_app_id )
            {
                ranklist.push_back( kfsetting->_rank_id );
            }
        }

        _kf_cluster_shard->AddObjectToProxy( handleid, ranklist );
    }

    void KFRankShardModule::LoadTotalRankData()
    {
        auto ranklist = _rank_driver->QueryList( __FUNC_LINE__, "smembers {}:{}",
                        __KF_STRING__( ranklist ), KFGlobal::Instance()->_app_id );
        if ( !ranklist->IsOk() )
        {
            return;
        }

        for ( auto& strrankkey : ranklist->_value )
        {
            auto queryrankdata = _rank_driver->QueryMap( __FUNC_LINE__, "hgetall {}", strrankkey.c_str() );
            if ( !queryrankdata->IsOk() || queryrankdata->_value.empty()  )
            {
                continue;
            }

            auto rankid = KFUtility::ToValue< uint32 >( queryrankdata->_value[ __KF_STRING__( id ) ] );
            auto zoneid = KFUtility::ToValue< uint32 >( queryrankdata->_value[ __KF_STRING__( zoneid ) ] );

            auto kfrankdata = _kf_rank_data.Create( RankKey( rankid, zoneid ) );
            kfrankdata->_rank_id = rankid;
            kfrankdata->_zone_id = zoneid;
            kfrankdata->_min_rank_score = KFUtility::ToValue< uint64 >( queryrankdata->_value[ __KF_STRING__( minrankscore ) ] );

            auto strrankata = queryrankdata->_value[ __KF_STRING__( rankdata ) ];
            KFProto::Parse( &kfrankdata->_rank_datas, strrankata, KFCompressEnum::Compress );
        }
    }

    void KFRankShardModule::SaveRankData( KFRankData* kfrankdata )
    {
        auto strrankdata = KFProto::Serialize( &kfrankdata->_rank_datas, KFCompressEnum::Compress );

        MapString rankdata;
        rankdata[ __KF_STRING__( id ) ] = __TO_STRING__( kfrankdata->_rank_id );
        rankdata[ __KF_STRING__( zoneid ) ] = __TO_STRING__( kfrankdata->_zone_id );
        rankdata[ __KF_STRING__( minrankscore ) ] = __TO_STRING__( kfrankdata->_min_rank_score );
        rankdata[ __KF_STRING__( rankdata ) ] = strrankdata;

        auto rankkey = __FORMAT__( "{}:{}:{}", __KF_STRING__( rank ), kfrankdata->_rank_id, kfrankdata->_zone_id );

        _rank_driver->Append( rankdata, "hmset {}", rankkey );
        _rank_driver->Append( "sadd {}:{} {}", __KF_STRING__( ranklist ), KFGlobal::Instance()->_app_id, rankkey );
        _rank_driver->Pipeline( __FUNC_LINE__ );
    }

    void KFRankShardModule::StartRefreshRankDataTimer()
    {
        for ( auto& iter : _kf_rank_config->_kf_rank_setting._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_shard_id != KFGlobal::Instance()->_app_id )
            {
                continue;
            }

            StartRefreshRankDataTimer( kfsetting );
        }
    }

    void KFRankShardModule::StartRefreshRankDataTimer( const KFRankSetting* kfsetting )
    {
        if ( kfsetting->_refresh_type == KFTimeEnum::None )
        {
            // 配置_refresh_time 单位:秒
            __REGISTER_LOOP_TIMER__( kfsetting->_rank_id, kfsetting->_refresh_time * 1000, &KFRankShardModule::OnTimerRefreshRankData );
            return;
        }

        auto kfschedulesetting = _kf_schedule->CreateScheduleSetting();
        kfschedulesetting->SetData( kfsetting->_rank_id, nullptr, 0 );
        switch ( kfsetting->_refresh_type )
        {
        case KFTimeEnum::Week:
            kfschedulesetting->SetDayOfWeek( KFScheduleEnum::Loop, kfsetting->_refresh_minute, kfsetting->_refresh_hour, kfsetting->_refresh_time );
            break;
        default:
            kfschedulesetting->SetDate( KFScheduleEnum::Loop, kfsetting->_refresh_minute, kfsetting->_refresh_hour, kfsetting->_refresh_time );
            break;
        }

        _kf_schedule->RegisterSchedule( kfschedulesetting, this, &KFRankShardModule::OnScheduleRefreshRankData );
    }

    __KF_TIMER_FUNCTION__( KFRankShardModule::OnTimerRefreshRankData )
    {
        RefreshRankData( objectid );
    }

    void KFRankShardModule::OnScheduleRefreshRankData( uint32 rankid, const char* data, uint32 size )
    {
        RefreshRankData( rankid );
    }

    std::string& KFRankShardModule::FormatRankDataKey( uint32 rankid, uint32 zoneid )
    {
        static std::string rankdatakey = "";
        rankdatakey = __FORMAT__( "{}:{}:{}", __KF_STRING__( rankdata ), rankid, zoneid );
        return rankdatakey;
    }

    std::string& KFRankShardModule::FormatRankSortKey( uint32 rankid, uint32 zoneid )
    {
        static std::string ranksortkey = "";
        ranksortkey = __FORMAT__( "{}:{}:{}", __KF_STRING__( ranksort ), rankid, zoneid );
        return ranksortkey;
    }

    void KFRankShardModule::RefreshRankData( uint32 rankid )
    {
        auto kfsetting = _kf_rank_config->FindRankSetting( rankid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 获得排行榜列表
        auto queryzonelist = _rank_driver->QueryList( __FUNC_LINE__, "smembers {}:{}", __KF_STRING__( ranksortlist ), rankid );
        auto zonelist = queryzonelist->_value;
        for ( auto& strzoneid : zonelist )
        {
            auto zoneid = KFUtility::ToValue< uint32 >( strzoneid );

            auto kfrankdata = _kf_rank_data.Create( RankKey( rankid, zoneid ) );
            kfrankdata->_rank_id = rankid;
            kfrankdata->_zone_id = zoneid;
            kfrankdata->_min_rank_score = 0;
            kfrankdata->_rank_datas.Clear();

            auto& ranksortkey = FormatRankSortKey( rankid, zoneid );
            auto& rankdatakey = FormatRankDataKey( rankid, zoneid );

            auto queryidlist = _rank_driver->QueryList( __FUNC_LINE__, "zrevrange {} 0 {}", ranksortkey, kfsetting->_max_count - 1 );
            if ( queryidlist->_value.empty() )
            {
                continue;
            }

            auto rankindex = 0u;
            for ( auto& strplayerid : queryidlist->_value )
            {
                auto playerid = KFUtility::ToValue< uint32 >( strplayerid );

                // 读取排行榜信息
                auto queryrankdata = _rank_driver->QueryString( __FUNC_LINE__, "hget {} {}", rankdatakey, playerid );

                auto pbrankdata = kfrankdata->_rank_datas.add_rankdata();
                pbrankdata->set_rankindex( ++rankindex );
                KFProto::Parse( pbrankdata, queryrankdata->_value, KFCompressEnum::Convert );

                // 加载玩家显示数据
                // todo : 更新的时候pbdata里面包含玩家数据
                // LoadPlayerShowData( pbdatas, playerid );
            }

            // 保存排行榜信息
            SaveRankData( kfrankdata );

            if ( kfsetting->_is_reset_data )
            {
                // 清空数据
                _rank_driver->Append( "del {}", rankdatakey );
                _rank_driver->Append( "del {}", ranksortkey );
                _rank_driver->Pipeline( __FUNC_LINE__ );
            }
            else
            {
                // 最小积分
                //auto playerid = idlist.back();
                //std::string minscore = "";
                //_rank_driver->StringExecute( minscore, "zscore {}:{}:{} {}",
                //                             __KF_STRING__( ranksort ), rankid, zoneid, playerid );
                //kfrankdata->_min_rank_score = KFUtility::ToValue< uint64 >( minscore );

                // 删除指定数量以后的排序
                auto rankcount = _rank_driver->QueryUInt64( __FUNC_LINE__, "zcard {}", ranksortkey );
                if ( rankcount->_value > kfsetting->_max_count )
                {
                    _rank_driver->Execute( __FUNC_LINE__, "zremrangebyrank {} 0 {}", ranksortkey, rankcount->_value - kfsetting->_max_count );
                }
            }
        }
    }

    void KFRankShardModule::LoadPlayerShowData( KFMsg::PBStrings* pbdatas, uint32 playerid )
    {
        //// 查询玩家的信息
        //MapString playervalues;
        //_public_driver->MapExecute( playervalues, "hgetall {}:{}",
        //                            __KF_STRING__( public ), playerid );

        //for ( auto& showdata : _kf_rank_config->_player_data )
        //{
        //    auto pbdata = pbdatas->add_pbstring();
        //    pbdata->set_name( showdata );
        //    pbdata->set_value( playervalues[ showdata ] );
        //}
    }

    // 处理打榜请求
    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleUpdateRankDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateRankDataReq );

        auto& ranksortkey = FormatRankSortKey( kfmsg.rankid(), kfmsg.zoneid() );
        auto& rankdatakey = FormatRankDataKey( kfmsg.rankid(), kfmsg.zoneid() );

        auto pbrankdata = &kfmsg.pbrankdata();

        // 排行显示属性
        auto strrankdata = KFProto::Serialize( pbrankdata, KFCompressEnum::Convert );
        _rank_driver->Append( "hset {} {} {}", rankdatakey, kfmsg.playerid(), strrankdata );

        // 判断最低的分数
        // if ( IsNeedUpdateRankData( kfmsg.rankid(), kfmsg.zoneid(), kfmsg.rankscore() ) )
        {
            // 添加到排行榜的zone列表
            _rank_driver->Append( "sadd {}:{} {}", __KF_STRING__( ranksortlist ), kfmsg.rankid(), kfmsg.zoneid() );

            // 积分排行列表
            _rank_driver->Append( "zadd {} {} {}", ranksortkey, pbrankdata->rankscore(), kfmsg.playerid() );
        }

        _rank_driver->Pipeline( __FUNC_LINE__ );
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

        KFMsg::MsgQueryRankListAck ack;
        ack.set_rankid( kfmsg.rankid() );
        auto pbrankdatas = ack.mutable_rankdatas();

        auto kfrankdata = _kf_rank_data.Find( RankKey( kfmsg.rankid(), kfmsg.zoneid() ) );
        if ( kfrankdata != nullptr )
        {
            pbrankdatas->CopyFrom( kfrankdata->_rank_datas );
        }

        _kf_cluster_shard->SendMessageToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::MSG_QUERY_RANK_LIST_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryFriendRanklistReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendRankListReq );

        auto kfsetting = _kf_rank_config->FindRankSetting( kfmsg.rankid() );
        if ( kfsetting == nullptr )
        {
            return ;
        }

        // 查询
        for ( auto i = 0; i < kfmsg.friendid_size(); ++i )
        {
            auto friendid = kfmsg.friendid( i );

            auto& rankdatakey = FormatRankDataKey( kfmsg.rankid(), CalcRankZoneId( friendid, kfsetting ) );
            _rank_driver->Append( "hget {} {}", rankdatakey, friendid );
        }

        auto rankdatalist = _rank_driver->PipelineList( __FUNC_LINE__ );

        KFMsg::MsgQueryFriendRankListAck ack;
        ack.set_rankid( kfmsg.rankid() );
        auto pbrankdatas = ack.mutable_rankdatas();
        for ( auto& strrankdata : rankdatalist->_value )
        {
            if ( !strrankdata.empty() )
            {
                KFProto::Parse( pbrankdatas->add_rankdata(), strrankdata, KFCompressEnum::Convert );
            }
        }

        _kf_cluster_shard->SendMessageToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::MSG_QUERY_FRIEND_RANK_LIST_ACK, &ack );
    }

    uint32 KFRankShardModule::CalcRankZoneId( uint32 playerid, const KFRankSetting* kfsetting )
    {
        auto zoneid = _invalid_int;
        if ( kfsetting->_zone_type == KFRankEnum::ZoneRank )
        {
            zoneid = KFUtility::CalcZoneId( playerid );
        }

        return zoneid;
    }
}