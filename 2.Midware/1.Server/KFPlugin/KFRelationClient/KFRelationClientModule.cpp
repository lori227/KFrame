#include "KFRelationClientModule.h"

namespace KFrame
{
    void KFRelationClientModule::BeforeRun()
    {
        // 注册属性回调
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterUpdateDataModule( this, &KFRelationClientModule::OnRelationValueUpdate );
        _kf_component->RegisterUpdateStringModule( this, &KFRelationClientModule::OnRelationStringUpdate );

        _kf_player->RegisterEnterFunction( this, &KFRelationClientModule::OnEnterQueryFriend );
        _kf_player->RegisterLeaveFunction( this, &KFRelationClientModule::OnLeaveUpdateFriend );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_ADD_FRIEND_INVITE_REQ, &KFRelationClientModule::HandleAddFriendInviteReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REPLY_FRIEND_INVITE_REQ, &KFRelationClientModule::HandleReplyInviteReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_DEL_FRIEND_REQ, &KFRelationClientModule::HandleDelFriendReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_REFUSE_FRIEND_INVITE_REQ, &KFRelationClientModule::HandleSetRefuseFriendInviteReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_TO_GAME_ACK, &KFRelationClientModule::HandleQueryFriendToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_TO_GAME_ACK, &KFRelationClientModule::HandleQueryFriendInviteToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_DATA_TO_FRIEND_REQ, &KFRelationClientModule::HandleUpdateDataToFriendReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_TO_GAME_ACK, &KFRelationClientModule::HandleAddFriendInviteToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_TO_GAME_ACK, &KFRelationClientModule::HandleAddFriendToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_TO_GAME_ACK, &KFRelationClientModule::HandleDelFriendToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK, &KFRelationClientModule::HandleUpdateFriendLinessToGameAck );
    }

    void KFRelationClientModule::BeforeShut()
    {
        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateStringModule( this );

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_ADD_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REPLY_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_DEL_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_REFUSE_FRIEND_INVITE_REQ );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_DATA_TO_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRelationClientModule::SendToRelation( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_route->SendToRand( playerid, __KF_STRING__( relation ), msgid, message );
    }

    bool KFRelationClientModule::SendToRelation( uint64 playerid, KFData* kfrelation, uint32 msgid, google::protobuf::Message* message )
    {
        auto kfbasic = kfrelation->FindData( __KF_STRING__( basic ) );
        return _kf_game->SendToPlayer( playerid, kfbasic, msgid, message );
    }

    void KFRelationClientModule::SendMessageToFriend( KFEntity* player, uint32 msgid, google::protobuf::Message* message )
    {
        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );

        auto kffriend = kffriendrecord->FirstData();
        while ( kffriend != nullptr )
        {
            SendToRelation( player->GetKeyID(), kffriend, msgid, message );
            kffriend = kffriendrecord->NextData();
        }
    }

    uint32 KFRelationClientModule::GetMaxFriendCount()
    {
        static auto _max_friend_count = _kf_option->FindOption( __KF_STRING__( freindmaxcount ) );
        return _max_friend_count->_uint32_value;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationClientModule::OnEnterQueryFriend( KFEntity* player )
    {
        {
            // 查询好友列表
            KFMsg::S2SQueryFriendToRelationReq req;
            req.set_playerid( player->GetKeyID() );
            SendToRelation( player->GetKeyID(), KFMsg::S2S_QUERY_FRIEND_TO_RELATION_REQ, &req );
        }

        {
            // 查询好友申请列表
            KFMsg::S2SQueryFriendInviteToRelationReq req;
            req.set_playerid( player->GetKeyID() );
            SendToRelation( player->GetKeyID(), KFMsg::S2S_QUERY_FRIEND_INVITE_TO_RELATION_REQ, &req );
        }
    }

    void KFRelationClientModule::OnLeaveUpdateFriend( KFEntity* player )
    {
        // 通知所有好友,我下线了
        MapString values;
        values[ __KF_STRING__( status ) ] = __TO_STRING__( KFMsg::OfflineState );
        values[ __KF_STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        SendUpdateToFriend( player, values );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRelationClientModule::OnRelationValueUpdate )
    {
        if ( !kfdata->HaveFlagMask( KFDataDefine::Mask_Relation_Data ) ||
                !kfdata->GetParent()->HaveFlagMask( KFDataDefine::Mask_Relation_Data ) )
        {
            return;
        }

        // 更新给好友
        MapString values;
        values[ kfdata->GetName() ] = kfdata->ToString();
        SendUpdateToFriend( player, values );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFRelationClientModule::OnRelationStringUpdate )
    {
        if ( !kfdata->HaveFlagMask( KFDataDefine::Mask_Relation_Data ) ||
                !kfdata->GetParent()->HaveFlagMask( KFDataDefine::Mask_Relation_Data ) )
        {
            return;
        }

        // 更新给好友
        MapString values;
        values[ kfdata->GetName() ] = kfdata->ToString();
        SendUpdateToFriend( player, values );
    }

    void KFRelationClientModule::SendUpdateToFriend( KFEntity* player, MapString& values )
    {
        KFMsg::S2SUpdateDataToFriendReq req;
        req.set_playerid( player->GetKeyID() );

        auto pbdata = req.mutable_pbdata();
        for ( auto& iter : values )
        {
            ( *pbdata )[ iter.first ] = iter.second;
        }

        SendMessageToFriend( player, KFMsg::S2S_UPDATE_DATA_TO_FRIEND_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateDataToFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateDataToFriendReq );

        auto playerid = __ROUTE_RECV_ID__;
        auto player = _kf_player->FindPlayer( playerid );
        if ( player != nullptr )
        {
            // 玩家存在, 就更新好友属性
            auto kfobject = player->GetData();
            auto kfbasic = kfobject->FindData( __KF_STRING__( friend ), kfmsg.playerid(), __KF_STRING__( basic ) );
            if ( kfbasic != nullptr )
            {
                auto pbdata = &kfmsg.pbdata();
                for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )
                {
                    player->UpdateData( kfbasic, iter->first, iter->second );
                }
            }
        }
        else
        {
            // 信息不同步时候, 设置玩家不在线
            // 如果game宕机, 不会发送下线消息, 然后该玩家不在上线, 状态会一直显示在线
            MapString values;
            values[ __KF_STRING__( serverid ) ] = "0";
            values[ __KF_STRING__( status ) ] = __TO_STRING__( KFMsg::OfflineState );
            values[ __KF_STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
            _kf_public->UpdatePublicData( playerid, values );

            if ( route._send_id != _invalid_int )
            {
                // 同步给好友, 避免每次都更新消息过来
                KFMsg::S2SUpdateDataToFriendReq req;
                req.set_playerid( playerid );
                auto pbdata = req.mutable_pbdata();
                for ( auto& iter : values )
                {
                    ( *pbdata )[ iter.first ] = iter.second;
                }
                _kf_game->SendToPlayer( _invalid_int, route._server_id, route._send_id, KFMsg::S2S_UPDATE_DATA_TO_FRIEND_REQ, &req );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryFriendToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryFriendToGameAck );

        auto kfobject = player->GetData();
        auto kffreindrecord = kfobject->FindData( __KF_STRING__( friend ) );

        for ( auto i = 0; i < kfmsg.pbfriend_size(); ++i )
        {
            auto pbfriend = &kfmsg.pbfriend( i );
            auto kffriend = _kf_kernel->CreateObject( kffreindrecord->GetDataSetting() );

            PBRelationToKFData( pbfriend, kffriend );
            player->AddData( kffreindrecord, kffriend );
        }

        // 通知所有好友,我上线了
        MapString values;
        values[ __KF_STRING__( status ) ] = __TO_STRING__( KFMsg::OnlineState );
        values[ __KF_STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __KF_STRING__( serverid ) ] = __TO_STRING__( KFGlobal::Instance()->_app_id->GetId() );
        SendUpdateToFriend( player, values );
    }

    void KFRelationClientModule::PBRelationToKFData( const KFMsg::PBRelation* pbrelation, KFData* kfrelation )
    {
        kfrelation->SetKeyID( pbrelation->playerid() );

        // 玩家基础属性
        auto kfbasic = kfrelation->FindData( __KF_STRING__( basic ) );
        auto pbplayerdata = &pbrelation->playerdata();
        for ( auto iter = pbplayerdata->begin(); iter != pbplayerdata->end(); ++iter )
        {
            auto kfdata = kfbasic->FindData( iter->first );
            if ( kfdata != nullptr )
            {
                kfdata->SetValue< std::string >( iter->second );
            }
        }

        // 好友关系属性
        auto pbrelationdata = &pbrelation->relationdata();
        for ( auto iter = pbrelationdata->begin(); iter != pbrelationdata->end(); ++iter )
        {
            auto kfdata = kfrelation->FindData( iter->first );
            if ( kfdata != nullptr )
            {
                kfdata->SetValue< std::string >( iter->second );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryFriendInviteToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryFriendInviteToGameAck );

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( friendinvite ) );

        for ( auto i = 0; i < kfmsg.pbinvite_size(); ++i )
        {
            auto pbinvite = &kfmsg.pbinvite( i );
            auto kfinvite = _kf_kernel->CreateObject( kfinviterecord->GetDataSetting() );

            PBRelationToKFData( pbinvite, kfinvite );
            player->AddData( kfinviterecord, kfinvite );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddFriendInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgAddFriendInviteReq );

        // 不能添加自己为好友
        if ( playerid == kfmsg.playerid() )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );

        // 已经是好友, 不能申请
        auto kffriend = kffriendrecord->FindData( kfmsg.playerid() );
        if ( kffriend != nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendAlready, kfmsg.name() );
        }

        // 判断自己好友数量
        auto maxfriendcount = GetMaxFriendCount();
        if ( kffriendrecord->Size() >= maxfriendcount )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendSelfLimit, maxfriendcount );
        }

        // 检查屏蔽字
        auto message = kfmsg.message();
        KFUtility::ReplaceString( message, " ", "" );
        if ( _kf_filter->CheckFilter( message ) )
        {
            return _kf_display->SendToClient( player, KFMsg::MessageFilterError );
        }

        // 发送好友集群处理
        KFMsg::S2SAddFriendInviteToRelationReq req;
        req.set_name( kfmsg.name() );
        req.set_playerid( kfmsg.playerid() );
        req.set_message( message );
        auto ok = SendToRelation( playerid, KFMsg::S2S_ADD_FRIEND_INVITE_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddFriendInviteToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SAddFriendInviteToGameAck );

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( friendinvite ) );
        auto kfinvite = _kf_kernel->CreateObject( kfinviterecord->GetDataSetting() );

        PBRelationToKFData( &kfmsg.pbinvite(), kfinvite );
        player->AddData( kfinviterecord, kfinvite );

        auto name = kfinvite->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::FriendInviteReq, name );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleReplyInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReplyFriendInviteReq );

        // playerid 为0, 代表全部操作
        if ( kfmsg.playerid() == _invalid_int )
        {
            ReplyFriendInvite( player, kfmsg.operate() );
        }
        else
        {
            ReplyFriendInvite( player, kfmsg.playerid(), kfmsg.operate() );
        }
    }

    void KFRelationClientModule::ReplyFriendInvite( KFEntity* player, uint32 operate )
    {
        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( friendinvite ) );

        std::list< uint64 > removelist;

        auto kfinvite = kfinviterecord->FirstData();
        while ( kfinvite != nullptr )
        {
            auto removeid = ReplyFriendInvite( player, kfinvite, operate );
            if ( removeid != _invalid_int  )
            {
                removelist.push_back( removeid );
            }

            kfinvite = kfinviterecord->NextData();
        }

        for ( auto removeid : removelist )
        {
            player->RemoveData( __KF_STRING__( friendinvite ), removeid );
        }
    }

    void KFRelationClientModule::ReplyFriendInvite( KFEntity* player, uint64 playerid, uint32 operate )
    {
        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( friendinvite ) );

        auto kfinvite = kfinviterecord->FindData( playerid );
        if ( kfinvite == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendInviteNotExist );
        }

        auto inviteid = ReplyFriendInvite( player, kfinvite, operate );
        if ( inviteid != _invalid_int )
        {
            player->RemoveData( kfinviterecord, inviteid );
        }
    }

    uint64 KFRelationClientModule::ReplyFriendInvite( KFEntity* player, KFData* kfinvite, uint32 operate )
    {
        auto removeid = _invalid_int;
        switch ( operate )
        {
        case KFMsg::Consent:	// 同意申请
        {
            ProcessAddFriend( player, kfinvite );
        }
        break;
        case KFMsg::Refuse:		// 拒绝, 通知对方
        {
            auto kfobject = player->GetData();
            auto selfname = kfobject->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
            auto targetserverid = kfinvite->GetValue( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
            _kf_display->SendToPlayer( targetserverid, kfinvite->GetKeyID(), KFMsg::FriendRefuseYourInvite, selfname );
        }
        //break; 注释break, 执行下次发送删除消息操作
        case KFMsg::Delete:	// 删除, 不通知对方
        {
            removeid = kfinvite->GetKeyID();

            // 发送到好友集群
            KFMsg::S2SDelFriendInviteToRelationReq req;
            req.set_selfplayerid( player->GetKeyID() );
            req.set_targetplayerid( kfinvite->GetKeyID() );
            SendToRelation( player->GetKeyID(), KFMsg::S2S_DEL_FRIEND_INVITE_TO_RELATION_REQ, &req );
        }
        break;
        default:
            break;
        }

        return removeid;
    }

    void KFRelationClientModule::ProcessAddFriend( KFEntity* player, KFData* kfinvite )
    {
        // 判断自己的好友数量
        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );

        auto maxfreidncount = GetMaxFriendCount();
        if ( kffriendrecord->Size() >= maxfreidncount )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendSelfLimit, maxfreidncount );
        }

        // 发送消息到好友集群
        KFMsg::S2SAddFriendToRelationReq req;
        req.set_playerid( kfinvite->GetKeyID() );
        req.set_name( kfinvite->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) ) );
        auto ok = SendToRelation( player->GetKeyID(), KFMsg::S2S_ADD_FRIEND_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddFriendToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SAddFriendToGameAck );

        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );

        // 添加好友
        auto kffriend = _kf_kernel->CreateObject( kffriendrecord->GetDataSetting() );
        PBRelationToKFData( &kfmsg.pbfriend(), kffriend );
        player->AddData( kffriendrecord, kffriend );

        // 删除邀请
        player->RemoveData( __KF_STRING__( friendinvite ), kffriend->GetKeyID() );

        auto friendname = kffriend->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::FriendAddOk, friendname );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelFriendReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDelFriendReq );

        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );
        auto kffriend = kffriendrecord->FindData( kfmsg.playerid() );
        if ( kffriend == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendNotExist );
        }

        // 发送好友集群处理
        KFMsg::S2SDelFriendToRelationReq req;
        req.set_playerid( kfmsg.playerid() );
        auto ok = SendToRelation( playerid, KFMsg::S2S_DEL_FRIEND_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelFriendToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SDelFriendToGameAck );

        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );
        auto kffriend = kffriendrecord->FindData( kfmsg.friendid() );
        if ( kffriend == nullptr )
        {
            return;
        }

        auto name = kffriend->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::FriendDelOk, kfmsg.friendid(), name );

        // 直接删除好友
        player->RemoveData( kffriendrecord, kfmsg.friendid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleSetRefuseFriendInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetRefuseFriendInviteReq );

        player->UpdateData( __KF_STRING__( refusefriend ), KFOperateEnum::Set, kfmsg.refuse() );
    }

    void KFRelationClientModule::AddFriendLiness( KFEntity* player, uint64 friendid, uint32 type, uint32 value )
    {
        auto kfobject = player->GetData();
        auto kffriend = kfobject->FindData( __KF_STRING__( friend ), friendid );
        if ( kffriend == nullptr )
        {
            return;
        }

        // 最大好友度限制
        static auto _max_friend_liness_option = _kf_option->FindOption( __KF_STRING__( freindlinessmax ) );
        auto friendliness = kffriend->GetValue( __KF_STRING__( friendliness ) );
        if ( friendliness >= _max_friend_liness_option->_uint32_value )
        {
            return;
        }

        KFMsg::S2SUpdateFriendLinessToRelationReq req;
        req.set_type( type );
        req.set_friendliness( value );
        req.set_targetplayerid( friendid );
        req.set_selfplayerid( player->GetKeyID() );
        SendToRelation( player->GetKeyID(), KFMsg::S2S_UPDATE_FRIENDLINESS_TO_RELATION_REQ, &req );
    }

    void KFRelationClientModule::AddFriendLinessOnce( KFEntity* player, uint64 friendid, uint32 type, uint32 value )
    {
        // 好友度是双向的, 数据只保存了一条, 所以更新的时候使用id比较小的一个来做逻辑
        if ( player->GetKeyID() > friendid )
        {
            return;
        }

        AddFriendLiness( player, friendid, type, value );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateFriendLinessToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SUpdateFriendLinessToGameAck );

        auto kfobject = player->GetData();
        auto kffriend = kfobject->FindData( __KF_STRING__( friend ), kfmsg.targetplayerid() );
        if ( kffriend == nullptr )
        {
            return;
        }

        // 更新好友度
        player->UpdateData( kffriend, __KF_STRING__( friendliness ), KFOperateEnum::Add, kfmsg.friendliness() );

        // 显示提示
        auto friendname = kffriend->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::FriendLinessAdd, friendname, kfmsg.friendliness() );
    }

    //    void KFRelationClientModule::AddRecentPlayer( KFEntity* player, uint64 roomid, const KFMsg::PBBattleScore* pbscore )
    //    {
    //#define __ADD_PB_STRING__( name, value ) \
    //    {\
    //        auto pbstring = pbstrings->add_pbstring();\
    //        pbstring->set_name( name );\
    //        pbstring->set_value( value );\
    //    }
    //
    //        // 单人模式
    //        if ( pbscore->members_size() == 0 )
    //        {
    //            return;
    //        }
    //        /////////////////////////////////////////////////////////////////////////////////////////////
    //        /////////////////////////////////////////////////////////////////////////////////////////////
    //        // 格式化最近玩家战绩数据
    //        KFMsg::S2SAddRecentPlayerDataReq req;
    //        req.set_roomid( roomid );
    //        req.set_playerid( pbscore->playerid() );
    //        req.mutable_members()->CopyFrom( pbscore->members() );
    //
    //        auto pbstrings = req.mutable_pbdata();
    //        __ADD_PB_STRING__( __KF_STRING__( id ), __TO_STRING__( pbscore->playerid() ) );
    //        __ADD_PB_STRING__( __KF_STRING__( score ), __TO_STRING__( pbscore->score() ) );
    //        __ADD_PB_STRING__( __KF_STRING__( rank ), __TO_STRING__( pbscore->ranking() ) );
    //        __ADD_PB_STRING__( __KF_STRING__( totalcount ), __TO_STRING__( pbscore->playercount() ) );
    //        __ADD_PB_STRING__( __KF_STRING__( time ), __TO_STRING__( KFGlobal::Instance()->_real_time ) );
    //
    //        // 其他战斗数据属性
    //        auto kfobject = player->GetData();
    //        auto kfrecentrecord = kfobject->FindData( __KF_STRING__( recentplayer ) );
    //        auto kfclasssetting = kfrecentrecord->GetClassSetting();
    //        for ( auto i = 0; i < pbscore->pbdata_size(); ++i )
    //        {
    //            auto pbdata = &pbscore->pbdata( i );
    //            if ( kfclasssetting->IsChildData( pbdata->name() ) )
    //            {
    //                __ADD_PB_STRING__( pbdata->name(), __TO_STRING__( pbdata->value() ) );
    //            }
    //        }
    //
    //        // 添加到关系逻辑服务器
    //        SendMessageToRelation( KFMsg::S2S_ADD_RECENT_PLAYER_DATA_REQ, &req );
    //    }
    //
    //    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRecentListReq )
    //    {
    //        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryRecentListReq );
    //
    //        // 先删除最近玩家列表
    //        player->RemoveData( __KF_STRING__( recentplayer ) );
    //
    //        KFMsg::S2SQueryRecentListReq req;
    //        req.set_playerid( playerid );
    //        SendMessageToRelation( KFMsg::S2S_QUERY_RECENT_LIST_REQ, &req );
    //    }
    //
    //    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRecentListAck )
    //    {
    //        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryRecentListAck );
    //
    //        auto kfobject = player->GetData();
    //        auto kfrecentplayer = kfobject->FindData( __KF_STRING__( recentplayer ) );
    //
    //        for ( auto i = 0; i < kfmsg.pbrelation_size(); ++i )
    //        {
    //            auto pbrelation = &kfmsg.pbrelation( i );
    //
    //            auto kfrelation = _kf_kernel->CreateObject( kfrecentplayer->GetDataSetting() );
    //            PBRelationToKFData( pbrelation, kfrelation );
    //            player->AddData( kfrecentplayer, kfrelation );
    //        }
    //    }
}