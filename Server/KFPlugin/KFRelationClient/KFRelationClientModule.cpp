#include "KFRelationClientModule.h"

namespace KFrame
{
    KFRelationClientModule::KFRelationClientModule()
    {
        _kf_component = nullptr;
    }

    KFRelationClientModule::~KFRelationClientModule()
    {
    }

    void KFRelationClientModule::InitModule()
    {
    }

    void KFRelationClientModule::BeforeRun()
    {
        // 注册属性回调
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterUpdateDataModule( this, &KFRelationClientModule::OnRelationValueUpdate );
        _kf_component->RegisterUpdateStringModule( this, &KFRelationClientModule::OnRelationStringUpdate );

        _kf_player->RegisterEnterFunction( this, &KFRelationClientModule::OnEnterQueryFriend );
        _kf_player->RegisterLeaveFunction( this, &KFRelationClientModule::OnLeaveUpdateFriend );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_ACK, &KFRelationClientModule::HandleQueryFriendAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_ACK, &KFRelationClientModule::HandleQueryInviteAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_ADD_FRIEND_INVITE_REQ, &KFRelationClientModule::HandleAddFriendInviteReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_ACK, &KFRelationClientModule::HandleAddFriendInviteAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_DEL_FRIEND_REQ, &KFRelationClientModule::HandleDelFriendReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_ACK, &KFRelationClientModule::HandleDelFriendAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_ACK, &KFRelationClientModule::HandleAddFriendAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIEND_REQ, &KFRelationClientModule::HandleUpdateFriendReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REPLY_FRIEND_INVITE_REQ, &KFRelationClientModule::HandleReplyInviteReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_REFUSE_FRIEND_INVITE_REQ, &KFRelationClientModule::HandleSetRefuseFriendInviteReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_ACK, &KFRelationClientModule::HandleUpdateFriendLinessAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RECENT_LIST_ACK, &KFRelationClientModule::HandleQueryRecentListAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_RECENT_LIST_REQ, &KFRelationClientModule::HandleQueryRecentListReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PLAYER_TOAST_REQ, &KFRelationClientModule::HandlePlayerToastReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_TOAST_ACK, &KFRelationClientModule::HandlePlayerToastAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_TOAST_COUNT_REQ, &KFRelationClientModule::HandleQueryToastCountReq );
    }

    void KFRelationClientModule::BeforeShut()
    {
        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateStringModule( this );

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_INVITE_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_ADD_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_INVITE_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_DEL_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEL_FRIEND_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_FRIEND_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIEND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REPLY_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_REFUSE_FRIEND_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_RECENT_LIST_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_RECENT_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_PLAYER_TOAST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_TOAST_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_TOAST_COUNT_REQ );
    }

    void KFRelationClientModule::OnceRun()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRelationClientModule::SendMessageToRelation( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendToShard( __KF_STRING__( relation ), msgid, message );
    }

    bool KFRelationClientModule::SendMessageToRelation( KFData* kfrelation, uint32 msgid, google::protobuf::Message* message )
    {
        auto serverid = kfrelation->GetValue< uint32 >( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
        if ( serverid == _invalid_int )
        {
            return false;
        }

        auto playerid = kfrelation->GetKeyID();
        return _kf_route->SendToRoute( serverid, playerid, msgid, message );
    }

    void KFRelationClientModule::OnEnterQueryFriend( KFEntity* player )
    {
        {
            KFMsg::S2SQueryFriendReq req;
            req.set_playerid( player->GetKeyID() );
            req.set_serverid( KFGlobal::Instance()->_app_id );
            SendMessageToRelation( KFMsg::S2S_QUERY_FRIEND_REQ, &req );
        }

        {
            KFMsg::S2SQueryFriendInviteReq req;
            req.set_playerid( player->GetKeyID() );
            req.set_serverid( KFGlobal::Instance()->_app_id );
            SendMessageToRelation( KFMsg::S2S_QUERY_FRIEND_INVITE_REQ, &req );
        }
    }

    void KFRelationClientModule::OnLeaveUpdateFriend( KFEntity* player )
    {
        // 通知所有好友,我下线了
        MapString values;
        values[ __KF_STRING__( groupid ) ] = "0";
        values[ __KF_STRING__( status ) ] = __TO_STRING__( KFMsg::OfflineStatus );
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
        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );
        if ( kffriendrecord == nullptr )
        {
            return;
        }

        KFMsg::S2SUpdateFriendReq req;
        req.set_friendid( player->GetKeyID() );
        req.set_serverid( KFGlobal::Instance()->_app_id );

        for ( auto iter : values )
        {
            auto pbdata = req.add_pbdata();
            pbdata->set_name( iter.first );
            pbdata->set_value( iter.second );
        }

        auto kffriend = kffriendrecord->FirstData();
        while ( kffriend != nullptr )
        {
            SendMessageToRelation( kffriend, KFMsg::S2S_UPDATE_FRIEND_REQ, &req );
            kffriend = kffriendrecord->NextData();
        }
    }

    void KFRelationClientModule::PBRelationToKFData( const KFMsg::PBRelation* pbrelation, KFData* kfrelation )
    {
        kfrelation->SetKeyID( pbrelation->playerid() );

        auto kfbasic = kfrelation->FindData( __KF_STRING__( basic ) );

        auto pbplayerdata = &pbrelation->playerdata();
        for ( auto i = 0; i < pbplayerdata->pbstring_size(); ++i )
        {
            auto pbdata = &pbplayerdata->pbstring( i );

            auto kfdata = kfbasic->FindData( pbdata->name() );
            if ( kfdata != nullptr )
            {
                kfdata->SetValue< std::string >( pbdata->value() );
            }
        }

        auto pbrelationdata = &pbrelation->relationdata();
        for ( auto i = 0; i < pbrelationdata->pbstring_size(); ++i )
        {
            auto pbdata = &pbrelationdata->pbstring( i );

            auto kfdata = kfrelation->FindData( pbdata->name() );
            if ( kfdata != nullptr )
            {
                kfdata->SetValue< std::string >( pbdata->value() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateFriendReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateFriendReq );

        auto playerid = __KF_DATA_ID__( kfid );
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            // 信息不同步时候, 设置玩家不在线
            // 如果game宕机, 不会发送下线消息, 然后该玩家不在上线, 状态会一直显示在线
            MapString values;
            values[ __KF_STRING__( serverid ) ] = "0";
            values[ __KF_STRING__( status ) ] = __TO_STRING__( KFMsg::OfflineStatus );
            values[ __KF_STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
            _kf_public->UpdatePublicData( playerid, values );

            // 同步给好友, 避免每次都更新消息过来
            if ( kfmsg.serverid() != _invalid_int )
            {
                KFMsg::S2SUpdateFriendReq req;
                req.set_friendid( playerid );
                req.set_serverid( _invalid_int );
                for ( auto iter : values )
                {
                    auto pbdata = req.add_pbdata();
                    pbdata->set_name( iter.first );
                    pbdata->set_value( iter.second );
                }
                _kf_route->SendToRoute( kfmsg.serverid(), kfmsg.friendid(), KFMsg::S2S_UPDATE_FRIEND_REQ, &req );
            }
        }
        else
        {
            auto kfobject = player->GetData();
            auto kfbasic = kfobject->FindData( __KF_STRING__( friend ), kfmsg.friendid(), __KF_STRING__( basic ) );
            if ( kfbasic == nullptr )
            {
                return;
            }

            for ( auto i = 0; i < kfmsg.pbdata_size(); ++i )
            {
                auto pbdata = &kfmsg.pbdata( i );
                player->UpdateData( kfbasic, pbdata->name(), pbdata->value() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryFriendAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryFriendAck );

        auto kfobject = player->GetData();
        auto kffreindrecord = kfobject->FindData( __KF_STRING__( friend ) );

        for ( auto i = 0; i < kfmsg.pbfriend_size(); ++i )
        {
            auto pbfriend = &kfmsg.pbfriend( i );
            auto kffriend = _kf_kernel->CreateObject( kffreindrecord->GetDataSetting() );
            if ( kffriend == nullptr )
            {
                continue;
            }

            PBRelationToKFData( pbfriend, kffriend );
            player->AddData( kffreindrecord, kffriend );
        }

        // 通知所有好友,我上线了
        MapString values;
        values[ __KF_STRING__( status ) ] = __TO_STRING__( KFMsg::OnlineStatus );
        values[ __KF_STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __KF_STRING__( serverid ) ] = __TO_STRING__( KFGlobal::Instance()->_app_id );

        SendUpdateToFriend( player, values );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryInviteAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryFriendInviteAck );

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( friendinvite ) );

        for ( auto i = 0; i < kfmsg.pbfriend_size(); ++i )
        {
            auto pbfriend = &kfmsg.pbfriend( i );
            auto kfinvite = _kf_kernel->CreateObject( kfinviterecord->GetDataSetting() );
            if ( kfinvite == nullptr )
            {
                continue;
            }

            PBRelationToKFData( pbfriend, kfinvite );
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
        auto kffriend = kffriendrecord->FindData( kfmsg.playerid() );
        if ( kffriend != nullptr )
        {
            // 已经是好友, 不能申请
            return _kf_display->SendToClient( player, KFMsg::FriendAlready, kfmsg.name() );
        }

        // 判断自己好友数量
        static auto _max_friend_count = _kf_option->FindOption( __KF_STRING__( freindmaxcount ) );
        if ( kffriendrecord->Size() >= _max_friend_count->_uint32_value )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendSelfLimit, _max_friend_count->_uint32_value );
        }

        auto message = kfmsg.message();
        KFUtility::ReplaceString( message, " ", "" );

        // 发送好友集群处理
        KFMsg::S2SAddFriendInviteReq req;
        req.set_selfplayerid( playerid );
        req.set_targetplayerid( kfmsg.playerid() );
        req.set_targetname( kfmsg.name() );
        req.set_message( message );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        auto ok = SendMessageToRelation( KFMsg::S2S_ADD_FRIEND_INVITE_REQ, &req );

        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::FriendServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddFriendInviteAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SAddFriendInviteAck );

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( friendinvite ) );
        auto kfinvite = _kf_kernel->CreateObject( kfinviterecord->GetDataSetting() );
        if ( kfinvite == nullptr )
        {
            return;
        }

        PBRelationToKFData( &kfmsg.pbfriend(), kfinvite );
        player->AddData( kfinviterecord, kfinvite );

        auto name = kfinvite->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::FriendInviteReq, name );
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
        KFMsg::S2SDelFriendReq req;
        req.set_selfplayerid( playerid );
        req.set_targetplayerid( kfmsg.playerid() );
        auto ok = SendMessageToRelation( KFMsg::S2S_DEL_FRIEND_REQ, &req );
        if ( ok )
        {
            auto name = kffriend->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
            _kf_display->SendToClient( player, KFMsg::FriendDelOK, name, kfmsg.playerid() );

            player->RemoveData( kffriendrecord, kfmsg.playerid() );
        }
        else
        {
            _kf_display->SendToClient( player, KFMsg::FriendServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelFriendAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SDelFriendAck );

        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );
        auto kffriend = kffriendrecord->FindData( kfmsg.targetplayerid() );
        if ( kffriend == nullptr )
        {
            return;
        }

        auto name = kffriend->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::FriendDelOK, kfmsg.targetplayerid(), name );

        // 直接删除好友
        player->RemoveData( __KF_STRING__( friend ), kfmsg.targetplayerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleReplyInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReplyFriendInviteReq );

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

        std::vector<uint32> removelist;

        auto kfinvite = kfinviterecord->FirstData();
        while ( kfinvite != nullptr )
        {
            auto removeid = ReplyFriendInvite( player, kfinvite, operate );

            if ( _invalid_int != removeid )
            {
                removelist.push_back( removeid );
            }

            kfinvite = kfinviterecord->NextData();
        }

        for ( auto iter : removelist )
        {
            player->RemoveData( __KF_STRING__( friendinvite ), iter );
        }
    }

    void KFRelationClientModule::ReplyFriendInvite( KFEntity* player, uint32 playerid, uint32 operate )
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

    uint32 KFRelationClientModule::ReplyFriendInvite( KFEntity* player, KFData* kfinvite, uint32 operate )
    {
        auto removeid = _invalid_int;

        switch ( operate )
        {
        case KFMsg::InviteEnum::Consent:
        {
            AddFriend( player, kfinvite );
        }
        break;
        case KFMsg::InviteEnum::Refuse:
        {
            auto kfobject = player->GetData();
            auto selfname = kfobject->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
            auto targetserverid = kfinvite->GetValue< uint32 >( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
            _kf_display->SendToPlayer( targetserverid, kfinvite->GetKeyID(), KFMsg::FriendRefuseYourInvite, selfname );
        }//break; 注释break, 执行下次发送删除消息操作
        case  KFMsg::InviteEnum::Delete:
        {
            removeid = kfinvite->GetKeyID();

            // 发送到好友集群
            KFMsg::S2SDelFriendInviteReq req;
            req.set_selfplayerid( player->GetKeyID() );
            req.set_targetplayerid( kfinvite->GetKeyID() );
            SendMessageToRelation( KFMsg::S2S_DEL_FRIEND_INVITE_REQ, &req );
        }
        break;
        default:
            break;
        }

        return removeid;
    }

    void KFRelationClientModule::AddFriend( KFEntity* player, KFData* kfinvite )
    {
        static auto _max_friend_count = _kf_option->FindOption( __KF_STRING__( freindmaxcount ) );

        // 判断自己的好友数量
        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );
        auto friendcount = kffriendrecord->Size();
        if ( friendcount >= _max_friend_count->_uint32_value )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendSelfLimit, _max_friend_count->_uint32_value );
        }

        // 发送消息到好友集群
        KFMsg::S2SAddFriendReq req;
        req.set_selfplayerid( player->GetKeyID() );
        req.set_targetplayerid( kfinvite->GetKeyID() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        req.set_targetname( kfinvite->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) ) );
        auto ok = SendMessageToRelation( KFMsg::S2S_ADD_FRIEND_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::FriendServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddFriendAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SAddFriendAck );

        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );

        // 添加好友
        auto kffriend = _kf_kernel->CreateObject( kffriendrecord->GetDataSetting() );
        PBRelationToKFData( &kfmsg.pbfriend(), kffriend );
        player->AddData( kffriendrecord, kffriend );

        // 删除邀请
        player->RemoveData( __KF_STRING__( friendinvite ), kffriend->GetKeyID() );

        auto friendname = kffriend->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::FriendAddOK, friendname );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleSetRefuseFriendInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetRefuseFriendInviteReq );

        player->UpdateData( __KF_STRING__( refuseinvite ), KFOperateEnum::Set, kfmsg.refuse() );
    }

    void KFRelationClientModule::AddFriendLiness( KFEntity* player, uint32 friendid, uint32 type, uint32 value )
    {
        auto kfobject = player->GetData();
        auto kffriend = kfobject->FindData( __KF_STRING__( friend ), friendid );
        if ( kffriend == nullptr )
        {
            return;
        }

        // 获得最大好友度
        static auto _max_friend_liness_option = _kf_option->FindOption( __KF_STRING__( freindmaxcount ) );

        KFMsg::S2SUpdateFriendLinessReq req;
        req.set_type( type );
        req.set_friendliness( value );
        req.set_targetplayerid( friendid );
        req.set_selfplayerid( player->GetKeyID() );
        SendMessageToRelation( KFMsg::S2S_UPDATE_FRIEND_LINESS_REQ, &req );
    }

    void KFRelationClientModule::AddFriendLinessOnce( KFEntity* player, uint32 friendid, uint32 type, uint32 value )
    {
        // 好友度是双向的, 数据只保存了一条, 所以更新的时候使用id比较小的一个来做逻辑
        if ( player->GetKeyID() > friendid )
        {
            return;
        }

        AddFriendLiness( player, friendid, type, value );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateFriendLinessAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SUpdateFriendLinessAck );

        auto kfobject = player->GetData();
        auto kffriend = kfobject->FindData( __KF_STRING__( friend ), kfmsg.targetplayerid() );
        if ( kffriend == nullptr )
        {
            return;
        }

        // 更新好友度
        player->UpdateData( kffriend, __KF_STRING__( friendliness ), KFOperateEnum::Add, kfmsg.friendliness() );

        auto friendname = kffriend->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::FriendLinessAdd, friendname, kfmsg.friendliness() );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandlePlayerToastReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPlayerToastReq );

        auto kfobject = player->GetData();
        auto toastrecord = kfobject->FindData( __KF_STRING__( toast ) );

        //判断每日敬酒次数
        static auto _daily_send_limit = _kf_option->FindOption( __KF_STRING__( toastsendlimitcount ) );
        if ( toastrecord->Size() >= _daily_send_limit->_uint32_value )
        {
            return _kf_display->SendToClient( player, KFMsg::ToastSendCountOver );
        }

        //判断是否重复敬酒
        auto toastplayer = toastrecord->FindData( kfmsg.playerid() );
        if ( toastplayer != nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ToastCanNotRepeat );
        }

        //发送到public判断被敬酒次数
        KFMsg::S2SPlayerToastReq req;
        req.set_selfplayerid( playerid );
        req.set_targetplayerid( kfmsg.playerid() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToRelation( KFMsg::S2S_PLAYER_TOAST_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandlePlayerToastAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SPlayerToastAck );

        _kf_display->SendToClient( player, kfmsg.result() );
        if ( kfmsg.result() != KFMsg::ToastOK )
        {
            return;
        }

        // 添加敬酒列表
        player->UpdateData( __KF_STRING__( toast ), kfmsg.targetplayerid(), __KF_STRING__( id ), KFOperateEnum::Set, kfmsg.targetplayerid() );

        // 添加好友度
        static auto _toast_friend_liness = _kf_option->FindOption( __KF_STRING__( toastfriendliness ) );
        AddFriendLiness( player, kfmsg.targetplayerid(), KFMsg::Toast, _toast_friend_liness->_uint32_value );

        // 发送敬酒邮件
        static auto _toast_mail_id = _kf_option->FindOption( __KF_STRING__( toastmailid ) );
        _kf_mail->SendMail( player, kfmsg.targetserverid(), kfmsg.targetplayerid(), _toast_mail_id->_uint32_value, nullptr );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryToastCountReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryToastCountReq );

        KFMsg::S2SQueryToastCountReq req;
        req.set_selfplayerid( playerid );
        req.set_targetplayerid( kfmsg.playerid() );
        SendMessageToRelation( KFMsg::S2S_QUERY_TOAST_COUNT_REQ, &req );
    }

    void KFRelationClientModule::BalanceBattleRelation( KFEntity* player, uint64 roomid, const KFMsg::PBBattleScore* pbscore )
    {
        // 结算好友度
        BalanceFriendLiness( player, pbscore );

        // 添加最近游戏的人
        AddRecentPlayer( player, roomid, pbscore );
    }

    void KFRelationClientModule::BalanceFriendLiness( KFEntity* player, const KFMsg::PBBattleScore* pbscore )
    {
        static auto _team_friend_liness = _kf_option->FindOption( __KF_STRING__( teamfriendliness ) );
        static auto _win_friend_liness = _kf_option->FindOption( __KF_STRING__( winfriendliness ) );

        for ( auto i = 0; i < pbscore->members_size(); ++i )
        {
            auto playerid = pbscore->members( i );
            _kf_relation->AddFriendLinessOnce( player, playerid, KFMsg::Team, _team_friend_liness->_uint32_value );
            if ( pbscore->ranking() == __TOP_ONE__ )
            {
                _kf_relation->AddFriendLinessOnce( player, playerid, KFMsg::Win, _win_friend_liness->_uint32_value );
            }
        }
    }

    void KFRelationClientModule::AddRecentPlayer( KFEntity* player, uint64 roomid, const KFMsg::PBBattleScore* pbscore )
    {
#define __ADD_PB_STRING__( name, value ) \
    {\
        auto pbstring = pbstrings->add_pbstring();\
        pbstring->set_name( name );\
        pbstring->set_value( value );\
    }

        // 单人模式
        if ( pbscore->members_size() == 0 )
        {
            return;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 格式化最近玩家战绩数据
        KFMsg::S2SAddRecentPlayerDataReq req;
        req.set_roomid( roomid );
        req.set_playerid( pbscore->playerid() );
        req.mutable_members()->CopyFrom( pbscore->members() );

        auto pbstrings = req.mutable_pbdata();
        __ADD_PB_STRING__( __KF_STRING__( id ), __TO_STRING__( pbscore->playerid() ) );
        __ADD_PB_STRING__( __KF_STRING__( score ), __TO_STRING__( pbscore->score() ) );
        __ADD_PB_STRING__( __KF_STRING__( rank ), __TO_STRING__( pbscore->ranking() ) );
        __ADD_PB_STRING__( __KF_STRING__( totalcount ), __TO_STRING__( pbscore->playercount() ) );
        __ADD_PB_STRING__( __KF_STRING__( time ), __TO_STRING__( KFGlobal::Instance()->_real_time ) );

        // 其他战斗数据属性
        auto kfobject = player->GetData();
        auto kfrecentrecord = kfobject->FindData( __KF_STRING__( recentplayer ) );
        auto kfclasssetting = kfrecentrecord->GetClassSetting();
        for ( auto i = 0; i < pbscore->pbdata_size(); ++i )
        {
            auto pbdata = &pbscore->pbdata( i );
            if ( kfclasssetting->IsChildData( pbdata->name() ) )
            {
                __ADD_PB_STRING__( pbdata->name(), __TO_STRING__( pbdata->value() ) );
            }
        }

        // 添加到关系逻辑服务器
        SendMessageToRelation( KFMsg::S2S_ADD_RECENT_PLAYER_DATA_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRecentListReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryRecentListReq );

        // 先删除最近玩家列表
        player->RemoveData( __KF_STRING__( recentplayer ) );

        KFMsg::S2SQueryRecentListReq req;
        req.set_playerid( playerid );
        SendMessageToRelation( KFMsg::S2S_QUERY_RECENT_LIST_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRecentListAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryRecentListAck );

        auto kfobject = player->GetData();
        auto kfrecentplayer = kfobject->FindData( __KF_STRING__( recentplayer ) );

        for ( auto i = 0; i < kfmsg.pbrelation_size(); ++i )
        {
            auto pbrelation = &kfmsg.pbrelation( i );

            auto kfrelation = _kf_kernel->CreateObject( kfrecentplayer->GetDataSetting() );
            PBRelationToKFData( pbrelation, kfrelation );
            player->AddData( kfrecentplayer, kfrelation );
        }
    }
}