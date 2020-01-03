#include "KFRelationShardModule.hpp"

namespace KFrame
{
    void KFRelationShardModule::BeforeRun()
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RELATION_TO_RELATION_REQ, &KFRelationShardModule::HandleQueryRelationToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RELATION_INVITE_TO_RELATION_REQ, &KFRelationShardModule::HandleQueryRelationInviteToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_APPLY_ADD_RELATION_TO_RELATION_REQ, &KFRelationShardModule::HandleApplyAddRelationToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_RELATION_INVITE_TO_RELATION_REQ, &KFRelationShardModule::HandleDelRelationInviteToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_RELATION_TO_RELATION_REQ, &KFRelationShardModule::HandleAddRelationToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_RELATION_TO_RELATION_REQ, &KFRelationShardModule::HandleDelRelationToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_RELATION_REQ, &KFRelationShardModule::HandleUpdateFriendLinessToRelationReq );
    }

    void KFRelationShardModule::BeforeShut()
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_QUERY_RELATION_TO_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_RELATION_INVITE_TO_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::S2S_APPLY_ADD_RELATION_TO_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEL_RELATION_INVITE_TO_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ADD_RELATION_TO_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEL_RELATION_TO_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_RELATION_REQ );
    }

    void KFRelationShardModule::PrepareRun()
    {
        _public_redis = _kf_redis->Create( __STRING__( public ) );
        _relation_redis_driver = _kf_redis->Create( __STRING__( logic ) );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationShardModule::MapStringToPBPlayer( StringMap& values, uint64 relationid, KFMsg::PBRelation* pbrelation )
    {
        pbrelation->set_playerid( relationid );

        auto pbplayerdata = pbrelation->mutable_playerdata();
        for ( auto& iter : values )
        {
            ( *pbplayerdata )[ iter.first ] = iter.second;
        }
    }

    void KFRelationShardModule::MapStringToPBRelation( StringMap& values, KFMsg::PBRelation* pbrelation, bool newadd )
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

        ( *pbdata )[ __STRING__( newadd ) ] = "1";
        ( *pbdata )[ __STRING__( time ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
    }

    std::string KFRelationShardModule::FormatRelationKey( uint64 firstid, uint64 secondid, const KFRelationSetting* kfsetting )
    {
        auto id1 = firstid;
        auto id2 = secondid;
        if ( kfsetting->_both_way )
        {
            id1 = __MIN__( firstid, secondid );
            id2 = __MAX__( firstid, secondid );
        }

        return __FORMAT__( "{}:{}:{}", kfsetting->_id, id1, id2 );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryRelationToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRelationToRelationReq );

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relatiion=[{}] no setting", kfmsg.dataname() );
        }

        // 查询好友列表
        auto queryidlist = _relation_redis_driver->QueryList( "smembers {}:{}", kfsetting->_data_list_name, kfmsg.playerid() );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        KFMsg::S2SQueryRelationToGameAck ack;
        ack.set_dataname( kfmsg.dataname() );
        ack.set_playerid( kfmsg.playerid() );

        for ( auto& strid : queryidlist->_value )
        {
            auto relationid = KFUtility::ToValue< uint64 >( strid );

            // 好友的基本信息
            auto publicdata = _public_redis->QueryMap( "hgetall {}:{}", __STRING__( public ), relationid );
            if ( publicdata->_value.empty() )
            {
                continue;
            }

            auto pbrelation = ack.add_pbrelation();
            MapStringToPBPlayer( publicdata->_value, relationid, pbrelation );

            // 好友的关系属性
            auto relationkey = FormatRelationKey( kfmsg.playerid(), relationid, kfsetting );
            auto relationdata = _relation_redis_driver->QueryMap( "hgetall {}", relationkey );
            MapStringToPBRelation( relationdata->_value, pbrelation, false );
        }

        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_RELATION_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryRelationInviteToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRelationInviteToRelationReq );

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relatiion=[{}] no setting", kfmsg.dataname() );
        }

        auto queryidlist = _relation_redis_driver->QueryList( "smembers {}:{}", kfsetting->_invite_list_name, kfmsg.playerid() );
        if ( queryidlist->_value.empty() )
        {
            return;
        }

        StringList removes;

        KFMsg::S2SQueryRelationInviteToGameAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_dataname( kfsetting->_invite_data_name );

        for ( auto& strid : queryidlist->_value )
        {
            auto friendid = KFUtility::ToValue< uint64 >( strid );

            // 获得邀请的数据
            auto queryinvitedata = _relation_redis_driver->QueryMap( "hgetall {}:{}:{}", kfsetting->_invite_data_name, kfmsg.playerid(), friendid );
            if ( queryinvitedata->_value.empty() )
            {
                removes.push_back( strid );
                continue;
            }

            // 好友的基本信息
            auto querypublicdata = _public_redis->QueryMap( "hgetall {}:{}", __STRING__( public ), friendid );
            if ( querypublicdata->_value.empty() )
            {
                removes.push_back( strid );
                continue;
            }

            auto pbinvite = ack.add_pbinvite();
            MapStringToPBPlayer( querypublicdata->_value, friendid, pbinvite );
            MapStringToPBRelation( queryinvitedata->_value, pbinvite, false );
        }
        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_RELATION_INVITE_TO_GAME_ACK, &ack );

        if ( !removes.empty() )
        {
            // 删除已经过期的邀请信息
            _relation_redis_driver->Update( removes, "srem {}:{}", kfsetting->_invite_list_name, kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleApplyAddRelationToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SApplyAddRelationToRelationReq );

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.dataname() );
        }

        auto selfid = __ROUTE_SEND_ID__;

        // 查找对方的数据
        auto querytargetdata = _public_redis->QueryMap( "hgetall {}:{}", __STRING__( public ), kfmsg.playerid() );
        if ( querytargetdata->_value.empty() )
        {
            return _kf_display->SendToPlayer( route, KFMsg::PublicDatabaseBusy );
        }

        // 是否已经设置拒绝
        auto refuseinvite = KFUtility::ToValue< uint64 >( querytargetdata->_value[ kfsetting->_refuse_name ] );
        if ( refuseinvite != _invalid_int )
        {
            return _kf_display->SendToPlayer( route, KFMsg::RelationRefuseInvite, kfmsg.playername() );
        }

        // 查找对方申请列表是否有自己
        auto queryhaveself = _relation_redis_driver->QueryUInt64( "sismember {}:{} {}", kfsetting->_invite_list_name, kfmsg.playerid(), selfid );
        if ( queryhaveself->_value != _invalid_int )
        {
            return _kf_display->SendToPlayer( route, KFMsg::RelationInviteAlready, kfmsg.playername() );
        }

        // 查找对方申请列表数量
        auto queryinvitecount = _relation_redis_driver->QueryUInt64( "scard {}:{}", kfsetting->_invite_list_name, kfmsg.playerid() );
        if ( queryinvitecount->_value >= kfsetting->_invite_data_count )
        {
            return _kf_display->SendToPlayer( route, KFMsg::RelationInviteLimit, kfmsg.playername() );
        }

        // 加入到申请列表
        auto invitekey = __FORMAT__( "{}:{}:{}", kfsetting->_invite_data_name, kfmsg.playerid(), selfid );
        _relation_redis_driver->Append( "sadd {}:{} {}", kfsetting->_invite_list_name, kfmsg.playerid(), selfid );
        _relation_redis_driver->Append( "hmset {} {} {} {} {}", invitekey, __STRING__( message ), kfmsg.message(), __STRING__( time ), KFGlobal::Instance()->_real_time );
        _relation_redis_driver->Append( "expire {} {}", invitekey, kfsetting->_invite_keep_time );
        _relation_redis_driver->Pipeline();
        _kf_display->SendToPlayer( route, KFMsg::RelationInviteOk, kfmsg.playername() );

        // 判断对方是否在线, 如果在线直接发送消息
        auto serverid = KFUtility::ToValue< uint64 >( querytargetdata->_value[ __STRING__( serverid ) ] );
        if ( serverid != _invalid_int )
        {
            // 查找自己的数据
            auto queryselfdata = _public_redis->QueryMap( "hgetall {}:{}", __STRING__( public ), selfid );
            if ( !queryselfdata->_value.empty() )
            {
                KFMsg::S2SApplyAddRelationToGameAck ack;
                ack.set_dataname( kfsetting->_invite_data_name );
                ack.set_playerid( kfmsg.playerid() );

                auto pbinvite = ack.mutable_pbinvite();
                MapStringToPBPlayer( queryselfdata->_value, selfid, pbinvite );

                StringMap friendvaluse;
                friendvaluse[ __STRING__( message ) ] = kfmsg.message();
                MapStringToPBRelation( friendvaluse, pbinvite, true );
                _kf_route->SendToPlayer( selfid, serverid, kfmsg.playerid(), KFMsg::S2S_APPLY_ADD_RELATION_TO_GAME_ACK, &ack, true );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelRelationInviteToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelRelationInviteToRelationReq );

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.dataname() );
        }

        _relation_redis_driver->Append( "srem {}:{} {}", kfsetting->_invite_list_name, kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Append( "del {}:{}:{}", kfsetting->_invite_data_name, kfmsg.selfplayerid(), kfmsg.targetplayerid() );
        _relation_redis_driver->Pipeline();
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddRelationToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddRelationToRelationReq );

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.dataname() );
        }

        auto selfid = __ROUTE_SEND_ID__;

        // 判断自己的关系数量
        {
            auto selfcount = _relation_redis_driver->QueryUInt64( "scard {}:{}", kfsetting->_data_list_name, selfid );
            if ( selfcount->_value >= kfsetting->_max_count )
            {
                return _kf_display->SendToPlayer( route, KFMsg::RelationSelfLimit, kfsetting->_max_count );
            }
        }

        // 判断对方关系的数量
        if ( kfsetting->_both_way )
        {
            auto targetfriendcount = _relation_redis_driver->QueryUInt64( "scard {}:{}", kfsetting->_data_list_name, kfmsg.playerid() );
            if ( targetfriendcount->_value >= kfsetting->_max_count )
            {
                return _kf_display->SendToPlayer( route, KFMsg::RelationTargetLimit, kfmsg.playername() );
            }
        }

        // 添加关系
        AddRelation( selfid, kfmsg.playerid(), kfsetting );
        if ( kfsetting->_both_way )
        {
            AddRelation( kfmsg.playerid(), selfid, kfsetting );
        }
    }

    void KFRelationShardModule::AddRelation( uint64 playerid, uint64 relationid, const KFRelationSetting* kfsetting )
    {
        auto relationkey = FormatRelationKey( playerid, relationid, kfsetting );
        _relation_redis_driver->Append( "hset {} {} {}", relationkey, __STRING__( time ), KFGlobal::Instance()->_real_time );

        // 添加列表
        _relation_redis_driver->Append( "sadd {}:{} {}", kfsetting->_data_list_name, playerid, relationid );

        // 删除申请列表
        if ( !kfsetting->_invite_data_name.empty() )
        {
            _relation_redis_driver->Append( "srem {}:{} {}", kfsetting->_invite_list_name, playerid, relationid );
            _relation_redis_driver->Append( "del {}:{}:{}", kfsetting->_invite_data_name, playerid, relationid );
        }
        _relation_redis_driver->Pipeline();

        // 发送消息给游戏玩家
        auto queryserverid = _public_redis->QueryUInt64( "hget {}:{} {}", __STRING__( public ), playerid, __STRING__( serverid ) );
        if ( queryserverid->_value != _invalid_int )
        {
            // 获得对方数据
            auto querydata = _public_redis->QueryMap( "hgetall {}:{}", __STRING__( public ), relationid );
            if ( !querydata->_value.empty() )
            {
                KFMsg::S2SAddRelationToGameAck ack;
                ack.set_playerid( playerid );
                ack.set_dataname( kfsetting->_id );

                auto pbrelation = ack.mutable_pbrelation();
                MapStringToPBPlayer( querydata->_value, relationid, pbrelation );

                StringMap friendvalues;
                MapStringToPBRelation( friendvalues, pbrelation, true );
                _kf_route->SendToPlayer( playerid, queryserverid->_value, playerid, KFMsg::S2S_ADD_RELATION_TO_GAME_ACK, &ack, true );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelRelationToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelRelationToRelationReq );

        auto selfid = __ROUTE_SEND_ID__;
        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.dataname() );
        }

        // 删除关系
        DelRelation( selfid, kfmsg.playerid(), kfsetting );
        if ( kfsetting->_both_way )
        {
            DelRelation( kfmsg.playerid(), selfid, kfsetting );
        }
    }

    void KFRelationShardModule::DelRelation( uint64 playerid, uint64 relationid, const KFRelationSetting* kfsetting )
    {
        // 删除好友关系
        auto relationkey = FormatRelationKey( playerid, relationid, kfsetting );

        _relation_redis_driver->Append( "del {}", relationkey );
        _relation_redis_driver->Append( "srem {}:{} {}", kfsetting->_data_list_name, playerid, relationid );
        _relation_redis_driver->Pipeline();

        auto queryserverid = _public_redis->QueryUInt64( "hget {}:{} {}", __STRING__( public ), playerid, __STRING__( serverid ) );
        if ( queryserverid->_value != _invalid_int )
        {
            KFMsg::S2SDelRelationToGameAck ack;
            ack.set_dataname( kfsetting->_id );
            ack.set_playerid( playerid );
            ack.set_relationid( relationid );
            _kf_route->SendToPlayer( playerid, queryserverid->_value, playerid, KFMsg::S2S_DEL_RELATION_TO_GAME_ACK, &ack, true );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleUpdateFriendLinessToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateFriendLinessToRelationReq );

        UpdateFriendLiness( kfmsg.selfplayerid(), kfmsg.targetplayerid(), kfmsg.type(), kfmsg.friendliness() );
    }

    void KFRelationShardModule::UpdateFriendLiness( uint64 selfplayerid, uint64 targetplayerid, uint32 type, uint32 addvalue )
    {
        //// 计算总好友度
        //auto friendkey = FormatRelationKey( __STRING__( friend ), selfplayerid, targetplayerid );

        //auto querycurfriendliness = _relation_redis_driver->QueryUInt64( "hget {} {}", friendkey, __STRING__( friendliness ) );
        //auto nowfriendliness = querycurfriendliness->_value;
        //if ( nowfriendliness >= _max_friend_liness->_uint32_value )
        //{
        //    return;
        //}

        //// 计算当天剩余好感度
        //auto friendlinesslimitkey = FormatFriendLimitKey( selfplayerid, targetplayerid, type );
        //auto queryfriendliness = _relation_redis_driver->QueryUInt64( "hget {} {}", __STRING__( friendlinesslimit ), friendlinesslimitkey );
        //auto maxfriendliness = _kf_option->GetUInt32( __STRING__( freindlinessdailymax ), type );
        //if ( queryfriendliness->_value >= maxfriendliness )
        //{
        //    return;
        //}

        //// 计算剩余总的好感度
        //auto leftfriendliness = maxfriendliness - addvalue;
        //auto addfriendliness = __MIN__( leftfriendliness, addvalue );
        //auto restfriendliness = _max_friend_liness->_uint32_value - nowfriendliness;
        //addfriendliness = __MIN__( addfriendliness, restfriendliness );

        //if ( addfriendliness > _invalid_int )
        //{
        //    _relation_redis_driver->Append( "hincrby {} {} {}", friendkey, __STRING__( friendliness ), addfriendliness );
        //    _relation_redis_driver->Append( "hincrby {} {} {}", __STRING__( friendlinesslimit ), friendlinesslimitkey, addfriendliness );
        //    _relation_redis_driver->Pipeline();

        //    // 发送消息给玩家的好友
        //    SendAddFriendLinessToPlayer( selfplayerid, targetplayerid, addfriendliness );
        //    SendAddFriendLinessToPlayer( targetplayerid, selfplayerid, addfriendliness );
        //}
    }

    void KFRelationShardModule::SendAddFriendLinessToPlayer( uint64 selfid, uint64 targetid, uint32 friendliness )
    {
        auto queryserverid = _public_redis->QueryUInt64( "hget {}:{} {}", __STRING__( public ), selfid, __STRING__( serverid ) );
        if ( queryserverid->_value == _invalid_int )
        {
            return;
        }

        KFMsg::S2SUpdateFriendLinessToGameAck ack;
        ack.set_playerid( selfid );
        ack.set_targetplayerid( targetid );
        ack.set_friendliness( friendliness );
        _kf_route->SendToPlayer( selfid, queryserverid->_value, selfid, KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK, &ack, true );
    }

    //__KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddRecentPlayerDataReq )
    //{
    //    __PROTO_PARSE__( KFMsg::S2SAddRecentPlayerDataReq );

    //    static auto _recent_keep_time = _kf_option->FindOption( __STRING__( recentkeeptime ) );
    //    static auto _recent_max_count = _kf_option->FindOption( __STRING__( recentmaxcount ) );

    //    auto strbattlerecored = __FORMAT__( "{}:{}:{}", __STRING__( battlerecored ), kfmsg.playerid(), kfmsg.roomid() );

    //    // 先保存战斗数据, 设置有效时间默认7天
    //    auto& strdata = KFProto::Serialize( &kfmsg.pbdata(), KFCompressEnum::Convert );
    //    _relation_redis_driver->Append( "set {} {}", strbattlerecored, strdata );
    //    _relation_redis_driver->Append( "expire {} {}", strbattlerecored, _recent_keep_time->_uint32_value );
    //    _relation_redis_driver->Pipeline();

    //    // 添加到玩家列表
    //    for ( auto i = 0; i < kfmsg.members_size(); ++i )
    //    {
    //        auto strrecentplayer = __FORMAT__( "{}:{}", __STRING__( recentplayer ), kfmsg.members( i ) );
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
    //    auto queryrecentlist = _relation_redis_driver->QueryMap( "zrange {}:{} 0 -1 withscores", __STRING__( recentplayer ), kfmsg.playerid() );
    //    for ( auto& iter : queryrecentlist->_value )
    //    {
    //        // 查询记录
    //        auto playerid = KFUtility::ToValue< uint64 >( iter.first );
    //        auto roomid = KFUtility::ToValue< uint64 >( iter.second );

    //        auto querydata = _relation_redis_driver->QueryString( "get {}:{}:{}", __STRING__( battlerecored ), playerid, roomid );
    //        if ( !querydata->IsOk() )
    //        {
    //            continue;
    //        }

    //        // 找不到了 删除列表
    //        if ( querydata->_value.empty() )
    //        {
    //            _relation_redis_driver->Execute( "zrem {}:{} {}", __STRING__( recentplayer ), kfmsg.playerid(), playerid );
    //            continue;
    //        }

    //        // 查询玩家的基础信息
    //        auto queryplayerdata = _public_redis_driver->QueryMap( "hgetall {}:{}", __STRING__( public ), playerid );
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