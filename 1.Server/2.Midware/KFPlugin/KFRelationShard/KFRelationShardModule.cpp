#include "KFRelationShardModule.h"

namespace KFrame
{
    void KFRelationShardModule::BeforeRun()
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_TO_RELATION_REQ, &KFRelationShardModule::HandleQueryFriendToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_TO_RELATION_REQ, &KFRelationShardModule::HandleQueryFriendInviteToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_TO_RELATION_REQ, &KFRelationShardModule::HandleAddFriendInviteToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_INVITE_TO_RELATION_REQ, &KFRelationShardModule::HandleDelInviteToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_TO_RELATION_REQ, &KFRelationShardModule::HandleAddFriendToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_TO_RELATION_REQ, &KFRelationShardModule::HandleDelFriendToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_RELATION_REQ, &KFRelationShardModule::HandleUpdateFriendLinessToRelationReq );
    }

    void KFRelationShardModule::BeforeShut()
    {
        __UNREGISTER_SCHEDULE_FUNCTION__();
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_TO_RELATION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_TO_RELATION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_TO_RELATION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_INVITE_TO_RELATION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_TO_RELATION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_TO_RELATION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_RELATION_REQ );
    }

    void KFRelationShardModule::OnceRun()
    {
        _public_redis_driver = _kf_redis->Create( __KF_STRING__( public ) );
        _relation_redis_driver = _kf_redis->Create( __KF_STRING__( relation ) );

        auto kfsetting = _kf_schedule->CreateScheduleSetting();
        kfsetting->SetDate( KFScheduleEnum::Loop, 0, _kf_option->GetUInt32( __KF_STRING__( freindlinessresettime ) ) );
        __REGISTER_SCHEDULE_FUNCTION__( kfsetting, &KFRelationShardModule::OnScheduleClearFriendLiness );
    }

    __KF_SCHEDULE_FUNCTION__( KFRelationShardModule::OnScheduleClearFriendLiness )
    {
        _relation_redis_driver->Append( "del {}", __KF_STRING__( friendlinesslimit ) );
        _relation_redis_driver->Pipeline();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationShardModule::MapStringToPBPlayer( MapString& values, uint64 friendid, KFMsg::PBRelation* pbrelation )
    {
        pbrelation->set_playerid( friendid );

        auto pbplayerdata = pbrelation->mutable_playerdata();
        for ( auto& iter : values )
        {
            ( *pbplayerdata )[ iter.first ] = iter.second;
        }
    }

    void KFRelationShardModule::MapStringToPBRelation( MapString& values, KFMsg::PBRelation* pbrelation, bool newadd )
    {
        auto pbdata = pbrelation->mutable_relationdata();
        for ( auto& iter : values )
        {
            ( *pbdata )[ iter.first ] = iter.second;
        }

        if ( !newadd )
        {
            return;
        }

        ( *pbdata )[ __KF_STRING__( newadd ) ] = "1";
        ( *pbdata )[ __KF_STRING__( time ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
    }

    std::string KFRelationShardModule::FormatFriendKey( uint64 firstid, uint64 secondid )
    {
        auto id1 = __MIN__( firstid, secondid );
        auto id2 = __MAX__( firstid, secondid );
        return __FORMAT__( "{}:{}:{}", __KF_STRING__( friend ), id1, id2 );
    }

    std::string KFRelationShardModule::FormatFriendLimitKey( uint64 firstid, uint64 secondid, uint32 type )
    {
        auto id1 = __MIN__( firstid, secondid );
        auto id2 = __MAX__( firstid, secondid );
        return __FORMAT__( "{}:{}:{}", id1, id2, type );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryFriendToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendToRelationReq );

        // 查询好友列表
        auto queryidlist = _relation_redis_driver->QueryList( "smembers {}:{}", __KF_STRING__( friendlist ), kfmsg.playerid() );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        KFMsg::S2SQueryFriendToGameAck ack;
        ack.set_playerid( kfmsg.playerid() );

        for ( auto& strid : queryidlist->_value )
        {
            auto friendid = KFUtility::ToValue< uint64 >( strid );

            // 好友的基本信息
            auto publicdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), friendid );
            if ( publicdata->_value.empty() )
            {
                continue;
            }

            auto pbfriend = ack.add_pbfriend();
            MapStringToPBPlayer( publicdata->_value, friendid, pbfriend );

            // 好友的关系属性
            auto friendkey = FormatFriendKey( kfmsg.playerid(), friendid );
            auto frienddata = _relation_redis_driver->QueryMap( "hgetall {}", friendkey );
            MapStringToPBRelation( frienddata->_value, pbfriend, false );
        }

        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_FRIEND_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryFriendInviteToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendInviteToRelationReq );

        auto queryidlist = _relation_redis_driver->QueryList( "smembers {}:{}", __KF_STRING__( invitelist ), kfmsg.playerid() );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        ListString removes;

        KFMsg::S2SQueryFriendInviteToGameAck ack;
        ack.set_playerid( kfmsg.playerid() );
        for ( auto& strid : queryidlist->_value )
        {
            auto friendid = KFUtility::ToValue< uint64 >( strid );

            // 获得邀请的数据
            auto queryinvitedata = _relation_redis_driver->QueryMap( "hgetall {}:{}:{}", __KF_STRING__( invite ), kfmsg.playerid(), friendid );
            if ( queryinvitedata->_value.empty() )
            {
                removes.push_back( strid );
                continue;
            }

            // 好友的基本信息
            auto querypublicdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), friendid );
            if ( querypublicdata->_value.empty() )
            {
                removes.push_back( strid );
                continue;
            }

            auto pbinvite = ack.add_pbinvite();
            MapStringToPBPlayer( querypublicdata->_value, friendid, pbinvite );
            MapStringToPBRelation( queryinvitedata->_value, pbinvite, false );
        }
        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_FRIEND_INVITE_TO_GAME_ACK, &ack );

        if ( !removes.empty() )
        {
            // 删除已经过期的邀请信息
            _relation_redis_driver->Update( removes, "srem {}:{}", __KF_STRING__( invitelist ), kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddFriendInviteToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddFriendInviteToRelationReq );

        auto selfid = __ROUTE_SEND_ID__;

        // 查找对方的数据
        auto querytargetdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), kfmsg.playerid() );
        if ( querytargetdata->_value.empty() )
        {
            return _kf_display->SendToPlayer( route, KFMsg::PublicDatabaseError );
        }

        auto refuseinvite = KFUtility::ToValue< uint64 >( querytargetdata->_value[ __KF_STRING__( refusefriend ) ] );
        if ( refuseinvite != _invalid_int )
        {
            return _kf_display->SendToPlayer( route, KFMsg::FriendRefuseInvite, kfmsg.name() );
        }

        // 查找对方的好友数量
        static auto _max_friend_count = _kf_option->FindOption( __KF_STRING__( freindmaxcount ) );
        auto queryfriendcount = _relation_redis_driver->QueryUInt64( "scard {}:{}", __KF_STRING__( friendlist ), kfmsg.playerid() );
        if ( queryfriendcount->_value >= _max_friend_count->_uint32_value )
        {
            return _kf_display->SendToPlayer( route, KFMsg::FriendTargetLimit, kfmsg.name() );
        }

        // 查找对方申请列表是否有自己
        auto queryhaveself = _relation_redis_driver->QueryUInt64( "sismember {}:{} {}", __KF_STRING__( invitelist ), kfmsg.playerid(), selfid );
        if ( queryhaveself->_value != _invalid_int )
        {
            return _kf_display->SendToPlayer( route, KFMsg::FriendInviteAlready, kfmsg.name() );
        }

        // 查找对方申请列表数量
        static auto _max_invite_count = _kf_option->FindOption( __KF_STRING__( freindinvitecount ) );
        auto queryinvitecount = _relation_redis_driver->QueryUInt64( "scard {}:{}", __KF_STRING__( invitelist ), kfmsg.playerid() );
        if ( queryinvitecount->_value >= _max_invite_count->_uint32_value )
        {
            return _kf_display->SendToPlayer( route, KFMsg::FriendInviteLimit, kfmsg.name() );
        }

        // 加入到申请列表
        static auto _invite_keep_time = _kf_option->FindOption( __KF_STRING__( freindinvitekeeptime ) );

        auto invitekey = __FORMAT__( "{}:{}:{}", __KF_STRING__( invite ), kfmsg.playerid(), selfid );
        _relation_redis_driver->Append( "sadd {}:{} {}", __KF_STRING__( invitelist ), kfmsg.playerid(), selfid );
        _relation_redis_driver->Append( "hset {} {} {}", invitekey, __KF_STRING__( message ), kfmsg.message() );
        _relation_redis_driver->Append( "hset {} {} {}", invitekey, __KF_STRING__( time ), KFGlobal::Instance()->_real_time );
        _relation_redis_driver->Append( "expire {} {}", invitekey, _invite_keep_time->_uint32_value );
        _relation_redis_driver->Pipeline();
        _kf_display->SendToPlayer( route, KFMsg::FriendInviteOk, kfmsg.name() );

        // 判断对方是否在线, 如果在线直接发送消息
        auto serverid = KFUtility::ToValue< uint64 >( querytargetdata->_value[ __KF_STRING__( serverid ) ] );
        if ( serverid != _invalid_int )
        {
            // 查找自己的数据
            auto queryselfdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), selfid );
            if ( !queryselfdata->_value.empty() )
            {
                KFMsg::S2SAddFriendInviteToGameAck ack;
                ack.set_playerid( kfmsg.playerid() );

                auto pbinvite = ack.mutable_pbinvite();
                MapStringToPBPlayer( queryselfdata->_value, selfid, pbinvite );

                MapString friendvaluse;
                friendvaluse[ __KF_STRING__( message ) ] = kfmsg.message();
                MapStringToPBRelation( friendvaluse, pbinvite, true );
                _kf_route->SendToPlayer( selfid, serverid, kfmsg.playerid(), KFMsg::S2S_ADD_FRIEND_INVITE_TO_GAME_ACK, &ack );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelInviteToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelFriendInviteToRelationReq );

        _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( invitelist ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Append( "del {}:{}:{}", __KF_STRING__( invite ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Pipeline();
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddFriendToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddFriendToRelationReq );

        auto selfid = __ROUTE_SEND_ID__;
        static auto _max_friend_count = _kf_option->FindOption( __KF_STRING__( freindmaxcount ) );

        // 判断对方好友的数量
        auto targetfriendcount = _relation_redis_driver->QueryUInt64( "scard {}:{}", __KF_STRING__( friendlist ), kfmsg.playerid() );
        if ( targetfriendcount->_value >= _max_friend_count->_uint32_value )
        {
            return _kf_display->SendToPlayer( route, KFMsg::FriendTargetLimit, kfmsg.name() );
        }

        // 添加好友
        auto friendkey = FormatFriendKey( selfid, kfmsg.playerid() );
        _relation_redis_driver->Append( "hset {} {} {}", friendkey, __KF_STRING__( time ), KFGlobal::Instance()->_real_time );

        // 添加列表
        _relation_redis_driver->Append( "sadd {}:{} {}", __KF_STRING__( friendlist ), selfid, kfmsg.playerid() );
        _relation_redis_driver->Append( "sadd {}:{} {}", __KF_STRING__( friendlist ), kfmsg.playerid(), selfid );

        // 删除申请列表
        _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( invitelist ), selfid, kfmsg.playerid() );
        _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( invitelist ), kfmsg.playerid(), selfid );
        _relation_redis_driver->Append( "del {}:{}:{}", __KF_STRING__( invite ), selfid, kfmsg.playerid() );
        _relation_redis_driver->Pipeline();

        // 发送消息给游戏玩家
        {
            auto queryserverid = _public_redis_driver->QueryUInt64( "hget {}:{} {}", __KF_STRING__( public ), kfmsg.playerid(), __KF_STRING__( serverid ) );
            if ( queryserverid->_value != _invalid_int )
            {
                auto querydata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), selfid );
                if ( !querydata->_value.empty() )
                {
                    SendAddFriendToPlayer( queryserverid->_value, querydata->_value, selfid, kfmsg.playerid() );
                }
            }
        }

        {
            auto querydata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), kfmsg.playerid() );
            if ( !querydata->_value.empty() )
            {
                SendAddFriendToPlayer( __ROUTE_SERVER_ID__, querydata->_value, kfmsg.playerid(), selfid );
            }
        }
    }

    void KFRelationShardModule::SendAddFriendToPlayer( uint64 serverid, MapString& values, uint64 playerid, uint64 friendid )
    {
        KFMsg::S2SAddFriendToGameAck ack;
        ack.set_playerid( playerid );

        auto pbfriend = ack.mutable_pbfriend();
        MapStringToPBPlayer( values, friendid, pbfriend );

        MapString friendvalues;
        MapStringToPBRelation( friendvalues, pbfriend, true );
        _kf_route->SendToPlayer( playerid, serverid, friendid, KFMsg::S2S_ADD_FRIEND_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelFriendToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelFriendToRelationReq );

        auto selfid = __ROUTE_SEND_ID__;

        // 删除好友关系
        auto friendkey = FormatFriendKey( selfid, kfmsg.playerid() );

        _relation_redis_driver->Append( "del {}", friendkey );
        _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( friendlist ), selfid, kfmsg.playerid() );
        _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( friendlist ), kfmsg.playerid(), selfid );
        _relation_redis_driver->Pipeline();

        SendDelFriendToPlayer( __ROUTE_SERVER_ID__, selfid, kfmsg.playerid() );

        // 通知对方删除了好友
        auto queryserverid = _public_redis_driver->QueryUInt64( "hget {}:{} {}", __KF_STRING__( public ), kfmsg.playerid(), __KF_STRING__( serverid ) );
        if ( queryserverid->_value != _invalid_int )
        {
            SendDelFriendToPlayer( queryserverid->_value, kfmsg.playerid(), selfid );
        }
    }

    void KFRelationShardModule::SendDelFriendToPlayer( uint64 serverid, uint64 playerid, uint64 friendid )
    {
        KFMsg::S2SDelFriendToGameAck ack;
        ack.set_playerid( playerid );
        ack.set_friendid( friendid );
        _kf_route->SendToPlayer( playerid, serverid, playerid, KFMsg::S2S_DEL_FRIEND_TO_GAME_ACK, &ack );
    }


    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleUpdateFriendLinessToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateFriendLinessToRelationReq );

        UpdateFriendLiness( kfmsg.selfplayerid(), kfmsg.targetplayerid(), kfmsg.type(), kfmsg.friendliness() );
    }

    void KFRelationShardModule::UpdateFriendLiness( uint64 selfplayerid, uint64 targetplayerid, uint32 type, uint32 addvalue )
    {
        static auto _max_friend_liness = _kf_option->FindOption( __KF_STRING__( freindlinessmax ) );

        // 计算总好友度
        auto friendkey = FormatFriendKey( selfplayerid, targetplayerid );

        auto querycurfriendliness = _relation_redis_driver->QueryUInt64( "hget {} {}", friendkey, __KF_STRING__( friendliness ) );
        auto nowfriendliness = querycurfriendliness->_value;
        if ( nowfriendliness >= _max_friend_liness->_uint32_value )
        {
            return;
        }

        // 计算当天剩余好感度
        auto friendlinesslimitkey = FormatFriendLimitKey( selfplayerid, targetplayerid, type );
        auto queryfriendliness = _relation_redis_driver->QueryUInt64( "hget {} {}", __KF_STRING__( friendlinesslimit ), friendlinesslimitkey );
        auto maxfriendliness = _kf_option->GetUInt32( __KF_STRING__( freindlinessdailymax ), type );
        if ( queryfriendliness->_value >= maxfriendliness )
        {
            return;
        }

        // 计算剩余总的好感度
        auto leftfriendliness = maxfriendliness - addvalue;
        auto addfriendliness = __MIN__( leftfriendliness, addvalue );
        auto restfriendliness = _max_friend_liness->_uint32_value - nowfriendliness;
        addfriendliness = __MIN__( addfriendliness, restfriendliness );

        if ( addfriendliness > _invalid_int )
        {
            _relation_redis_driver->Append( "hincrby {} {} {}", friendkey, __KF_STRING__( friendliness ), addfriendliness );
            _relation_redis_driver->Append( "hincrby {} {} {}", __KF_STRING__( friendlinesslimit ), friendlinesslimitkey, addfriendliness );
            _relation_redis_driver->Pipeline();

            // 发送消息给玩家的好友
            SendAddFriendLinessToPlayer( selfplayerid, targetplayerid, addfriendliness );
            SendAddFriendLinessToPlayer( targetplayerid, selfplayerid, addfriendliness );
        }
    }

    void KFRelationShardModule::SendAddFriendLinessToPlayer( uint64 selfid, uint64 targetid, uint32 friendliness )
    {
        auto queryserverid = _public_redis_driver->QueryUInt64( "hget {}:{} {}", __KF_STRING__( public ), selfid, __KF_STRING__( serverid ) );
        if ( queryserverid->_value == _invalid_int )
        {
            return;
        }

        KFMsg::S2SUpdateFriendLinessToGameAck ack;
        ack.set_playerid( selfid );
        ack.set_targetplayerid( targetid );
        ack.set_friendliness( friendliness );
        _kf_route->SendToPlayer( selfid, queryserverid->_value, selfid, KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK, &ack );
    }

    //__KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddRecentPlayerDataReq )
    //{
    //    __PROTO_PARSE__( KFMsg::S2SAddRecentPlayerDataReq );

    //    static auto _recent_keep_time = _kf_option->FindOption( __KF_STRING__( recentkeeptime ) );
    //    static auto _recent_max_count = _kf_option->FindOption( __KF_STRING__( recentmaxcount ) );

    //    auto strbattlerecored = __FORMAT__( "{}:{}:{}", __KF_STRING__( battlerecored ), kfmsg.playerid(), kfmsg.roomid() );

    //    // 先保存战斗数据, 设置有效时间默认7天
    //    auto& strdata = KFProto::Serialize( &kfmsg.pbdata(), KFCompressEnum::Convert );
    //    _relation_redis_driver->Append( "set {} {}", strbattlerecored, strdata );
    //    _relation_redis_driver->Append( "expire {} {}", strbattlerecored, _recent_keep_time->_uint32_value );
    //    _relation_redis_driver->Pipeline();

    //    // 添加到玩家列表
    //    for ( auto i = 0; i < kfmsg.members_size(); ++i )
    //    {
    //        auto strrecentplayer = __FORMAT__( "{}:{}", __KF_STRING__( recentplayer ), kfmsg.members( i ) );
    //        _relation_redis_driver->Execute( "zadd {} {} {}", strrecentplayer, kfmsg.roomid(), kfmsg.playerid() );

    //        // 判断数量, 超过限制数量, 删除最久远的记录
    //        // 战场id是按时间生成的, 所以最新的都是最大的
    //        auto querycount = _relation_redis_driver->QueryUInt64( "zcard {}", strrecentplayer );
    //        if ( querycount->_value > _recent_max_count->_uint32_value )
    //        {
    //            _relation_redis_driver->Execute( "zremrangebyrank {} 0 0", strrecentplayer );
    //        }
    //    }
    //}

    //__KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryRecentListReq )
    //{
    //    __PROTO_PARSE__( KFMsg::S2SQueryRecentListReq );

    //    KFMsg::S2SQueryRecentListAck ack;
    //    ack.set_playerid( kfmsg.playerid() );

    //    // 查询列表
    //    auto queryrecentlist = _relation_redis_driver->QueryMap( "zrange {}:{} 0 -1 withscores", __KF_STRING__( recentplayer ), kfmsg.playerid() );
    //    for ( auto& iter : queryrecentlist->_value )
    //    {
    //        // 查询记录
    //        auto playerid = KFUtility::ToValue< uint64 >( iter.first );
    //        auto roomid = KFUtility::ToValue< uint64 >( iter.second );

    //        auto querydata = _relation_redis_driver->QueryString( "get {}:{}:{}", __KF_STRING__( battlerecored ), playerid, roomid );
    //        if ( !querydata->IsOk() )
    //        {
    //            continue;
    //        }

    //        // 找不到了 删除列表
    //        if ( querydata->_value.empty() )
    //        {
    //            _relation_redis_driver->Execute( "zrem {}:{} {}", __KF_STRING__( recentplayer ), kfmsg.playerid(), playerid );
    //            continue;
    //        }

    //        // 查询玩家的基础信息
    //        auto queryplayerdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), playerid );
    //        if ( queryplayerdata->_value.empty() )
    //        {
    //            continue;
    //        }

    //        auto pbrelation = ack.add_pbrelation();
    //        KFProto::Parse( pbrelation->mutable_relationdata(), querydata->_value, KFCompressEnum::Convert );
    //        MapStringToPBPlayer( queryplayerdata->_value, playerid, pbrelation );
    //    }

    //    _kf_cluster_shard->SendToClient( kfid, KFMsg::S2S_QUERY_RECENT_LIST_ACK, &ack );
    //}
}