#include "KFRelationClientModule.h"
#include "KFRelationConfig.h"

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
        __KF_ADD_CONFIG__( _kf_relation_config, true );
    }

    void KFRelationClientModule::BeforeRun()
    {
        // 注册属性回调
        _kf_component = _kf_kernel->FindComponent( KFField::_player );
        _kf_component->RegisterUpdateDataModule( this, &KFRelationClientModule::OnRelationValueUpdate );
        _kf_component->RegisterUpdateStringModule( this, &KFRelationClientModule::OnRelationStringUpdate );
        _kf_component->RegisterAddDataFunction( KFField::_toast, this, &KFRelationClientModule::OnAddToastCallBack );

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
        __REGISTER_MESSAGE__( KFMsg::S2S_MODIFY_RECENT_LIST_REQ, &KFRelationClientModule::HandleModifyRecentListReq );
    }

    void KFRelationClientModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();

        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateStringModule( this );
        _kf_component->UnRegisterAddDataFunction( KFField::_toast );

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
        __UNREGISTER_MESSAGE__( KFMsg::S2S_MODIFY_RECENT_LIST_REQ );
    }

    void KFRelationClientModule::OnceRun()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRelationClientModule::SendMessageToRelation( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToShard( KFField::_relation, msgid, message );
    }

    bool KFRelationClientModule::SendMessageToRelation( KFData* kfrelation, uint32 msgid, google::protobuf::Message* message )
    {
        auto serverid = kfrelation->GetValue< uint32 >( KFField::_basic, KFField::_server_id );

        if ( serverid == _invalid_int )
        {
            return false;
        }

        auto playerid = kfrelation->GetKeyID();
        return _kf_route->SendMessageToRoute( serverid, playerid, msgid, message );
    }

    bool KFRelationClientModule::SendMessageToRelation( uint32 serverid, uint32 playerid, uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_route->SendMessageToRoute( serverid, playerid, msgid, message );
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
        values[ KFField::_group_id ] = "0";
        values[ KFField::_status ] = __KF_STRING__( KFMsg::StatusEnum::OfflineStatus );
        values[ KFField::_status_time ] = __KF_STRING__( KFGlobal::Instance()->_real_time );
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

    __KF_ADD_DATA_FUNCTION__( KFRelationClientModule::OnAddToastCallBack )
    {
        // 敬酒增加好友度
        auto targetplayerid = kfdata->GetValue< uint32 >( KFField::_id );
        UpdateFriendLiness( player, targetplayerid, KFOperateEnum::Add, _kf_relation_config->_jing_jiu_friend_liness, KFFriendEnum::Laud );
    }

    void KFRelationClientModule::SendUpdateToFriend( KFEntity* player, MapString& values )
    {
        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( KFField::_friend );

        if ( kffriendrecord == nullptr )
        {
            return;
        }

        KFMsg::S2SUpdateFriendReq req;
        req.set_friendid( player->GetKeyID() );

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

    void KFRelationClientModule::PBFriendToKFData( const KFMsg::PBFriend* pbfriend, KFData* kffriend )
    {
        kffriend->SetKeyID( pbfriend->playerid() );

        auto kfbasic = kffriend->FindData( KFField::_basic );

        for ( auto i = 0; i < pbfriend->playerdata_size(); ++i )
        {
            auto pbdata = &pbfriend->playerdata( i );

            auto kfdata = kfbasic->FindData( pbdata->name() );

            if ( kfdata != nullptr )
            {
                kfdata->SetValue< std::string >( pbdata->value() );
            }
        }

        for ( auto i = 0; i < pbfriend->frienddata_size(); ++i )
        {
            auto pbdata = &pbfriend->frienddata( i );

            auto kfdata = kffriend->FindData( pbdata->name() );

            if ( kfdata != nullptr )
            {
                kfdata->SetValue< std::string >( pbdata->value() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateFriendReq )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2SUpdateFriendReq );

        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( KFField::_friend, kfmsg.friendid(), KFField::_basic );

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

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryFriendAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryFriendAck );

        auto kfobject = player->GetData();
        auto kffreindrecord = kfobject->FindData( KFField::_friend );

        for ( auto i = 0; i < kfmsg.pbfriend_size(); ++i )
        {
            auto pbfriend = &kfmsg.pbfriend( i );

            auto kffriend = _kf_kernel->CreateObject( kffreindrecord->GetDataSetting() );

            if ( kffriend == nullptr )
            {
                continue;
            }

            PBFriendToKFData( pbfriend, kffriend );
            player->AddData( kffreindrecord, kffriend );
        }

        // 通知所有好友,我上线了
        MapString values;
        values[ KFField::_status ] = __KF_STRING__( KFMsg::StatusEnum::OnlineStatus );
        values[ KFField::_status_time ] = __KF_STRING__( KFGlobal::Instance()->_real_time );
        values[ KFField::_server_id ] = __KF_STRING__( KFGlobal::Instance()->_app_id );

        SendUpdateToFriend( player, values );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryInviteAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryFriendInviteAck );

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( KFField::_friend_invite );

        for ( auto i = 0; i < kfmsg.pbfriend_size(); ++i )
        {
            auto pbfriend = &kfmsg.pbfriend( i );

            auto kfinvite = _kf_kernel->CreateObject( kfinviterecord->GetDataSetting() );

            if ( kfinvite == nullptr )
            {
                continue;
            }

            PBFriendToKFData( pbfriend, kfinvite );
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
        auto kffriendrecord = kfobject->FindData( KFField::_friend );
        auto kffriend = kffriendrecord->FindData( kfmsg.playerid() );

        if ( kffriend != nullptr )
        {
            // 已经是好友, 不能申请
            return _kf_display->SendToClient( player, KFMsg::FriendAlready, kfmsg.name() );
        }

        // 判断自己好友数量
        if ( kffriendrecord->Size() >= _kf_relation_config->_max_friend_count )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendSelfLimit, _kf_relation_config->_max_friend_count );
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
        auto kfinviterecord = kfobject->FindData( KFField::_friend_invite );

        auto kfinvite = _kf_kernel->CreateObject( kfinviterecord->GetDataSetting() );

        if ( kfinvite == nullptr )
        {
            return;
        }

        PBFriendToKFData( &kfmsg.pbfriend(), kfinvite );
        player->AddData( kfinviterecord, kfinvite );

        auto name = kfinvite->GetValue< std::string >( KFField::_basic, KFField::_name );
        _kf_display->SendToClient( player, KFMsg::FriendInviteReq, name );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelFriendReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDelFriendReq );

        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( KFField::_friend );
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
            auto name = kffriend->GetValue< std::string >( KFField::_basic, KFField::_name );
            _kf_display->SendToClient( player, KFMsg::FriendDelOK, kfmsg.playerid(), name );

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
        auto kffriendrecord = kfobject->FindData( KFField::_friend );
        auto kffriend = kffriendrecord->FindData( kfmsg.targetplayerid() );

        if ( kffriend == nullptr )
        {
            return;
        }

        auto name = kffriend->GetValue< std::string >( KFField::_basic, KFField::_name );
        _kf_display->SendToClient( player, KFMsg::FriendDelOK, kfmsg.targetplayerid(), name );

        // 直接删除好友
        player->RemoveData( KFField::_friend, kfmsg.targetplayerid() );
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
        auto kfinviterecord = kfobject->FindData( KFField::_friend_invite );

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
            player->RemoveData( KFField::_friend_invite, iter );
        }
    }

    void KFRelationClientModule::ReplyFriendInvite( KFEntity* player, uint32 playerid, uint32 operate )
    {
        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( KFField::_friend_invite );

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
            auto selfname = kfobject->GetValue< std::string >( KFField::_basic, KFField::_name );
            auto targetserverid = kfinvite->GetValue< uint32 >( KFField::_basic, KFField::_server_id );
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
        // 判断自己的好友信息
        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( KFField::_friend );

        // 判断数量
        auto friendcount = kffriendrecord->Size();

        if ( friendcount >= _kf_relation_config->_max_friend_count )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendSelfLimit, _kf_relation_config->_max_friend_count );
        }

        // 发送消息到好友集群
        KFMsg::S2SAddFriendReq req;
        req.set_selfplayerid( player->GetKeyID() );
        req.set_targetplayerid( kfinvite->GetKeyID() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        req.set_targetname( kfinvite->GetValue< std::string >( KFField::_basic, KFField::_name ) );
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
        auto kffriendrecord = kfobject->FindData( KFField::_friend );

        // 添加好友
        auto kffriend = _kf_kernel->CreateObject( kffriendrecord->GetDataSetting() );
        PBFriendToKFData( &kfmsg.pbfriend(), kffriend );
        player->AddData( kffriendrecord, kffriend );

        // 删除邀请
        player->RemoveData( KFField::_friend_invite, kffriend->GetKeyID() );

        auto friendname = kffriend->GetValue< std::string >( KFField::_basic, KFField::_name );
        _kf_display->SendToClient( player, KFMsg::FriendAddOK, friendname );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleSetRefuseFriendInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetRefuseFriendInviteReq );

        player->UpdateData( KFField::_refuse_invite, KFOperateEnum::Set, kfmsg.refuse() );
    }

    void KFRelationClientModule::UpdateFriendLiness( KFEntity* player, uint32 friendid, uint32 operate, uint64 value, uint32 type )
    {
        auto kfobject = player->GetData();
        auto kffriend = kfobject->FindData( KFField::_friend, friendid );

        if ( kffriend == nullptr )
        {
            return;
        }

        // 好友度是双向的, 数据只保存了一条, 所以更新的时候使用id比较小的一个来做逻辑
        KFMsg::S2SUpdateFriendLinessReq req;
        req.set_selfplayerid( player->GetKeyID() );
        req.set_targetplayerid( friendid );
        req.set_friendliness( value );
        req.set_type( type );
        SendMessageToRelation( KFMsg::S2S_UPDATE_FRIEND_LINESS_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateFriendLinessAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SUpdateFriendLinessAck );

        auto kfobject = player->GetData();
        auto kffriend = kfobject->FindData( KFField::_friend, kfmsg.targetplayerid() );

        if ( kffriend == nullptr )
        {
            return;
        }

        // 更新好友度
        player->UpdateData( kffriend, KFField::_friend_liness, KFOperateEnum::Add, kfmsg.friendliness() );

        auto friendname = kffriend->GetValue< std::string >( KFField::_basic, KFField::_name );
        _kf_display->SendToClient( player, KFMsg::FriendLinessAdd, friendname, kfmsg.friendliness() );
    }


    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleModifyRecentListReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SModifyRecentListReq );
        auto kfobject = player->GetData();
        auto kfrecentplayer = kfobject->FindData( KFField::_recent_player );

        switch ( kfmsg.operate() )
        {
        case KFOperateEnum::Add:
        case  KFOperateEnum::Set:
        {

            auto uidsize = kfmsg.uids().playerid_size();
            auto basicsize = kfmsg.basicdatas().basicdata_size();

            if ( kfmsg.uids().playerid_size() != kfmsg.basicdatas().basicdata_size() )
            {
                return;
            }

            for ( auto i = 0; i < kfmsg.uids().playerid_size(); ++i )
            {
                auto recentid = kfmsg.uids().playerid( i );

                if ( _invalid_int == recentid )
                {
                    continue;
                }

                auto pbbasic = &kfmsg.basicdatas().basicdata( i );
                auto strdata = kfmsg.uidsinfos().uidsinfo( i );
                KFMsg::PBRecentData pbrecentdata;
                KFProto::Parse( &pbrecentdata, strdata, KFCompressEnum::Convert );

                if ( nullptr == pbbasic || strdata.empty() )
                {
                    continue;
                }

                auto kfdata = PBRecentListToKFData( recentid, &pbrecentdata, pbbasic, kfrecentplayer->GetDataSetting() );
                player->AddData( kfrecentplayer, kfdata );
            }
        }
        break;

        case KFOperateEnum::Dec:
        {
            for ( auto i = 0; i < kfmsg.uids().playerid_size(); ++i )
            {
                player->RemoveData( KFField::_recent_player, kfmsg.uids().playerid( i ) );
            }

        }
        break;

        default:
            break;
        }

    }


    KFData* KFRelationClientModule::PBRecentListToKFData( uint32 playerid, const KFMsg::PBRecentData* pbrecentdata,
            const KFMsg::PBStrings* basicdata, const KFDataSetting* kfsetting )
    {
        auto kfrecentobject = _kf_kernel->CreateObject( kfsetting );

        if ( kfrecentobject == nullptr )
        {
            return nullptr;
        }

        kfrecentobject->SetKeyID( playerid );
        auto kfbasic = kfrecentobject->FindData( KFField::_basic );

        for ( auto i = 0; i < basicdata->pbstring_size(); ++i )
        {
            auto pbdata = &basicdata->pbstring( i );

            auto kfdata = kfbasic->FindData( pbdata->name() );

            if ( nullptr != kfdata )
            {
                kfdata->SetValue<std::string >( pbdata->value() );
            }
        }

        // 单局排名
        kfrecentobject->SetValue<uint32 >( KFField::_rank, pbrecentdata->ranking() );

        // 单局游戏人数
        kfrecentobject->SetValue<uint32 >( KFField::_total_player, pbrecentdata->totalnum() );

        // 击杀人数
        kfrecentobject->SetValue<uint32 >( KFField::_kill, pbrecentdata->kill() );

        // 单局评分
        if ( pbrecentdata->has_score() )
        {
            kfrecentobject->SetValue<uint32 >( KFField::_score, pbrecentdata->score() );
        }

        // 关系类型
        if ( pbrecentdata->has_relationtype() )
        {
            kfrecentobject->SetValue<uint32 >( KFField::_relation_type, pbrecentdata->relationtype() );
        }

        else
        {
            kfrecentobject->SetValue<uint32 >( KFField::_relation_type, _invalid_int );
        }

        return kfrecentobject;
    }



}