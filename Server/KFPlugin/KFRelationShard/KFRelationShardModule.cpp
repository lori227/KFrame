#include "KFRelationShardModule.h"
#include "KFRelationConfig.h"

namespace KFrame
{
    static KFRedisDriver* _kf_public_redis = nullptr;
#define __PUBLIC_REDIS_DRIVER__ _kf_public_redis
    //#define __PUBLIC_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_public )


    static KFRedisDriver* _kf_relation_redis = nullptr;
#define __RELATION_REDIS_DRIVER__ _kf_relation_redis
    //#define __RELATION_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_relation )

    KFRelationShardModule::KFRelationShardModule()
    {
    }

    KFRelationShardModule::~KFRelationShardModule()
    {
    }

    void KFRelationShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_relation_config, true );
    }

    void KFRelationShardModule::BeforeRun()
    {
        auto kfsetting = _kf_schedule->CreateScheduleSetting();
        kfsetting->SetDate( KFScheduleEnum::Loop, 0, _kf_relation_config->_friend_liness_reset_time );
        _kf_schedule->RegisterSchedule( kfsetting, this, &KFRelationShardModule::OnScheduleClearFriendLiness );

        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_REQ, &KFRelationShardModule::HandleQueryFriendReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_REQ, &KFRelationShardModule::HandleQueryFriendInviteReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_REQ, &KFRelationShardModule::HandleAddFriendInviteReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_INVITE_REQ, &KFRelationShardModule::HandleDelInviteReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_REQ, &KFRelationShardModule::HandleAddFriendReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_REQ, &KFRelationShardModule::HandleDelFriendReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIEND_LINESS_REQ, &KFRelationShardModule::HandleUpdateFriendLinessReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_BATTLE_FRIEND_DATA_REQ, &KFRelationShardModule::HandleAddBattleFriendDataReq );
    }

    void KFRelationShardModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        _kf_schedule->UnRegisterSchedule( this );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIEND_LINESS_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_BATTLE_FRIEND_DATA_REQ );
    }

    void KFRelationShardModule::OnceRun()
    {
        _kf_public_redis = _kf_redis->CreateExecute( KFField::_public );
        _kf_relation_redis = _kf_redis->CreateExecute( KFField::_relation );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationShardModule::MapStringToPBPlayer( MapString& values, uint32 friendid, KFMsg::PBFriend* pbfriend )
    {
        pbfriend->set_playerid( friendid );

        for ( auto iter : values )
        {
            auto pbdata = pbfriend->add_playerdata();
            pbdata->set_name( iter.first );
            pbdata->set_value( iter.second );
        }
    }

    void KFRelationShardModule::MapStringToPBFriend( MapString& values, KFMsg::PBFriend* pbfriend, bool newadd )
    {
        for ( auto iter : values )
        {
            auto pbdata = pbfriend->add_frienddata();
            pbdata->set_name( iter.first );
            pbdata->set_value( iter.second );
        }

        if ( !newadd )
        {
            return;
        }

        {
            auto pbdata = pbfriend->add_frienddata();
            pbdata->set_name( KFField::_new_add );
            pbdata->set_value( "1" );
        }

        {
            auto pbdata = pbfriend->add_frienddata();
            pbdata->set_name( KFField::_time );
            pbdata->set_value( __KF_STRING__( KFGlobal::Instance()->_real_time ) );
        }
    }

    void KFRelationShardModule:: MapStringToPBBasic( MapString& values, KFMsg::PBStrings* pbstrings )
    {
        for ( auto iter : values )
        {
            auto pbdata = pbstrings->add_pbstring();

            pbdata->set_name( iter.first );
            pbdata->set_value( iter.second );
        }
    }

    const char* KFRelationShardModule::FormatFriendKey( const std::string& key, uint32 firstid, uint32 secondid )
    {
        static char _buffer[ 128 ] = "";

        auto id1 = __MIN__( firstid, secondid );
        auto id2 = __MAX__( firstid, secondid );

        sprintf( _buffer, "%s:%u:%u", key.c_str(), id1, id2 );
        return _buffer;
    }

    const char* KFRelationShardModule::FormatFriendLimitKey( uint32 firstid, uint32 secondid, uint32 type )
    {
        static char _buffer[ 128 ] = "";

        auto id1 = __MIN__( firstid, secondid );
        auto id2 = __MAX__( firstid, secondid );

        sprintf( _buffer, "%u:%u:%u", id1, id2, type );
        return _buffer;
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendReq );
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        // 查询最近的玩家
        VectorString emptyuidinfo;
        SendRecentListToClient( kfmsg.playerid(), KFOperateEnum::Set, emptyuidinfo );
        // 查询好友列表
        VectorString idlist;
        friendredisdriver->VectorExecute( idlist, "smembers %s:%u", KFField::_friend_list.c_str(), kfmsg.playerid() );

        if ( idlist.empty() )
        {
            return;
        }

        KFMsg::S2SQueryFriendAck ack;
        ack.set_playerid( kfmsg.playerid() );

        for ( auto& strid : idlist )
        {
            auto friendid = KFUtility::ToValue< uint32 >( strid );

            // 好友的基本信息
            MapString publicvalues;
            publicredisdriver->MapExecute( publicvalues, "hgetall %s:%u", KFField::_public.c_str(), friendid );

            if ( publicvalues.empty() )
            {
                continue;
            }

            auto pbfriend = ack.add_pbfriend();
            MapStringToPBPlayer( publicvalues, friendid, pbfriend );

            // 好友的关系属性
            MapString friendvalues;
            auto friendkey = FormatFriendKey( KFField::_friend, kfmsg.playerid(), friendid );
            friendredisdriver->MapExecute( friendvalues, "hgetall %s", friendkey );
            MapStringToPBFriend( friendvalues, pbfriend, false );
        }

        _kf_cluster_shard->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_QUERY_FRIEND_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryFriendInviteReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendInviteReq );
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        VectorString idlist;
        friendredisdriver->VectorExecute( idlist, "smembers %s:%u", KFField::_invite_list.c_str(), kfmsg.playerid() );

        if ( idlist.empty() )
        {
            return;
        }

        std::set< uint32 > removes;

        KFMsg::S2SQueryFriendInviteAck ack;
        ack.set_playerid( kfmsg.playerid() );

        for ( auto& strid : idlist )
        {
            auto friendid = KFUtility::ToValue< uint32 >( strid );

            // 获得邀请的时间
            MapString invitevalues;
            friendredisdriver->MapExecute( invitevalues, "hgetall %s:%u:%u", KFField::_invite.c_str(), kfmsg.playerid(), friendid );

            if ( invitevalues.empty() )
            {
                removes.insert( friendid );
                continue;
            }

            // 好友的基本信息
            MapString publicvalues;
            publicredisdriver->MapExecute( publicvalues, "hgetall %s:%u", KFField::_public.c_str(), friendid );

            if ( publicvalues.empty() )
            {
                continue;
            }

            auto pbfriend = ack.add_pbfriend();
            MapStringToPBPlayer( publicvalues, friendid, pbfriend );
            MapStringToPBFriend( invitevalues, pbfriend, false );
        }

        _kf_cluster_shard->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_QUERY_FRIEND_INVITE_ACK, &ack );

        // 删除已经过期的邀请信息
        for ( auto id : removes )
        {
            friendredisdriver->AppendCommand( "srem %s:%u %u", KFField::_invite_list.c_str(), kfmsg.playerid(), id );
        }

        friendredisdriver->PipelineExecute();
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddFriendInviteReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddFriendInviteReq );
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        // 查找对方的数据
        MapString targetvalues;
        publicredisdriver->MapExecute( targetvalues, "hgetall %s:%u", KFField::_public.c_str(), kfmsg.targetplayerid() );

        if ( targetvalues.empty() )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s:%u] can't find [%u] public data!",
                                __FUNCTION_LINE__, kfmsg.targetplayerid() );

            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::PublicDatabaseError );
        }

        auto refuseinvite = KFUtility::ToValue< uint32 >( targetvalues[ KFField::_refuse_invite ] );

        if ( refuseinvite != _invalid_int )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendRefuseInvite, kfmsg.targetname() );
        }

        // 查找对方的好友数量
        uint64 friendcount = _invalid_int;
        friendredisdriver->UInt64Execute( friendcount, "scard %s:%u", KFField::_friend_list.c_str(), kfmsg.targetplayerid() );

        if ( friendcount >= _kf_relation_config->_max_friend_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendTargetLimit, kfmsg.targetname() );
        }

        // 查找对方申请列表是否有自己
        uint64 haveself = _invalid_int;
        friendredisdriver->UInt64Execute( haveself, "sismember %s:%u %u", KFField::_invite_list.c_str(), kfmsg.targetplayerid(), kfmsg.selfplayerid() );

        if ( haveself != _invalid_int )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendInviteAlready, kfmsg.targetname() );
        }

        // 查找对方申请列表数量
        uint64 invitecount = _invalid_int;
        friendredisdriver->UInt64Execute( invitecount, "scard %s:%u", KFField::_invite_list.c_str(), kfmsg.targetplayerid() );

        if ( invitecount >= _kf_relation_config->_max_invite_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendInviteLimit, kfmsg.targetname() );
        }

        // 查找自己的数据
        MapString selfvalues;
        publicredisdriver->MapExecute( selfvalues, "hgetall %s:%u", KFField::_public.c_str(), kfmsg.selfplayerid() );

        if ( selfvalues.empty() )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s:%u] can't find [%u] public data!",
                                __FUNCTION_LINE__, kfmsg.selfplayerid() );

            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::PublicDatabaseError );
        }

        // 加入到申请列表
        {
            friendredisdriver->AppendCommand( "sadd %s:%u %u", KFField::_invite_list.c_str(), kfmsg.targetplayerid(), kfmsg.selfplayerid() );

            auto strtime = __KF_STRING__( KFGlobal::Instance()->_real_time );
            friendredisdriver->AppendCommand( "hset %s:%u:%u %s %s",
                                              KFField::_invite.c_str(), kfmsg.targetplayerid(), kfmsg.selfplayerid(), KFField::_time.c_str(), strtime.c_str() );
            friendredisdriver->AppendCommand( "hset %s:%u:%u %s %s",
                                              KFField::_invite.c_str(), kfmsg.targetplayerid(), kfmsg.selfplayerid(), KFField::_message.c_str(), kfmsg.message().c_str() );
            friendredisdriver->AppendCommand( "expire %s:%u:%u %u",
                                              KFField::_invite.c_str(), kfmsg.targetplayerid(), kfmsg.selfplayerid(), _kf_relation_config->_invite_keep_time );

            friendredisdriver->PipelineExecute();
        }

        _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendInviteOK );

        // 判断对方是否在线, 如果在线直接发送消息
        auto serverid = KFUtility::ToValue< uint32 >( targetvalues[ KFField::_server_id ] );

        if ( serverid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SAddFriendInviteAck ack;
        ack.set_playerid( kfmsg.targetplayerid() );

        auto pbfriend = ack.mutable_pbfriend();
        MapStringToPBPlayer( selfvalues, kfmsg.selfplayerid(), pbfriend );

        MapString friendvaluse;
        friendvaluse[ KFField::_message ] = kfmsg.message();
        MapStringToPBFriend( friendvaluse, pbfriend, true );
        _kf_cluster_shard->SendMessageToClient( serverid, KFMsg::S2S_ADD_FRIEND_INVITE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelInviteReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelFriendInviteReq );

        auto friendredisdriver = __RELATION_REDIS_DRIVER__;
        friendredisdriver->AppendCommand( "srem %s:%u %u", KFField::_invite_list.c_str(), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->AppendCommand( "del %s:%u:%u", KFField::_invite.c_str(), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->PipelineExecute();
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddFriendReq );
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        // 判断好友的数量
        uint64 selffriendcount = _invalid_int;
        friendredisdriver->UInt64Execute( selffriendcount, "scard %s:%u", KFField::_friend_list.c_str(), kfmsg.selfplayerid() );

        if ( selffriendcount >= _kf_relation_config->_max_friend_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendSelfLimit, _kf_relation_config->_max_friend_count );
        }

        uint64 targetfriendcount = _invalid_int;
        friendredisdriver->UInt64Execute( targetfriendcount, "scard %s:%u", KFField::_friend_list.c_str(), kfmsg.targetplayerid() );

        if ( targetfriendcount >= _kf_relation_config->_max_friend_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendTargetLimit, kfmsg.targetname() );
        }

        auto strtime = __KF_STRING__( KFGlobal::Instance()->_real_time );
        auto friendkey = FormatFriendKey( KFField::_friend, kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->AppendCommand( "hset %s %s %s", friendkey, KFField::_time.c_str(), strtime.c_str() );
        friendredisdriver->AppendCommand( "sadd %s:%u %u", KFField::_friend_list.c_str(), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->AppendCommand( "sadd %s:%u %u", KFField::_friend_list.c_str(), kfmsg.targetplayerid(), kfmsg.selfplayerid() );
        //添加好友 删除申请列表
        friendredisdriver->AppendCommand( "srem %s:%u %u", KFField::_invite_list.c_str(), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->AppendCommand( "del %s:%u:%u", KFField::_invite.c_str(), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->PipelineExecute();

        // 发送消息给游戏玩家
        {
            uint32 serverid = _invalid_int;
            publicredisdriver->UInt32Execute( serverid, "hget %s:%u %s", KFField::_public.c_str(), kfmsg.targetplayerid(), KFField::_server_id.c_str() );

            if ( serverid != _invalid_int )
            {
                MapString values;
                publicredisdriver->MapExecute( values, "hgetall %s:%u", KFField::_public.c_str(), kfmsg.selfplayerid() );
                SendAddFriendToClient( serverid, values, kfmsg.selfplayerid(), kfmsg.targetplayerid() );
            }
        }

        {
            uint32 serverid = _invalid_int;
            publicredisdriver->UInt32Execute( serverid, "hget %s:%u %s", KFField::_public.c_str(), kfmsg.selfplayerid(), KFField::_server_id.c_str() );

            if ( serverid != _invalid_int )
            {
                MapString values;
                publicredisdriver->MapExecute( values, "hgetall %s:%u", KFField::_public.c_str(), kfmsg.targetplayerid() );
                SendAddFriendToClient( serverid, values, kfmsg.targetplayerid(), kfmsg.selfplayerid() );
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
        MapStringToPBFriend( friendvalues, pbfriend, true );
        _kf_cluster_shard->SendMessageToClient( serverid, KFMsg::S2S_ADD_FRIEND_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelFriendReq );

        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        // 删除好友关系
        auto friendkey = FormatFriendKey( KFField::_friend, kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->AppendCommand( "del %s", friendkey );
        friendredisdriver->AppendCommand( "srem %s:%u %u", KFField::_friend_list.c_str(), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->AppendCommand( "srem %s:%u %u", KFField::_friend_list.c_str(), kfmsg.targetplayerid(), kfmsg.selfplayerid() );
        friendredisdriver->PipelineExecute();

        // 通知对方删除了好友
        auto serverid = _invalid_int;
        publicredisdriver->UInt32Execute( serverid, "hget %s:%u %s", KFField::_public.c_str(), kfmsg.targetplayerid(), KFField::_server_id.c_str() );

        if ( serverid != _invalid_int )
        {
            KFMsg::S2SDelFriendAck ack;
            ack.set_playerid( kfmsg.targetplayerid() );
            ack.set_targetplayerid( kfmsg.selfplayerid() );
            _kf_cluster_shard->SendMessageToClient( serverid, KFMsg::S2S_DEL_FRIEND_ACK, &ack );
        }
    }

    void KFRelationShardModule::OnScheduleClearFriendLiness( uint32 id, const char* data, uint32 size )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;
        friendredisdriver->VoidExecute( "del %s", KFField::_friend_liness_limit.c_str() );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleUpdateFriendLinessReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateFriendLinessReq );

        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        uint32 friendliness = _invalid_int;
        auto friendlinesskey = FormatFriendLimitKey( kfmsg.selfplayerid(), kfmsg.targetplayerid(), kfmsg.type() );
        friendredisdriver->UInt32Execute( friendliness, "hget %s %s", KFField::_friend_liness_limit.c_str(), friendlinesskey );

        //计算当天剩余好感度
        auto maxfriendliness = _kf_relation_config->getMaxFriendLine( kfmsg.type() );

        if ( friendliness >= maxfriendliness )
        {
            auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
            uint32 serverid = _invalid_int;
            publicredisdriver->UInt32Execute( serverid, "hget %s:%u %s", KFField::_public.c_str(), kfmsg.selfplayerid(), KFField::_server_id.c_str() );
            _kf_display->SendToGame( serverid, kfmsg.selfplayerid(), KFMsg::FriendLinessAddLimit );
            return;
        }

        auto leftfriendliness = maxfriendliness - friendliness;
        uint32 addfriendliness = __MIN__( leftfriendliness, kfmsg.friendliness() );

        auto friendkey = FormatFriendKey( KFField::_friend, kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->AppendCommand( "hincrby %s %s %u", KFField::_friend_liness_limit.c_str(), friendlinesskey, addfriendliness );
        friendredisdriver->AppendCommand( "hincrby %s %s %u", friendkey, KFField::_friend_liness.c_str(), addfriendliness );
        friendredisdriver->PipelineExecute();

        // 发送消息给玩家的好友
        SendAddFriendLinessToClient( kfmsg.selfplayerid(), kfmsg.targetplayerid(), addfriendliness );
        SendAddFriendLinessToClient( kfmsg.targetplayerid(), kfmsg.selfplayerid(), addfriendliness );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddBattleFriendDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddBattleFriendDataReq );

        _add_uids.clear();
        _del_uids.clear();

        for ( auto i = 0; i < kfmsg.recentdata_size(); ++i )
        {

            VectorString recent_uids_info;
            auto selfrecentdata = kfmsg.recentdata( i );

            for ( auto  j = 0; j < kfmsg.recentdata_size(); ++j )
            {
                // TODO:这边处理淘汰者数据 策划:暂不处理,因为无法得到淘汰者的战斗数据
                if ( i == j )
                {
                    continue;
                }

                auto tagrecentdata = kfmsg.recentdata( j );

                if ( IsFriend( selfrecentdata.playerid(), tagrecentdata.playerid() ) )
                {
                    continue;
                }

                if ( IsRecentFriend( selfrecentdata.playerid(), tagrecentdata.playerid() ) )
                {
                    _del_uids.insert( tagrecentdata.playerid() );
                }

                if ( !IsPublicPlayer( tagrecentdata.playerid() ) )
                {
                    // log error
                    continue;
                }

                auto& strdata = KFProto::Serialize( &tagrecentdata, KFCompressEnum::Convert );
                recent_uids_info.push_back( strdata );

                _add_uids.insert( tagrecentdata.playerid() );
            }

            DelRecentListToDB( selfrecentdata.playerid() );
            VectorString emptyuidinfo;
            SendRecentListToClient( selfrecentdata.playerid(), KFOperateEnum::Dec, emptyuidinfo );
            SaveRecentListToDB( selfrecentdata.playerid(), recent_uids_info );
            SendRecentListToClient( selfrecentdata.playerid(), KFOperateEnum::Add, recent_uids_info );

        }
    }


    void KFRelationShardModule::SendAddFriendLinessToClient( uint32 selfid, uint32 targetid, uint32 friendliness )
    {
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;

        uint32 serverid = _invalid_int;
        publicredisdriver->UInt32Execute( serverid, "hget %s:%u %s", KFField::_public.c_str(), selfid, KFField::_server_id.c_str() );

        if ( serverid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SUpdateFriendLinessAck ack;
        ack.set_playerid( selfid );
        ack.set_targetplayerid( targetid );
        ack.set_friendliness( friendliness );
        _kf_cluster_shard->SendMessageToClient( serverid, KFMsg::S2S_UPDATE_FRIENDLINESS_ACK, &ack );
    }

    bool KFRelationShardModule::IsFriend( uint32 playerid, uint32 targetid )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;
        uint64 isFriend = _invalid_int;
        friendredisdriver->UInt64Execute( isFriend, "sismember %s:%u %u",
                                          KFField::_friend_list.c_str(), playerid, targetid );

        return isFriend != _invalid_int;
    }

    bool KFRelationShardModule::IsRecentFriend( uint32 playerid, uint32 targetid )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;
        uint64 isrecentfriend = _invalid_int;
        friendredisdriver->UInt64Execute( isrecentfriend, "hexists %s:%u %u",
                                          KFField::_recent_hash.c_str(), playerid, targetid );

        return isrecentfriend != _invalid_int;
    }

    bool KFRelationShardModule::DelRecentListToDB( uint32 playerid )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;
        uint64 recentlistcount = _invalid_int;
        friendredisdriver->UInt64Execute( recentlistcount, "llen %s:%u", KFField::_recent_list.c_str(), playerid );
        // 计算需要移除的超过配置列表数量
        int popcount = recentlistcount - _del_uids.size() + _add_uids.size() - _kf_relation_config->_max_recent_list;

        // 计算超过配置需要移除的最近列表信息
        if ( popcount > 0 )
        {
            for ( auto i = _invalid_int; i < popcount; ++i )
            {
                uint32 removeid = _invalid_int;
                auto ok = friendredisdriver->UInt32Execute( removeid, "rpop %s:%u", KFField::_recent_list.c_str(), playerid );

                if ( _invalid_int != removeid )
                {
                    _del_uids.insert( removeid );
                    //friendredisdriver->AppendCommand( "hdel %s:%u %u", KFField::_recent_list.c_str(), playerid, removeid );
                }
            }
        }

        // 删除最近列表的玩家信息
        if ( _del_uids.size() > _invalid_int )
        {
            for ( auto iter : _del_uids )
            {
                // 从表头删除一个相同的最近玩家id
                friendredisdriver->AppendCommand( "lrem %s:%u 1 %u", KFField::_recent_list.c_str(), playerid, iter );
                friendredisdriver->AppendCommand( "hdel %s:%u %u", KFField::_recent_hash.c_str(), playerid, iter );
            }

            auto ok = friendredisdriver->PipelineExecute();
            return ok;
        }

        return true;
    }


    bool KFRelationShardModule::SaveRecentListToDB( uint32 playerid, VectorString& uidinfos )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        auto cursor = 0u;

        for ( auto iter : _add_uids )
        {
            friendredisdriver->AppendCommand( "lpush %s:%u %u",
                                              KFField::_recent_list.c_str(), playerid, iter );
            friendredisdriver->AppendCommand( "hset %s:%u %u %s",
                                              KFField::_recent_hash.c_str(), playerid, iter
                                              , uidinfos[ cursor ].c_str() );
            ++cursor;
        }

        auto ok = friendredisdriver->PipelineExecute();
        return ok;

    }

    void KFRelationShardModule::SendRecentListToClient( uint32 selfid, uint32 operate, VectorString& uidinfos )
    {
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;
        KFMsg::S2SModifyRecentListReq req;
        req.set_playerid( selfid );
        req.set_operate( operate );

        switch ( operate )
        {
        case KFOperateEnum::Add:
        {
            auto pbplayerids = req.mutable_uids();
            auto pbbasics = req.mutable_basicdatas();

            for ( auto iter : _add_uids )
            {
                MapString publicvalues;
                publicredisdriver->MapExecute( publicvalues, "hgetall %s:%u", KFField::_public.c_str(), iter );

                if ( publicvalues.empty() )
                {
                    // log
                    continue;
                }

                pbplayerids->add_playerid( iter );

                MapStringToPBBasic( publicvalues, pbbasics->add_basicdata() );
            }

            _add_uids.clear();
            KFMsg::PBUidsInfo* pbuidsinfos = req.mutable_uidsinfos();

            for ( auto iter : uidinfos )
            {
                pbuidsinfos->add_uidsinfo( iter );
            }
        }
        break;

        case KFOperateEnum::Dec:
        {
            for ( auto iter : _del_uids )
            {
                KFMsg::PBPlayerIds*  pbplayerids = req.mutable_uids();
                pbplayerids->add_playerid( iter );
            }

            _del_uids.clear();
        }
        break;

        case  KFOperateEnum::Set:
        {
            _add_uids.clear();
            MapString recentvalues;
            VectorString uidvalues;

            friendredisdriver->VectorExecute( uidvalues, "lrange %s:%u 0 -1", KFField::_recent_list.c_str(), selfid );
            friendredisdriver->MapExecute( recentvalues, "hgetall %s:%u", KFField::_recent_hash.c_str(), selfid );

            if ( recentvalues.empty() || uidvalues.empty()
                    || recentvalues.size() != uidvalues.size() )
            {
                // log
                return;
            }

            auto pbplayerids = req.mutable_uids();
            auto pbuidinfos = req.mutable_uidsinfos();

            // 排序
            for ( auto iter : uidvalues )
            {
                auto iterfind = recentvalues.find( iter );

                if ( iterfind != recentvalues.end() )
                {
                    pbplayerids->add_playerid( KFUtility::ToValue<uint32>( iter ) );
                    pbuidinfos->add_uidsinfo( iterfind->second );
                    _add_uids.insert( KFUtility::ToValue<uint32>( iter ) );
                }
            }

            auto pbbasics = req.mutable_basicdatas();

            for ( auto iter : _add_uids )
            {
                MapString publicvalues;
                publicredisdriver->MapExecute( publicvalues, "hgetall %s:%u", KFField::_public.c_str(), iter );

                if ( publicvalues.empty() )
                {
                    continue;
                }

                MapStringToPBBasic( publicvalues, pbbasics->add_basicdata() );
            }
        }
        break;

        default:
            break;
        }

        if ( req.uids().playerid_size() > _invalid_int )
        {
            uint32 serverid = _invalid_int;
            publicredisdriver->UInt32Execute( serverid, "hget %s:%u %s", KFField::_public.c_str(), selfid, KFField::_server_id.c_str() );

            if ( serverid == _invalid_int )
            {
                return;
            }

            _kf_cluster_shard->SendMessageToClient( serverid, KFMsg::S2S_MODIFY_RECENT_LIST_REQ, &req );
        }

    }


    bool KFRelationShardModule::IsPublicPlayer( uint32 playerid )
    {
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        MapString publicvalues;
        publicredisdriver->MapExecute( publicvalues, "hgetall %s:%u", KFField::_public.c_str(), playerid );

        return !publicvalues.empty();

    }
}