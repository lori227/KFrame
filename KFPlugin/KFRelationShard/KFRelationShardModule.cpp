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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationShardModule::MapStringToPBRelation( uint64 playerid, StringMap& values, KFMsg::PBRelation* pbrelation )
    {
        pbrelation->set_playerid( playerid );
        auto& pbrelationdata = *pbrelation->mutable_relationdata();
        __MAP_TO_PROTO__( values, pbrelationdata );
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

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.relationname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relatiion=[{}] no setting", kfmsg.relationname() );
        }

        RelationListType relationlist;
        _kf_relation_attribute->QueryRelationList( kfsetting->_relation_list_name, kfsetting->_id, kfmsg.playerid(), relationlist );
        if ( relationlist.empty() )
        {
            return;
        }

        KFMsg::S2SQueryRelationToGameAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_relationname( kfmsg.relationname() );
        for ( auto& iter : relationlist )
        {
            MapStringToPBRelation( iter.first, iter.second, ack.add_pbrelation() );
        }
        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_RELATION_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleQueryRelationInviteToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRelationInviteToRelationReq );

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.relationname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relatiion=[{}] no setting", kfmsg.relationname() );
        }

        RelationListType invitelist;
        _kf_relation_attribute->QueryInviteList( kfsetting->_invite_list_name, kfsetting->_invite_data_name, kfmsg.playerid(), invitelist );
        if ( invitelist.empty() )
        {
            return;
        }

        KFMsg::S2SQueryRelationInviteToGameAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_relationname( kfsetting->_invite_data_name );
        for ( auto& iter : invitelist )
        {
            MapStringToPBRelation( iter.first, iter.second, ack.add_pbinvite() );
        }
        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_RELATION_INVITE_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleApplyAddRelationToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SApplyAddRelationToRelationReq );

        auto selfid = __ROUTE_SEND_ID__;
        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.relationname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.relationname() );
        }

        // 是否已经设置拒绝
        auto refuseinvite = _kf_basic_attribute->QueryBasicIntValue( kfmsg.playerid(), kfsetting->_refuse_name );
        if ( refuseinvite != _invalid_int )
        {
            return _kf_display->SendToPlayer( route, KFMsg::RelationRefuseInvite, kfmsg.playername() );
        }

        // 查找对方申请列表是否有自己
        bool isrelationexist = _kf_relation_attribute->RelationExist( kfsetting->_invite_list_name, kfmsg.playerid(), selfid );
        if ( isrelationexist )
        {
            return _kf_display->SendToPlayer( route, KFMsg::RelationInviteAlready, kfmsg.playername() );
        }

        // 查找对方申请列表数量
        auto invitecount = _kf_relation_attribute->RelationCount( kfsetting->_invite_list_name, kfmsg.playerid() );
        if ( invitecount >= kfsetting->_invite_data_count )
        {
            return _kf_display->SendToPlayer( route, KFMsg::RelationInviteLimit, kfmsg.playername() );
        }

        _kf_relation_attribute->AddInvite( kfsetting->_invite_list_name, kfsetting->_invite_data_name, kfmsg.playerid(), selfid, kfmsg.message(), kfsetting->_invite_keep_time );
        _kf_display->SendToPlayer( route, KFMsg::RelationInviteOk, kfmsg.playername() );

        // 判断对方是否在线, 如果在线直接发送消息
        auto serverid = _kf_basic_attribute->QueryBasicServerId( kfmsg.playerid() );
        if ( serverid == _invalid_int )
        {
            return;
        }

        StringMap selfbasicdata;
        _kf_basic_attribute->QueryBasicAttribute( selfid, selfbasicdata );
        if ( selfbasicdata.empty() )
        {
            return;
        }

        selfbasicdata[ __STRING__( message ) ] = kfmsg.message();

        KFMsg::S2SApplyAddRelationToGameAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_relationname( kfsetting->_invite_data_name );
        MapStringToPBRelation( selfid, selfbasicdata, ack.mutable_pbinvite() );
        _kf_route->RepeatToPlayer( selfid, serverid, kfmsg.playerid(), KFMsg::S2S_APPLY_ADD_RELATION_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelRelationInviteToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelRelationInviteToRelationReq );

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.relationname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.relationname() );
        }

        _kf_relation_attribute->RemoveRelation( kfsetting->_invite_list_name, kfsetting->_invite_data_name, kfmsg.selfplayerid(), kfmsg.targetplayerid(), false );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleAddRelationToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddRelationToRelationReq );

        auto selfid = __ROUTE_SEND_ID__;
        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.relationname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.relationname() );
        }

        // 判断自己的关系数量
        auto selfcount = _kf_relation_attribute->RelationCount( kfsetting->_relation_list_name, selfid );
        if ( selfcount >= kfsetting->_max_count )
        {
            return _kf_display->SendToPlayer( route, KFMsg::RelationSelfLimit, kfsetting->_max_count );
        }

        // 判断对方关系的数量
        if ( kfsetting->_both_way )
        {
            auto targetcount = _kf_relation_attribute->RelationCount( kfsetting->_relation_list_name, kfmsg.playerid() );
            if ( targetcount >= kfsetting->_max_count )
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

    void KFRelationShardModule::AddRelation( uint64 playerid, uint64 targetid, const KFRelationSetting* kfsetting )
    {
        _kf_relation_attribute->AddRelation( kfsetting->_relation_list_name, kfsetting->_id, playerid, targetid, kfsetting->_both_way );
        if ( !kfsetting->_invite_data_name.empty() )
        {
            _kf_relation_attribute->RemoveRelation( kfsetting->_invite_list_name, kfsetting->_invite_data_name, playerid, targetid, false );
        }

        // 发送消息给游戏玩家
        auto serverid = _kf_basic_attribute->QueryBasicServerId( playerid );
        if ( serverid == _invalid_int )
        {
            return;
        }

        // 获得对方数据
        StringMap querytargtedata;
        _kf_basic_attribute->QueryBasicAttribute( targetid, querytargtedata );
        if ( querytargtedata.empty() )
        {
            return;
        }

        KFMsg::S2SAddRelationToGameAck ack;
        ack.set_playerid( playerid );
        ack.set_relationname( kfsetting->_id );
        MapStringToPBRelation( targetid, querytargtedata, ack.mutable_pbrelation() );
        _kf_route->RepeatToPlayer( playerid, serverid, playerid, KFMsg::S2S_ADD_RELATION_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationShardModule::HandleDelRelationToRelationReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelRelationToRelationReq );

        auto selfid = __ROUTE_SEND_ID__;
        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.relationname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.relationname() );
        }

        // 删除关系
        DelRelation( selfid, kfmsg.playerid(), kfsetting );
        if ( kfsetting->_both_way )
        {
            DelRelation( kfmsg.playerid(), selfid, kfsetting );
        }
    }

    void KFRelationShardModule::DelRelation( uint64 playerid, uint64 targetid, const KFRelationSetting* kfsetting )
    {
        _kf_relation_attribute->RemoveRelation( kfsetting->_relation_list_name, kfsetting->_id, playerid, targetid, kfsetting->_both_way );

        auto serverid = _kf_basic_attribute->QueryBasicServerId( playerid );
        if ( serverid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SDelRelationToGameAck ack;
        ack.set_playerid( playerid );
        ack.set_relationid( targetid );
        ack.set_relationname( kfsetting->_id );
        _kf_route->SendToPlayer( playerid, serverid, playerid, KFMsg::S2S_DEL_RELATION_TO_GAME_ACK, &ack );
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
        //auto queryserverid = _public_redis->QueryUInt64( "hget {}:{} {}", __STRING__( public ), selfid, __STRING__( serverid ) );
        //if ( queryserverid->_value == _invalid_int )
        //{
        //    return;
        //}

        //KFMsg::S2SUpdateFriendLinessToGameAck ack;
        //ack.set_playerid( selfid );
        //ack.set_targetplayerid( targetid );
        //ack.set_friendliness( friendliness );
        //_kf_route->SendToPlayer( selfid, queryserverid->_value, selfid, KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK, &ack, true );
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
    //        auto playerid = __TO_UINT64__( iter.first );
    //        auto roomid = __TO_UINT64__( iter.second );

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