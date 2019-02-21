#include "KFRankShardModule.h"

namespace KFrame
{
    void KFRankShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_rank_config, true );
    }

    void KFRankShardModule::BeforeRun()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REFRESH_RANK_REQ, &KFRankShardModule::HandleRefreshRankReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_REFRESH_RANK, &KFRankShardModule::HandleTellRefreshRank );

        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_RANK_DATA_REQ, &KFRankShardModule::HandleUpdateRankDataReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_REQ, &KFRankShardModule::HandleQueryRanklistReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &KFRankShardModule::HandleQueryFriendRanklistReq );
    }

    void KFRankShardModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_rank_config );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        __UNREGISTER_MESSAGE__( KFMsg::S2S_REFRESH_RANK_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_REFRESH_RANK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_RANK_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ );
    }

    void KFRankShardModule::OnceRun()
    {
        _rank_redis_driver = _kf_redis->Create( __KF_STRING__( rank ) );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRankShardModule::LoadTotalRankData()
    {
        _kf_rank_data.Clear();

        for ( auto& iter : _kf_rank_config->_kf_rank_setting._objects )
        {
            auto kfsetting = iter.second;
            LoadRankData( kfsetting->_rank_id );
        }
    }

    void KFRankShardModule::LoadRankData( uint32 rankid )
    {
        // 排行榜的种类列表
        auto queryzonelist = _rank_redis_driver->QueryList( "smembers {}:{}", __KF_STRING__( ranksortlist ), rankid );
        if ( !queryzonelist->IsOk() )
        {
            return;
        }

        // 排行榜的数据
        for ( auto& strzoneid : queryzonelist->_value )
        {
            auto queryrankdata = _rank_redis_driver->QueryMap( "hgetall {}:{}:{}", __KF_STRING__( rank ), rankid, strzoneid );
            if ( !queryrankdata->IsOk() || queryrankdata->_value.empty() )
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
        auto kfresult = _rank_redis_driver->Update( rankdata, "hmset {}:{}:{}", __KF_STRING__( rank ), kfrankdata->_rank_id, kfrankdata->_zone_id );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "rank=[{}:{}] save failed!", kfrankdata->_rank_id, kfrankdata->_zone_id );
        }
    }

    void KFRankShardModule::RefreshRankData( uint32 rankid )
    {
        auto kfsetting = _kf_rank_config->FindRankSetting( rankid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 获得排行榜列表
        auto queryzonelist = _rank_redis_driver->QueryList( "smembers {}:{}", __KF_STRING__( ranksortlist ), rankid );
        for ( auto& strzoneid : queryzonelist->_value )
        {
            auto zoneid = KFUtility::ToValue< uint32 >( strzoneid );

            auto kfrankdata = _kf_rank_data.Create( RankKey( rankid, zoneid ) );
            kfrankdata->_rank_id = rankid;
            kfrankdata->_zone_id = zoneid;
            kfrankdata->_min_rank_score = 0;
            kfrankdata->_rank_datas.Clear();

            auto& ranksortkey = FormatRankSortKey( rankid, zoneid );
            auto& rankdatakey = FormatRankDataKey( rankid, zoneid );

            auto queryidlist = _rank_redis_driver->QueryList( "zrevrange {} 0 {}", ranksortkey, kfsetting->_max_count - 1 );
            if ( queryidlist->_value.empty() )
            {
                continue;
            }

            auto rankindex = 0u;
            for ( auto& strplayerid : queryidlist->_value )
            {
                auto playerid = KFUtility::ToValue< uint64 >( strplayerid );

                // 读取排行榜信息
                auto queryrankdata = _rank_redis_driver->QueryString( "hget {} {}", rankdatakey, playerid );

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
                _rank_redis_driver->Append( "del {}", rankdatakey );
                _rank_redis_driver->Append( "del {}", ranksortkey );
                _rank_redis_driver->Pipeline();
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
                auto rankcount = _rank_redis_driver->QueryUInt64( "zcard {}", ranksortkey );
                if ( rankcount->_value > kfsetting->_max_count + 1 )
                {
                    _rank_redis_driver->Execute( "zremrangebyrank {} 0 {}", ranksortkey, rankcount->_value - kfsetting->_max_count - 1 );
                }
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleRefreshRankReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRefreshRankReq );

        RefreshRankData( kfmsg.rankid() );

        // 返回结果
        KFMsg::S2SRefreshRankAck ack;
        ack.set_rankid( kfmsg.rankid() );
        auto ok = _kf_tcp_client->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_REFRESH_RANK_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "rank=[{}] refresh failed!", kfmsg.rankid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleTellRefreshRank )
    {
        __PROTO_PARSE__( KFMsg::S2STellRefreshRank );

        LoadRankData( kfmsg.rankid() );
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
        _rank_redis_driver->Append( "hset {} {} {}", rankdatakey, kfmsg.playerid(), strrankdata );

        // 判断最低的分数
        // if ( IsNeedUpdateRankData( kfmsg.rankid(), kfmsg.zoneid(), kfmsg.rankscore() ) )
        {
            // 添加到排行榜的zone列表
            _rank_redis_driver->Append( "sadd {}:{} {}", __KF_STRING__( ranksortlist ), kfmsg.rankid(), kfmsg.zoneid() );

            // 积分排行列表
            _rank_redis_driver->Append( "zadd {} {} {}", ranksortkey, pbrankdata->rankscore(), kfmsg.playerid() );
        }

        _rank_redis_driver->Pipeline();
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

        _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_RANK_LIST_ACK, &ack );
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
            _rank_redis_driver->Append( "hget {} {}", rankdatakey, friendid );
        }

        auto rankdatalist = _rank_redis_driver->PipelineList();

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

        _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_FRIEND_RANK_LIST_ACK, &ack );
    }

    uint32 KFRankShardModule::CalcRankZoneId( uint64 playerid, const KFRankSetting* kfsetting )
    {
        auto zoneid = _invalid_int;
        if ( kfsetting->_zone_type == KFMsg::ZoneRank )
        {
            zoneid = KFUtility::CalcZoneId( playerid );
        }

        return zoneid;
    }
}