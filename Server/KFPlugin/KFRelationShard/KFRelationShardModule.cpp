#include "KFRelationShardModule.h"
#include "KFRelationConfig.h"

namespace KFrame
{
    static KFRedisDriver* _kf_public_redis = nullptr;
#define __PUBLIC_REDIS_DRIVER__ _kf_public_redis
    //#define __PUBLIC_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( public ) )


    static KFRedisDriver* _kf_relation_redis = nullptr;
#define __RELATION_REDIS_DRIVER__ _kf_relation_redis
    //#define __RELATION_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( relation ) )

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
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RECENT_LIST_REQ, &KFRelationShardModule::HandleQueryRecentListReq );

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
        _kf_public_redis = _kf_redis->CreateExecute( __KF_STRING__( public ) );
        _kf_relation_redis = _kf_redis->CreateExecute( __KF_STRING__( relation ) );
    }

    void KFRelationShardModule::OnScheduleClearFriendLiness( uint32 id, const char* data, uint32 size )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;
        friendredisdriver->Execute( __FUNC_LINE__, "del {}", __KF_STRING__( friendlinesslimit ) );
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
            pbdata->set_name( __KF_STRING__( newadd ) );
            pbdata->set_value( "1" );
        }

        {
            auto pbdata = pbfriend->add_frienddata();
            pbdata->set_name( __KF_STRING__( time ) );
            pbdata->set_value( __TO_STRING__( KFGlobal::Instance()->_real_time ) );
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
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        //VectorString emptyuidinfo;
        //  SendRecentListToClient( kfmsg.playerid(), KFOperateEnum::Set, emptyuidinfo );

        // 查询好友列表
        auto queryidlist = friendredisdriver->QueryList( __FUNC_LINE__, "smembers {}:{}",
                           __KF_STRING__( friendlist ), kfmsg.playerid() );
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
            auto publicdata = publicredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}",
                              __KF_STRING__( public ), friendid );

            if ( publicdata->_value.empty() )
            {
                continue;
            }

            auto pbfriend = ack.add_pbfriend();
            MapStringToPBPlayer( publicdata->_value, friendid, pbfriend );

            // 好友的关系属性

            auto friendkey = FormatFriendKey( __KF_STRING__( friend ), kfmsg.playerid(), friendid );

            auto frienddata = friendredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}", friendkey );
            MapStringToPBFriend( frienddata->_value, pbfriend, false );
        }

        _kf_cluster_shard->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_QUERY_FRIEND_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryFriendInviteReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendInviteReq );
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        auto queryidlist = friendredisdriver->QueryList( __FUNC_LINE__, "smembers {}:{}",
                           __KF_STRING__( invitelist ), kfmsg.playerid() );
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
            auto queryinvitedata = friendredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}:{}",
                                   __KF_STRING__( invite ), kfmsg.playerid(), friendid );
            if ( queryinvitedata->_value.empty() )
            {
                removes.insert( friendid );
                continue;
            }

            MapString invitedata;
            invitedata.swap( queryinvitedata->_value );

            // 好友的基本信息
            auto querypublicdata = publicredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}",
                                   __KF_STRING__( public ), friendid );
            if ( querypublicdata->_value.empty() )
            {
                removes.insert( friendid );
                continue;
            }

            MapString publicdata;
            publicdata.swap( querypublicdata->_value );


            auto pbfriend = ack.add_pbfriend();
            MapStringToPBPlayer( publicdata, friendid, pbfriend );
            MapStringToPBFriend( invitedata, pbfriend, false );
        }

        _kf_cluster_shard->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_QUERY_FRIEND_INVITE_ACK, &ack );

        // 删除已经过期的邀请信息
        for ( auto id : removes )
        {
            friendredisdriver->Append( "srem {}:{} {}", __KF_STRING__( invitelist ), kfmsg.playerid(), id );
        }

        friendredisdriver->Pipeline( __FUNC_LINE__ );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddFriendInviteReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddFriendInviteReq );
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        // 查找对方的数据
        auto querytargetdata =  publicredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}",
                                __KF_STRING__( public ), kfmsg.targetplayerid() );
        if ( querytargetdata->_value.empty() )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::PublicDatabaseError );
        }

        MapString targetdata;
        targetdata.swap( querytargetdata->_value );

        auto refuseinvite = KFUtility::ToValue< uint32 >( targetdata[ __KF_STRING__( refuseinvite ) ] );
        if ( refuseinvite != _invalid_int )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendRefuseInvite, kfmsg.targetname() );
        }

        // 查找对方的好友数量
        auto queryfriendcount = friendredisdriver->QueryUInt64( __FUNC_LINE__, "scard {}:{}",
                                __KF_STRING__( friendlist ), kfmsg.targetplayerid() );
        if ( queryfriendcount->_value >= _kf_relation_config->_max_friend_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendTargetLimit, kfmsg.targetname() );
        }

        // 查找对方申请列表是否有自己
        auto queryhaveself = friendredisdriver->QueryUInt64( __FUNC_LINE__, "sismember {}:{} {}",
                             __KF_STRING__( invitelist ), kfmsg.targetplayerid(), kfmsg.selfplayerid() );
        if ( queryhaveself->_value != _invalid_int )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendInviteAlready, kfmsg.targetname() );
        }

        // 查找对方申请列表数量
        auto queryinvitecount = friendredisdriver->QueryUInt64( __FUNC_LINE__, "scard {}:{}",
                                __KF_STRING__( invitelist ), kfmsg.targetplayerid() );
        if ( queryinvitecount->_value >= _kf_relation_config->_max_invite_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendInviteLimit, kfmsg.targetname() );
        }

        // 查找自己的数据
        auto queryselfdata = publicredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}",
                             __KF_STRING__( public ), kfmsg.selfplayerid() );
        if ( queryselfdata->_value.empty() )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::PublicDatabaseError );
        }

        MapString selfdata;
        selfdata.swap( queryselfdata->_value );

        // 加入到申请列表
        friendredisdriver->Append( "sadd {}:{} {}",
                                   __KF_STRING__( invitelist ), kfmsg.targetplayerid(), kfmsg.selfplayerid() );

        friendredisdriver->Append( "hset {}:{}:{} {} {}",
                                   __KF_STRING__( invite ), kfmsg.targetplayerid(), kfmsg.selfplayerid(),
                                   __KF_STRING__( time ), KFGlobal::Instance()->_real_time );
        friendredisdriver->Append( "hset {}:{}:{} {} {}",
                                   __KF_STRING__( invite ), kfmsg.targetplayerid(), kfmsg.selfplayerid(),
                                   __KF_STRING__( message ), kfmsg.message() );
        friendredisdriver->Append( "expire {}:{}:{} {}",
                                   __KF_STRING__( invite ), kfmsg.targetplayerid(), kfmsg.selfplayerid(),
                                   _kf_relation_config->_invite_keep_time );

        friendredisdriver->Pipeline( __FUNC_LINE__ );

        _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendInviteOK );

        // 判断对方是否在线, 如果在线直接发送消息
        auto serverid = KFUtility::ToValue< uint32 >( targetdata[ __KF_STRING__( serverid ) ] );
        if ( serverid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SAddFriendInviteAck ack;
        ack.set_playerid( kfmsg.targetplayerid() );

        auto pbfriend = ack.mutable_pbfriend();
        MapStringToPBPlayer( selfdata, kfmsg.selfplayerid(), pbfriend );

        MapString friendvaluse;
        friendvaluse[ __KF_STRING__( message ) ] = kfmsg.message();
        MapStringToPBFriend( friendvaluse, pbfriend, true );
        _kf_cluster_shard->SendMessageToClient( serverid, KFMsg::S2S_ADD_FRIEND_INVITE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelInviteReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelFriendInviteReq );

        auto friendredisdriver = __RELATION_REDIS_DRIVER__;
        friendredisdriver->Append( "srem {}:{} {}", __KF_STRING__( invitelist ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->Append( "del {}:{}:{}", __KF_STRING__( invite ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->Pipeline( __FUNC_LINE__ );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddFriendReq );
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        // 判断好友的数量
        auto selffriendcount = friendredisdriver->QueryUInt64( __FUNC_LINE__, "scard {}:{}",
                               __KF_STRING__( friendlist ), kfmsg.selfplayerid() );
        if ( selffriendcount->_value >= _kf_relation_config->_max_friend_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendSelfLimit, _kf_relation_config->_max_friend_count );
        }

        auto targetfriendcount = friendredisdriver->QueryUInt64( __FUNC_LINE__, "scard {}:{}",
                                 __KF_STRING__( friendlist ), kfmsg.targetplayerid() );
        if ( targetfriendcount->_value >= _kf_relation_config->_max_friend_count )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.selfplayerid(), KFMsg::FriendTargetLimit, kfmsg.targetname() );
        }

        auto friendkey = FormatFriendKey( __KF_STRING__( friend ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->Append( "hset {} {} {}", friendkey, __KF_STRING__( time ), KFGlobal::Instance()->_real_time );
        friendredisdriver->Append( "sadd {}:{} {}", __KF_STRING__( friendlist ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->Append( "sadd {}:{} {}", __KF_STRING__( friendlist ), kfmsg.targetplayerid(), kfmsg.selfplayerid() );

        //添加好友 删除申请列表
        friendredisdriver->Append( "srem {}:{} {}", __KF_STRING__( invitelist ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->Append( "del {}:{}:{}", __KF_STRING__( invite ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->Pipeline( __FUNC_LINE__ );

        // 发送消息给游戏玩家
        {
            auto queryserverid = publicredisdriver->QueryUInt32( __FUNC_LINE__, "hget {}:{} {}",
                                 __KF_STRING__( public ), kfmsg.targetplayerid(), __KF_STRING__( serverid ) );
            if ( queryserverid->_value != _invalid_int )
            {
                auto querydata = publicredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}",
                                 __KF_STRING__( public ), kfmsg.selfplayerid() );
                if ( !querydata->_value.empty() )
                {
                    SendAddFriendToClient( queryserverid->_value, querydata->_value, kfmsg.selfplayerid(), kfmsg.targetplayerid() );
                }
            }
        }

        {
            auto queryserverid = publicredisdriver->QueryUInt32( __FUNC_LINE__, "hget {}:{} {}",
                                 __KF_STRING__( public ), kfmsg.selfplayerid(), __KF_STRING__( serverid ) );
            if ( queryserverid->_value != _invalid_int )
            {
                auto querydata = publicredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}",
                                 __KF_STRING__( public ), kfmsg.targetplayerid() );
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
        MapStringToPBFriend( friendvalues, pbfriend, true );
        _kf_cluster_shard->SendMessageToClient( serverid, KFMsg::S2S_ADD_FRIEND_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelFriendReq );

        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        // 删除好友关系
        auto friendkey = FormatFriendKey( __KF_STRING__( friend ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->Append( "del {}", friendkey );
        friendredisdriver->Append( "srem {}:{} {}", __KF_STRING__( friendlist ), kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        friendredisdriver->Append( "srem {}:{} {}", __KF_STRING__( friendlist ), kfmsg.targetplayerid(), kfmsg.selfplayerid() );
        friendredisdriver->Pipeline(  __FUNC_LINE__ );

        // 通知对方删除了好友
        auto queryserverid = publicredisdriver->QueryUInt32( __FUNC_LINE__, "hget {}:{} {}",
                             __KF_STRING__( public ), kfmsg.targetplayerid(), __KF_STRING__( serverid ) );

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

        UpdateFriendLiness( kfmsg.selfplayerid(), kfmsg.targetplayerid(), kfmsg.friendliness(), kfmsg.type() );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddBattleFriendDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddBattleFriendDataReq );

        _add_uids.clear();
        _del_uids.clear();

        //处理组队战斗结算好感度
        HandleBattleFrinedLiness( data, length );

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

                auto& strdata = KFProto::Serialize( &tagrecentdata, KFCompressEnum::Convert );
                recent_uids_info.push_back( strdata );

                _add_uids.insert( tagrecentdata.playerid() );
            }

            DelRecentListToDB( selfrecentdata.playerid() );
            //  VectorString emptyuidinfo;
            // SendRecentListToClient( selfrecentdata.playerid(), KFOperateEnum::Dec, emptyuidinfo );
            SaveRecentListToDB( selfrecentdata.playerid(), recent_uids_info );
            _add_uids.clear();
            _del_uids.clear();
            // SendRecentListToClient( selfrecentdata.playerid(), KFOperateEnum::Add, recent_uids_info );

        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryRecentListReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRecentListReq );
        VectorString emptyuidinfo;
        SendRecentListToClient( kfmsg.playerid(), KFOperateEnum::Set, emptyuidinfo );
    }

    void KFRelationShardModule::SendAddFriendLinessToClient( uint32 selfid, uint32 targetid, uint32 friendliness )
    {
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;

        auto queryserverid = publicredisdriver->QueryUInt32( __FUNC_LINE__, "hget {}:{} {}",
                             __KF_STRING__( public ), selfid, __KF_STRING__( serverid ) );
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

    bool KFRelationShardModule::IsFriend( uint32 playerid, uint32 targetid )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        auto isfriend =  friendredisdriver->QueryUInt64( __FUNC_LINE__, "sismember {}:{} {}",
                         __KF_STRING__( friendlist ), playerid, targetid );

        return isfriend->_value != _invalid_int;
    }

    bool KFRelationShardModule::IsRecentFriend( uint32 playerid, uint32 targetid )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;
        auto isrecentfriend = friendredisdriver->QueryUInt64( __FUNC_LINE__, "hexists {}:{} {}",
                              __KF_STRING__( recenthash ), playerid, targetid );

        return isrecentfriend->_value != _invalid_int;
    }

    void KFRelationShardModule::DelRecentListToDB( uint32 playerid )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        auto recentlistcount = friendredisdriver->QueryUInt64( __FUNC_LINE__, "llen {}:{}",
                               __KF_STRING__( recentlist ), playerid );
        if ( recentlistcount->_value == _invalid_int )
        {
            return;
        }

        // 计算需要移除的超过配置列表数量
        int popcount = recentlistcount->_value - _del_uids.size() + _add_uids.size() - _kf_relation_config->_max_recent_list;

        // 计算超过配置需要移除的最近列表信息
        if ( popcount > 0 )
        {
            for ( auto i = 0; i < popcount; ++i )
            {
                auto removeid = friendredisdriver->QueryUInt32( __FUNC_LINE__, "rpop {}:{}",
                                __KF_STRING__( recentlist ), playerid );

                if ( removeid->_value != _invalid_int )
                {
                    _del_uids.insert( removeid->_value );
                }
            }
        }

        // 删除最近列表的玩家信息
        if ( !_del_uids.empty() )
        {
            for ( auto uid : _del_uids )
            {
                // 从表头删除一个相同的最近玩家id
                friendredisdriver->Append( "lrem {}:{} 1 {}", __KF_STRING__( recentlist ), playerid, uid );
                friendredisdriver->Append( "hdel {}:{} {}", __KF_STRING__( recenthash ), playerid, uid );
            }

            friendredisdriver->Pipeline( __FUNC_LINE__ );
        }
    }


    void KFRelationShardModule::SaveRecentListToDB( uint32 playerid, VectorString& uidinfos )
    {
        auto friendredisdriver = __RELATION_REDIS_DRIVER__;

        auto cursor = 0u;

        for ( auto iter : _add_uids )
        {
            friendredisdriver->Append( "lpush {}:{} {}",
                                       __KF_STRING__( recentlist ), playerid, iter );
            friendredisdriver->Append( "hset {}:{} {} {}",
                                       __KF_STRING__( recenthash ), playerid, iter
                                       , uidinfos[ cursor ] );
            ++cursor;
        }

        friendredisdriver->Pipeline( __FUNC_LINE__ );
    }

    void KFRelationShardModule::SendRecentListToClient( uint32 selfid, uint32 operate, VectorString& uidinfos )
    {
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto frienddriver = __RELATION_REDIS_DRIVER__;
        KFMsg::S2SQueryRecentListAck req;
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
                auto publicdata = publicredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}",
                                  __KF_STRING__( public ), iter );
                if ( publicdata->_value.empty() )
                {
                    continue;
                }

                pbplayerids->add_playerid( iter );

                MapStringToPBBasic( publicdata->_value, pbbasics->add_basicdata() );
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
            auto uidvalues =  frienddriver->QueryList( __FUNC_LINE__, "lrange {}:{} 0 -1",
                              __KF_STRING__( recentlist ), selfid );

            auto recentvalues = frienddriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}",
                                __KF_STRING__( recenthash ), selfid );

            if ( recentvalues->_value.empty() || uidvalues->_value.empty()
                    || recentvalues->_value.size() != uidvalues->_value.size() )
            {
                return;
            }

            auto pbplayerids = req.mutable_uids();
            auto pbuidinfos = req.mutable_uidsinfos();

            // 排序
            for ( auto iter : uidvalues->_value )
            {
                auto iterfind = recentvalues->_value.find( iter );

                if ( iterfind != recentvalues->_value.end() )
                {
                    auto addplayerid = KFUtility::ToValue<uint32>( iter );
                    pbplayerids->add_playerid( addplayerid );
                    pbuidinfos->add_uidsinfo( iterfind->second );
                    _add_uids.insert( addplayerid );
                }
            }

            auto pbbasics = req.mutable_basicdatas();

            for ( auto iter : _add_uids )
            {
                auto publicvalues = publicredisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}",
                                    __KF_STRING__( public ), iter );
                if ( publicvalues->_value.empty() )
                {
                    continue;
                }

                MapStringToPBBasic( publicvalues->_value, pbbasics->add_basicdata() );
            }
        }
        break;

        default:
            break;
        }

        if ( req.uids().playerid_size() > _invalid_int )
        {
            auto serverid = publicredisdriver->QueryUInt32( __FUNC_LINE__, "hget {}:{} {}",
                            __KF_STRING__( public ), selfid, __KF_STRING__( serverid ) );
            if ( serverid->_value != _invalid_int )
            {
                _kf_cluster_shard->SendMessageToClient( serverid->_value, KFMsg::S2S_QUERY_RECENT_LIST_ACK, &req );
            }
        }

    }

    void KFRelationShardModule::UpdateFriendLiness( uint32 selfplayerid, uint32 targetplayerid, uint32 addvalue, uint32 type )
    {
        auto frienddriver = __RELATION_REDIS_DRIVER__;
        auto maxtotalFriendLines = _kf_relation_config->GetMaxFriendLine();

        // 计算总好友度
        auto friendkey = FormatFriendKey( __KF_STRING__( friend ), selfplayerid, targetplayerid );

        auto querycurfriendliness = frienddriver->QueryUInt32( __FUNC_LINE__, "hget {} {}",
                                    friendkey, __KF_STRING__( friendliness ) );
        auto curfriendliness = querycurfriendliness->_value;
        if ( curfriendliness >= maxtotalFriendLines )
        {
            auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;

            auto serverid = publicredisdriver->QueryUInt32( __FUNC_LINE__, "hget {}:{} {}",
                            __KF_STRING__( public ), selfplayerid, __KF_STRING__( serverid ) );

            auto targetname = publicredisdriver->QueryString( __FUNC_LINE__, "hget {}:{} {}",
                              __KF_STRING__( public ), targetplayerid, __KF_STRING__( name ) );

            return _kf_display->SendToGame( serverid->_value, selfplayerid, KFMsg::FriendLinessMaxLimit, targetname->_value );
        }

        auto friendlinesskey = FormatFriendLimitKey( selfplayerid, targetplayerid, type );
        auto queryfriendliness = frienddriver->QueryUInt32( __FUNC_LINE__, "hget {} {}",
                                 __KF_STRING__( friendlinesslimit ), friendlinesskey );

        //计算当天剩余好感度
        auto maxfriendliness = _kf_relation_config->GetMaxFriendLine( type );
        if ( queryfriendliness->_value >= maxfriendliness )
        {
            return;
        }

        auto leftfriendliness = maxfriendliness - addvalue;
        uint32 addfriendliness = __MIN__( leftfriendliness, addvalue );

        // 计算剩余总的好感度
        auto restfriendliness = maxtotalFriendLines - curfriendliness;
        addfriendliness = __MIN__( addfriendliness, restfriendliness );

        if ( _invalid_int < addfriendliness )
        {
            frienddriver->Append( "hincrby {} {} {}", __KF_STRING__( friendlinesslimit ), friendlinesskey, addfriendliness );
            frienddriver->Append( "hincrby {} {} {}", friendkey, __KF_STRING__( friendliness ), addfriendliness );
            frienddriver->Pipeline( __FUNC_LINE__ );

            // 发送消息给玩家的好友
            SendAddFriendLinessToClient( selfplayerid, targetplayerid, addfriendliness );
            SendAddFriendLinessToClient( targetplayerid, selfplayerid, addfriendliness );
        }
    }

    void KFRelationShardModule::HandleBattleFrinedLiness( const char* data, uint32 length )
    {
        __PROTO_PARSE__( KFMsg::S2SAddBattleFriendDataReq );

        // 未组队不进行处理
        if ( 1 >= kfmsg.recentdata_size() )
        {
            return;
        }

        for ( auto i = 0; i < kfmsg.recentdata_size(); ++i )
        {
            auto selfrecentdata = kfmsg.recentdata( i );
            for ( auto j = i + 1 ; j < kfmsg.recentdata_size(); ++j )
            {
                auto tagrecentdata = kfmsg.recentdata( j );

                if ( IsFriend( selfrecentdata.playerid(), tagrecentdata.playerid() ) )
                {
                    UpdateFriendLiness( selfrecentdata.playerid(), tagrecentdata.playerid(), _kf_relation_config->_max_friend_count, KFMsg::FriendLinessEnum::Team );
                }
            }
        }
    }
}