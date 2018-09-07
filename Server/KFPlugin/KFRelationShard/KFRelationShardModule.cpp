#include "KFRelationShardModule.h"

namespace KFrame
{
    KFRelationShardModule::KFRelationShardModule()
    {
        _public_redis_driver = nullptr;
        _relation_redis_driver = nullptr;
    }

    KFRelationShardModule::~KFRelationShardModule()
    {
    }

    void KFRelationShardModule::InitModule()
    {
    }

    void KFRelationShardModule::BeforeRun()
    {
        auto kfsetting = _kf_schedule->CreateScheduleSetting();
        kfsetting->SetDate( KFScheduleEnum::Loop, 0, _kf_option->GetValue< uint32 >( __KF_STRING__( freindlinessresettime ) ) );
        __REGISTER_SCHEDULE_FUNCTION__( kfsetting, &KFRelationShardModule::OnScheduleClearFriendLiness );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_REQ, &KFRelationShardModule::HandleQueryFriendReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_REQ, &KFRelationShardModule::HandleQueryFriendInviteReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_REQ, &KFRelationShardModule::HandleAddFriendInviteReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_INVITE_REQ, &KFRelationShardModule::HandleDelInviteReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_REQ, &KFRelationShardModule::HandleAddFriendReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_REQ, &KFRelationShardModule::HandleDelFriendReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIEND_LINESS_REQ, &KFRelationShardModule::HandleUpdateFriendLinessReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RECENT_LIST_REQ, &KFRelationShardModule::HandleQueryRecentListReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_TOAST_REQ, &KFRelationShardModule::HandlePlayerToastReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_TOAST_COUNT_REQ, &KFRelationShardModule::HandleQueryToastCountReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_RECENT_PLAYER_DATA_REQ, &KFRelationShardModule::HandleAddRecentPlayerDataReq );
    }

    void KFRelationShardModule::BeforeShut()
    {
        __UNREGISTER_SCHEDULE_FUNCTION__();
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIEND_LINESS_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_RECENT_PLAYER_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_TOAST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_TOAST_COUNT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_RECENT_LIST_REQ );
    }

    void KFRelationShardModule::OnceRun()
    {
        _public_redis_driver = _kf_redis->CreateExecute( __KF_STRING__( public ) );
        _relation_redis_driver = _kf_redis->CreateExecute( __KF_STRING__( relation ) );
    }

    __KF_SCHEDULE_FUNCTION__( KFRelationShardModule::OnScheduleClearFriendLiness )
    {
        _relation_redis_driver->Append( "del {}", __KF_STRING__( dailytoast ) );
        _relation_redis_driver->Append( "del {}", __KF_STRING__( friendlinesslimit ) );
        _relation_redis_driver->Pipeline();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationShardModule::MapStringToPBPlayer( MapString& values, uint32 friendid, KFMsg::PBRelation* pbrelation )
    {
        pbrelation->set_playerid( friendid );

        auto pbstrings = pbrelation->mutable_playerdata();
        for ( auto iter : values )
        {
            auto pbdata = pbstrings->add_pbstring();
            pbdata->set_name( iter.first );
            pbdata->set_value( iter.second );
        }
    }

    void KFRelationShardModule::MapStringToPBRelation( MapString& values, KFMsg::PBRelation* pbrelation, bool newadd )
    {
        auto pbstrings = pbrelation->mutable_relationdata();
        for ( auto iter : values )
        {
            auto pbdata = pbstrings->add_pbstring();
            pbdata->set_name( iter.first );
            pbdata->set_value( iter.second );
        }

        if ( !newadd )
        {
            return;
        }

        {
            auto pbdata = pbstrings->add_pbstring();
            pbdata->set_name( __KF_STRING__( newadd ) );
            pbdata->set_value( "1" );
        }

        {
            auto pbdata = pbstrings->add_pbstring();
            pbdata->set_name( __KF_STRING__( time ) );
            pbdata->set_value( __TO_STRING__( KFGlobal::Instance()->_real_time ) );
        }
    }

    std::string KFRelationShardModule::FormatFriendKey( const std::string& key, uint32 firstid, uint32 secondid )
    {
        auto id1 = __MIN__( firstid, secondid );
        auto id2 = __MAX__( firstid, secondid );
        return __FORMAT__( "{}:{}:{}", key, id1, id2 );
    }

    std::string KFRelationShardModule::FormatFriendLimitKey( uint32 firstid, uint32 secondid, uint32 type )
    {
        auto id1 = __MIN__( firstid, secondid );
        auto id2 = __MAX__( firstid, secondid );
        return __FORMAT__( "{}:{}:{}", id1, id2, type );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendReq );

        // 查询好友列表
        auto queryidlist = _relation_redis_driver->QueryList( "smembers {}:{}", __KF_STRING__( friendlist ), kfmsg.playerid() );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        KFMsg::S2SQueryFriendAck ack;
        ack.set_playerid( kfmsg.playerid() );

        for ( auto& strid : queryidlist->_value )
        {
            auto friendid = KFUtility::ToValue< uint32 >( strid );

            // 好友的基本信息
            auto publicdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), friendid );
            if ( publicdata->_value.empty() )
            {
                continue;
            }

            auto pbfriend = ack.add_pbfriend();
            MapStringToPBPlayer( publicdata->_value, friendid, pbfriend );

            // 好友的关系属性
            auto friendkey = FormatFriendKey( __KF_STRING__( friend ), kfmsg.playerid(), friendid );
            auto frienddata = _relation_redis_driver->QueryMap( "hgetall {}", friendkey );
            MapStringToPBRelation( frienddata->_value, pbfriend, false );
        }

        _kf_cluster_shard->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_QUERY_FRIEND_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryFriendInviteReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendInviteReq );

        auto queryidlist = _relation_redis_driver->QueryList( "smembers {}:{}", __KF_STRING__( invitelist ), kfmsg.playerid() );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        std::set< uint32 > removes;

        KFMsg::S2SQueryFriendInviteAck ack;
        ack.set_playerid( kfmsg.playerid() );

        for ( auto& strid : queryidlist->_value )
        {
            auto friendid = KFUtility::ToValue< uint32 >( strid );

            // 获得邀请的时间
            auto queryinvitedata = _relation_redis_driver->QueryMap( "hgetall {}:{}:{}", __KF_STRING__( invite ), kfmsg.playerid(), friendid );
            if ( queryinvitedata->_value.empty() )
            {
                removes.insert( friendid );
                continue;
            }

            // 好友的基本信息
            auto querypublicdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), friendid );
            if ( querypublicdata->_value.empty() )
            {
                removes.insert( friendid );
                continue;
            }

            auto pbfriend = ack.add_pbfriend();
            MapStringToPBPlayer( querypublicdata->_value, friendid, pbfriend );
            MapStringToPBRelation( queryinvitedata->_value, pbfriend, false );
        }

        _kf_cluster_shard->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_QUERY_FRIEND_INVITE_ACK, &ack );

        // 删除已经过期的邀请信息
        for ( auto id : removes )
        {
            _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( invitelist ), kfmsg.playerid(), id );
        }
        _relation_redis_driver->Pipeline();
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddFriendInviteReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddFriendInviteReq );

        // 查找对方的数据
        auto querytargetdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), kfmsg.targetplayerid() );
        if ( querytargetdata->_value.empty() )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::PublicDatabaseError );
        }

        auto refuseinvite = KFUtility::ToValue< uint32 >( querytargetdata->_value[ __KF_STRING__( refuseinvite ) ] );
        if ( refuseinvite != _invalid_int )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendRefuseInvite, kfmsg.targetname() );
        }

        // 查找对方的好友数量
        static auto _max_friend_count = _kf_option->GetValue< uint32 >( __KF_STRING__( freindmaxcount ) );
        auto queryfriendcount = _relation_redis_driver->QueryUInt64( "scard {}:{}", __KF_STRING__( friendlist ), kfmsg.targetplayerid() );
        if ( queryfriendcount->_value >= _max_friend_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendTargetLimit, kfmsg.targetname() );
        }

        // 查找对方申请列表是否有自己
        auto queryhaveself = _relation_redis_driver->QueryUInt64( "sismember {}:{} {}", __KF_STRING__( invitelist ), kfmsg.targetplayerid(), kfmsg.selfplayerid() );
        if ( queryhaveself->_value != _invalid_int )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendInviteAlready, kfmsg.targetname() );
        }

        // 查找对方申请列表数量
        static auto _max_invite_count = _kf_option->GetValue< uint32 >( __KF_STRING__( freindinvitecount ) );
        auto queryinvitecount = _relation_redis_driver->QueryUInt64( "scard {}:{}", __KF_STRING__( invitelist ), kfmsg.targetplayerid() );
        if ( queryinvitecount->_value >= _max_invite_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendInviteLimit, kfmsg.targetname() );
        }

        // 查找自己的数据
        auto queryselfdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), kfmsg.selfplayerid() );
        if ( queryselfdata->_value.empty() )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::PublicDatabaseError );
        }

        // 加入到申请列表
        static auto _invite_keep_time = _kf_option->GetValue< uint32 >( __KF_STRING__( freindinvitekeeptime ) );
        _relation_redis_driver->Append( "sadd {}:{} {}", __KF_STRING__( invitelist ), kfmsg.targetplayerid(), kfmsg.selfplayerid() );
        _relation_redis_driver->Append( "hset {}:{}:{} {} {}", __KF_STRING__( invite ), kfmsg.targetplayerid(), kfmsg.selfplayerid(), __KF_STRING__( time ), KFGlobal::Instance()->_real_time );
        _relation_redis_driver->Append( "hset {}:{}:{} {} {}", __KF_STRING__( invite ), kfmsg.targetplayerid(), kfmsg.selfplayerid(), __KF_STRING__( message ), kfmsg.message() );
        _relation_redis_driver->Append( "expire {}:{}:{} {}", __KF_STRING__( invite ), kfmsg.targetplayerid(), kfmsg.selfplayerid(), _invite_keep_time );
        _relation_redis_driver->Pipeline();

        _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendInviteOK );

        // 判断对方是否在线, 如果在线直接发送消息
        auto serverid = KFUtility::ToValue< uint32 >( querytargetdata->_value[ __KF_STRING__( serverid ) ] );
        if ( serverid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SAddFriendInviteAck ack;
        ack.set_playerid( kfmsg.targetplayerid() );

        auto pbfriend = ack.mutable_pbfriend();
        MapStringToPBPlayer( queryselfdata->_value, kfmsg.selfplayerid(), pbfriend );

        MapString friendvaluse;
        friendvaluse[ __KF_STRING__( message ) ] = kfmsg.message();
        MapStringToPBRelation( friendvaluse, pbfriend, true );
        _kf_cluster_shard->SendMessageToClient( serverid, KFMsg::S2S_ADD_FRIEND_INVITE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelInviteReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelFriendInviteReq );

        _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( invitelist ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Append( "del {}:{}:{}", __KF_STRING__( invite ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Pipeline();
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddFriendReq );

        static auto _max_friend_count = _kf_option->GetValue< uint32 >( __KF_STRING__( freindmaxcount ) );

        // 判断好友的数量
        auto selffriendcount = _relation_redis_driver->QueryUInt64( "scard {}:{}", __KF_STRING__( friendlist ), kfmsg.selfplayerid() );
        if ( selffriendcount->_value >= _max_friend_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendSelfLimit, _max_friend_count );
        }

        auto targetfriendcount = _relation_redis_driver->QueryUInt64( "scard {}:{}", __KF_STRING__( friendlist ), kfmsg.targetplayerid() );
        if ( targetfriendcount->_value >= _max_friend_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendTargetLimit, kfmsg.targetname() );
        }

        //添加好友 删除申请列表
        auto friendkey = FormatFriendKey( __KF_STRING__( friend ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Append( "hset {} {} {}", friendkey, __KF_STRING__( time ), KFGlobal::Instance()->_real_time );
        _relation_redis_driver->Append( "sadd {}:{} {}", __KF_STRING__( friendlist ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Append( "sadd {}:{} {}", __KF_STRING__( friendlist ), kfmsg.targetplayerid(), kfmsg.selfplayerid() );
        _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( invitelist ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Append( "del {}:{}:{}", __KF_STRING__( invite ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Pipeline();

        // 发送消息给游戏玩家
        {
            auto queryserverid = _public_redis_driver->QueryUInt32( "hget {}:{} {}", __KF_STRING__( public ), kfmsg.targetplayerid(), __KF_STRING__( serverid ) );
            if ( queryserverid->_value != _invalid_int )
            {
                auto querydata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), kfmsg.selfplayerid() );
                if ( !querydata->_value.empty() )
                {
                    SendAddFriendToClient( queryserverid->_value, querydata->_value, kfmsg.selfplayerid(), kfmsg.targetplayerid() );
                }
            }
        }

        {
            auto queryserverid = _public_redis_driver->QueryUInt32( "hget {}:{} {}", __KF_STRING__( public ), kfmsg.selfplayerid(), __KF_STRING__( serverid ) );
            if ( queryserverid->_value != _invalid_int )
            {
                auto querydata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), kfmsg.targetplayerid() );
                if ( !querydata->_value.empty() )
                {
                    SendAddFriendToClient( queryserverid->_value, querydata->_value, kfmsg.targetplayerid(), kfmsg.selfplayerid() );
                }
            }
        }
    }

    void KFRelationShardModule::SendAddFriendToClient( uint32 serverid, MapString& values, uint32 friendid, uint32 playerid )
    {
        KFMsg::S2SAddFriendAck ack;
        ack.set_playerid( playerid );

        auto pbfriend = ack.mutable_pbfriend();
        MapStringToPBPlayer( values, friendid, pbfriend );

        MapString friendvalues;
        MapStringToPBRelation( friendvalues, pbfriend, true );
        _kf_cluster_shard->SendMessageToClient( serverid, KFMsg::S2S_ADD_FRIEND_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelFriendReq );

        // 删除好友关系
        auto friendkey = FormatFriendKey( __KF_STRING__( friend ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Append( "del {}", friendkey );
        _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( friendlist ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( friendlist ), kfmsg.targetplayerid(), kfmsg.selfplayerid() );
        _relation_redis_driver->Pipeline();

        // 通知对方删除了好友
        auto queryserverid = _public_redis_driver->QueryUInt32( "hget {}:{} {}", __KF_STRING__( public ), kfmsg.targetplayerid(), __KF_STRING__( serverid ) );
        if ( queryserverid->_value != _invalid_int )
        {
            KFMsg::S2SDelFriendAck ack;
            ack.set_playerid( kfmsg.targetplayerid() );
            ack.set_targetplayerid( kfmsg.selfplayerid() );
            _kf_cluster_shard->SendMessageToClient( queryserverid->_value, KFMsg::S2S_DEL_FRIEND_ACK, &ack );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleUpdateFriendLinessReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateFriendLinessReq );

        UpdateFriendLiness( kfmsg.selfplayerid(), kfmsg.targetplayerid(), kfmsg.type(), kfmsg.friendliness() );
    }

    void KFRelationShardModule::UpdateFriendLiness( uint32 selfplayerid, uint32 targetplayerid, uint32 type, uint32 addvalue )
    {
        static auto _max_friend_liness = _kf_option->GetValue< uint32 >( __KF_STRING__( freindlinessmax ) );

        // 计算总好友度
        auto friendkey = FormatFriendKey( __KF_STRING__( friend ), selfplayerid, targetplayerid );
        auto querycurfriendliness = _relation_redis_driver->QueryUInt32( "hget {} {}", friendkey, __KF_STRING__( friendliness ) );
        auto nowfriendliness = querycurfriendliness->_value;
        if ( nowfriendliness >= _max_friend_liness )
        {
            return;
        }

        //计算当天剩余好感度
        auto friendlinesslimitkey = FormatFriendLimitKey( selfplayerid, targetplayerid, type );
        auto queryfriendliness = _relation_redis_driver->QueryUInt32( "hget {} {}", __KF_STRING__( friendlinesslimit ), friendlinesslimitkey );
        auto maxfriendliness = _kf_option->GetValue< uint32 >( __KF_STRING__( freindlinessdailymax ), type );
        if ( queryfriendliness->_value >= maxfriendliness )
        {
            return;
        }

        // 计算剩余总的好感度
        auto leftfriendliness = maxfriendliness - addvalue;
        auto addfriendliness = __MIN__( leftfriendliness, addvalue );
        auto restfriendliness = _max_friend_liness - nowfriendliness;
        addfriendliness = __MIN__( addfriendliness, restfriendliness );

        if ( addfriendliness > _invalid_int )
        {
            _relation_redis_driver->Append( "hincrby {} {} {}", friendkey, __KF_STRING__( friendliness ), addfriendliness );
            _relation_redis_driver->Append( "hincrby {} {} {}", __KF_STRING__( friendlinesslimit ), friendlinesslimitkey, addfriendliness );
            _relation_redis_driver->Pipeline();

            // 发送消息给玩家的好友
            SendAddFriendLinessToClient( selfplayerid, targetplayerid, addfriendliness );
            SendAddFriendLinessToClient( targetplayerid, selfplayerid, addfriendliness );
        }
    }

    void KFRelationShardModule::SendAddFriendLinessToClient( uint32 selfid, uint32 targetid, uint32 friendliness )
    {
        auto queryserverid = _public_redis_driver->QueryUInt32( "hget {}:{} {}", __KF_STRING__( public ), selfid, __KF_STRING__( serverid ) );
        if ( queryserverid->_value == _invalid_int )
        {
            return;
        }

        KFMsg::S2SUpdateFriendLinessAck ack;
        ack.set_playerid( selfid );
        ack.set_targetplayerid( targetid );
        ack.set_friendliness( friendliness );
        _kf_cluster_shard->SendMessageToClient( queryserverid->_value, KFMsg::S2S_UPDATE_FRIENDLINESS_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddRecentPlayerDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddRecentPlayerDataReq );

        static auto _recent_keep_time = _kf_option->GetValue< uint32 >( __KF_STRING__( recentkeeptime ) );
        static auto _recent_max_count = _kf_option->GetValue< uint32 >( __KF_STRING__( recentmaxcount ) );

        auto strbattlerecored = __FORMAT__( "{}:{}:{}", __KF_STRING__( battlerecored ), kfmsg.playerid(), kfmsg.roomid() );

        // 先保存战斗数据, 设置有效时间默认7天
        auto& strdata = KFProto::Serialize( &kfmsg.pbdata(), KFCompressEnum::Convert );
        _relation_redis_driver->Append( "set {} {}", strbattlerecored, strdata );
        _relation_redis_driver->Append( "expire {} {}", strbattlerecored, _recent_keep_time );
        _relation_redis_driver->Pipeline();

        // 添加到玩家列表
        for ( auto i = 0; i < kfmsg.members_size(); ++i )
        {
            auto strrecentplayer = __FORMAT__( "{}:{}", __KF_STRING__( recentplayer ), kfmsg.members( i ) );
            _relation_redis_driver->Execute( "zadd {} {} {}", strrecentplayer, kfmsg.roomid(), kfmsg.playerid() );

            // 判断数量, 超过限制数量, 删除最久远的记录
            // 战场id是按时间生成的, 所以最新的都是最大的
            auto querycount = _relation_redis_driver->QueryUInt64( "zcard {}", strrecentplayer );
            if ( querycount->_value > _recent_max_count )
            {
                _relation_redis_driver->Execute( "zremrangebyrank {} 0 0", strrecentplayer );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryRecentListReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRecentListReq );

        KFMsg::S2SQueryRecentListAck ack;
        ack.set_playerid( kfmsg.playerid() );

        // 查询列表
        auto queryrecentlist = _relation_redis_driver->QueryMap( "zrange {}:{} 0 -1 withscores", __KF_STRING__( recentplayer ), kfmsg.playerid() );
        for ( auto& iter : queryrecentlist->_value )
        {
            // 查询记录
            auto playerid = KFUtility::ToValue< uint32 >( iter.first );
            auto roomid = KFUtility::ToValue< uint64 >( iter.second );

            auto querydata = _relation_redis_driver->QueryString( "get {}:{}:{}", __KF_STRING__( battlerecored ), playerid, roomid );
            if ( !querydata->IsOk() )
            {
                continue;
            }

            // 找不到了 删除列表
            if ( querydata->_value.empty() )
            {
                _relation_redis_driver->Execute( "zrem {}:{} {}", __KF_STRING__( recentplayer ), kfmsg.playerid(), playerid );
                continue;
            }

            // 查询玩家的基础信息
            auto queryplayerdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), playerid );
            if ( queryplayerdata->_value.empty() )
            {
                continue;
            }

            auto pbrelation = ack.add_pbrelation();
            KFProto::Parse( pbrelation->mutable_relationdata(), querydata->_value, KFCompressEnum::Convert );
            MapStringToPBPlayer( queryplayerdata->_value, playerid, pbrelation );
        }

        _kf_cluster_shard->SendMessageToClient( kfguid, KFMsg::S2S_QUERY_RECENT_LIST_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandlePlayerToastReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerToastReq );

        auto queryserverid = _public_redis_driver->QueryUInt32( "hget {}:{} {}", __KF_STRING__( public ), kfmsg.targetplayerid(), __KF_STRING__( serverid ) );

        KFMsg::S2SPlayerToastAck ack;
        ack.set_playerid( kfmsg.selfplayerid() );
        ack.set_targetplayerid( kfmsg.targetplayerid() );
        ack.set_targetserverid( queryserverid->_value );

        // 判断被敬酒次数是否用完
        static auto _toast_recv_limit_count = _kf_option->GetValue< uint32 >( __KF_STRING__( toastrecvlimitcount ) );
        auto querytoastcount = _relation_redis_driver->QueryUInt32( "hget {} {}", __KF_STRING__( dailytoast ), kfmsg.targetplayerid() );
        if ( querytoastcount->IsOk() )
        {
            if ( querytoastcount->_value < _toast_recv_limit_count )
            {
                ack.set_result( KFMsg::ToastOK );

                // 增加玩家今日被敬酒次数和总的被敬酒次数
                _relation_redis_driver->Append( "hincrby {} {} 1", __KF_STRING__( dailytoast ), kfmsg.targetplayerid() );
                _relation_redis_driver->Append( "hincrby {}:{} {} 1", __KF_STRING__( toast ), kfmsg.targetplayerid(), __KF_STRING__( count ) );
                _relation_redis_driver->Pipeline();
            }
            else
            {
                ack.set_result( KFMsg::ToastGetCountOver );
            }
        }
        else
        {
            ack.set_result( KFMsg::RelationDatabaseBusy );
        }

        _kf_cluster_shard->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_PLAYER_TOAST_ACK, &ack );
    }


    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryToastCountReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryToastCountReq );

        auto querytoastcount = _relation_redis_driver->QueryUInt32( "hget {}:{} {}", __KF_STRING__( toast ), kfmsg.targetplayerid(), __KF_STRING__( count ) );

        KFMsg::MsgQueryToastCountAck ack;
        ack.set_playerid( kfmsg.targetplayerid() );
        ack.set_toastcount( querytoastcount->_value );
        _kf_cluster_shard->SendMessageToClient( kfguid, KFMsg::S2S_QUERY_TOAST_COUNT_ACK, &ack );
    }
}