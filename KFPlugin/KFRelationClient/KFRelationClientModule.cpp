#include "KFRelationClientModule.hpp"

namespace KFrame
{
    void KFRelationClientModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_relation_client_config, true );
    }

    void KFRelationClientModule::BeforeRun()
    {
        _kf_player->RegisterEnterFunction( this, &KFRelationClientModule::OnEnterQueryRelation );
        _kf_player->RegisterLeaveFunction( this, &KFRelationClientModule::OnLeaveUpdateRelation );

        // 注册属性回调
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterUpdateDataModule( this, &KFRelationClientModule::OnRelationValueUpdate );
        _kf_component->RegisterUpdateStringModule( this, &KFRelationClientModule::OnRelationStringUpdate );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_ADD_RELATION_REQ, &KFRelationClientModule::HandleAddRelationReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_DEL_RELATION_REQ, &KFRelationClientModule::HandleDelRelationReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REPLY_RELATION_INVITE_REQ, &KFRelationClientModule::HandleReplyRelationInviteReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_REFUSE_RELATION_INVITE_REQ, &KFRelationClientModule::HandleSetRefuseRelationInviteReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RELATION_TO_GAME_ACK, &KFRelationClientModule::HandleQueryRelationToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_DATA_TO_RELATION_REQ, &KFRelationClientModule::HandleUpdateDataToRelationReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_RELATION_TO_GAME_ACK, &KFRelationClientModule::HandleAddRelationToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_RELATION_TO_GAME_ACK, &KFRelationClientModule::HandleDelRelationToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_APPLY_ADD_RELATION_TO_GAME_ACK, &KFRelationClientModule::HandleApplyAddRelationToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RELATION_INVITE_TO_GAME_ACK, &KFRelationClientModule::HandleQueryRelationInviteToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK, &KFRelationClientModule::HandleUpdateFriendLinessToGameAck );
    }

    void KFRelationClientModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_relation_client_config );

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );

        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateStringModule( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_ADD_RELATION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_DEL_RELATION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REPLY_RELATION_INVITE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_REFUSE_RELATION_INVITE_REQ );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_RELATION_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_DATA_TO_RELATION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_RELATION_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEL_RELATION_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_APPLY_ADD_RELATION_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_RELATION_INVITE_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRelationClientModule::SendToRelation( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_route->SendToRand( playerid, __KF_STRING__( relation ), msgid, message );
    }

    void KFRelationClientModule::SendMessageToRelation( KFEntity* player, const std::string& dataname, uint32 msgid, google::protobuf::Message* message )
    {
        auto kfobject = player->GetData();
        auto kfrecord = kfobject->FindData( dataname );
        for ( auto kfrelation = kfrecord->FirstData(); kfrelation != nullptr; kfrelation = kfrecord->NextData() )
        {
            auto kfbasic = kfrelation->FindData( __KF_STRING__( basic ) );
            _kf_game->SendToPlayer( player->GetKeyID(), kfbasic, msgid, message );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationClientModule::SendQueryToRelation( uint64 playerid, const KFRelationSetting* kfsetting )
    {
        // 查询关系列表
        {
            KFMsg::S2SQueryRelationToRelationReq req;
            req.set_playerid( playerid );
            req.set_dataname( kfsetting->_data_name );
            SendToRelation( playerid, KFMsg::S2S_QUERY_RELATION_TO_RELATION_REQ, &req );
        }

        // 申请列表
        if ( !kfsetting->_invite_data_name.empty() )
        {
            KFMsg::S2SQueryRelationInviteToRelationReq req;
            req.set_playerid( playerid );
            req.set_dataname( kfsetting->_data_name );
            SendToRelation( playerid, KFMsg::S2S_QUERY_RELATION_INVITE_TO_RELATION_REQ, &req );
        }
    }

    void KFRelationClientModule::OnEnterQueryRelation( KFEntity* player )
    {
        for ( auto& iter : _kf_relation_client_config->_relation_list._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_online_load )
            {
                continue;
            }

            SendQueryToRelation( player->GetKeyID(), kfsetting );
        }
    }

    void KFRelationClientModule::OnLeaveUpdateRelation( KFEntity* player )
    {
        for ( auto& iter : _kf_relation_client_config->_relation_list._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_need_update )
            {
                continue;
            }

            // 通知对方,我下线了
            MapString values;
            values[ __KF_STRING__( status ) ] = __TO_STRING__( KFMsg::OfflineState );
            values[ __KF_STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
            SendUpdateToRelation( player, kfsetting->_data_name, values );
        }
    }

    void KFRelationClientModule::OnRelationValueUpdate( KFEntity* player, KFData* kfdata )
    {
        if ( !kfdata->HaveFlagMask( KFDataDefine::Mask_Relation ) ||
                !kfdata->GetParent()->HaveFlagMask( KFDataDefine::Mask_Relation ) )
        {
            return;
        }

        // 更新给关系, 属性变化了
        for ( auto& iter : _kf_relation_client_config->_relation_list._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_need_update )
            {
                continue;
            }

            MapString values;
            values[ kfdata->GetName() ] = kfdata->ToString();
            SendUpdateToRelation( player, kfsetting->_data_name, values );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRelationClientModule::OnRelationValueUpdate )
    {
        OnRelationValueUpdate( player, kfdata );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFRelationClientModule::OnRelationStringUpdate )
    {
        OnRelationValueUpdate( player, kfdata );
    }

    void KFRelationClientModule::SendUpdateToRelation( KFEntity* player, const std::string& dataname, MapString& values )
    {
        KFMsg::S2SUpdateDataToRelationReq req;
        req.set_dataname( dataname );
        req.set_playerid( player->GetKeyID() );

        auto pbdata = req.mutable_pbdata();
        for ( auto& iter : values )
        {
            ( *pbdata )[ iter.first ] = iter.second;
        }

        SendMessageToRelation( player, dataname, KFMsg::S2S_UPDATE_DATA_TO_RELATION_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateDataToRelationReq )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2SUpdateDataToRelationReq );

        auto kfobject = player->GetData();
        auto kfrelation = kfobject->FindData( kfmsg.dataname(), kfmsg.playerid() );
        if ( kfrelation == nullptr )
        {
            return;
        }

        // 更新关系属性
        auto pbdata = &kfmsg.pbdata();
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )
        {
            auto kfdata = kfrelation->FindData( __KF_STRING__( basic ), iter->first );
            if ( kfdata == nullptr )
            {
                kfdata = kfrelation->FindData( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }
            }

            player->UpdateData( kfdata, iter->second );
        }
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

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRelationToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryRelationToGameAck );

        auto kfobject = player->GetData();
        auto kfrecord = kfobject->FindData( kfmsg.dataname() );

        for ( auto i = 0; i < kfmsg.pbrelation_size(); ++i )
        {
            auto pbrelation = &kfmsg.pbrelation( i );
            auto kfdata = _kf_kernel->CreateObject( kfrecord->GetDataSetting() );

            PBRelationToKFData( pbrelation, kfdata );
            player->AddData( kfrecord, kfdata );
        }

        auto kfsetting = _kf_relation_client_config->FindRelationSetting( kfmsg.dataname() );
        if ( kfsetting != nullptr && kfsetting->_need_update )
        {
            // 通知关系,我上线了

            MapString values;
            values[ __KF_STRING__( status ) ] = __TO_STRING__( KFMsg::OnlineState );
            values[ __KF_STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
            values[ __KF_STRING__( serverid ) ] = __TO_STRING__( KFGlobal::Instance()->_app_id->GetId() );
            SendUpdateToRelation( player, kfmsg.dataname(), values );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRelationInviteToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryRelationInviteToGameAck );

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( kfmsg.dataname() );

        for ( auto i = 0; i < kfmsg.pbinvite_size(); ++i )
        {
            auto pbinvite = &kfmsg.pbinvite( i );
            auto kfinvite = _kf_kernel->CreateObject( kfinviterecord->GetDataSetting() );

            PBRelationToKFData( pbinvite, kfinvite );
            player->AddData( kfinviterecord, kfinvite );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddRelationReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgAddRelationReq );

        // 不能添加自己
        if ( playerid == kfmsg.playerid() )
        {
            return;
        }

        auto kfsetting = _kf_relation_client_config->FindRelationSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationSettingError );
        }

        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( kfmsg.dataname() );
        if ( kffriendrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationDataError );
        }

        // 已经是好友, 不能申请
        auto kffriend = kffriendrecord->FindData( kfmsg.playerid() );
        if ( kffriend != nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationAlready, kfmsg.playername() );
        }

        // 判断自己好友数量
        if ( kffriendrecord->Size() >= kfsetting->_max_count )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationSelfLimit, kfsetting->_max_count );
        }

        // 检查屏蔽字
        auto message = kfmsg.message();
        KFUtility::ReplaceString( message, " ", "" );
        if ( _kf_filter->CheckFilter( message ) )
        {
            return _kf_display->SendToClient( player, KFMsg::MessageFilterError );
        }

        // 发送关系集群处理
        if ( !kfsetting->_invite_data_name.empty() )
        {
            // 申请列表
            KFMsg::S2SApplyAddRelationToRelationReq req;
            req.set_message( message );
            req.set_dataname( kfmsg.dataname() );
            req.set_playerid( kfmsg.playerid() );
            req.set_playername( kfmsg.playername() );
            auto ok = SendToRelation( playerid, KFMsg::S2S_APPLY_ADD_RELATION_TO_RELATION_REQ, &req );
            if ( !ok )
            {
                _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
            }
        }
        else
        {
            // 直接添加
            AddRelationToRelation( player, kfmsg.playerid(), kfmsg.playername(), kfsetting );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleApplyAddRelationToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SApplyAddRelationToGameAck );

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( kfmsg.dataname() );
        if ( kfinviterecord == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] is null!", kfmsg.dataname() );
        }

        auto kfinvite = _kf_kernel->CreateObject( kfinviterecord->GetDataSetting() );

        PBRelationToKFData( &kfmsg.pbinvite(), kfinvite );
        player->AddData( kfinviterecord, kfinvite );

        auto name = kfinvite->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::RelationInviteReq, name );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleReplyRelationInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReplyRelationInviteReq );

        auto kfsetting = _kf_relation_client_config->FindRelationSettingByInviteName( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] setting is null!", kfmsg.dataname() );
        }

        // playerid 为0, 代表全部操作
        if ( kfmsg.playerid() == _invalid_int )
        {
            ReplyRelationInvite( player, kfmsg.operate(), kfsetting );
        }
        else
        {
            ReplyRelationInvite( player, kfmsg.playerid(), kfmsg.operate(), kfsetting );
        }
    }

    void KFRelationClientModule::ReplyRelationInvite( KFEntity* player, uint32 operate, const KFRelationSetting* kfsetting )
    {
        auto kfobject = player->GetData();
        auto kfrecord = kfobject->FindData( kfsetting->_invite_data_name );
        if ( kfrecord == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] recored is null!", kfsetting->_invite_data_name );
        }

        std::list< uint64 > removelist;
        for ( auto kfinvite = kfrecord->FirstData(); kfinvite != nullptr; kfinvite = kfrecord->NextData() )
        {
            auto removeid = ReplyRelationInvite( player, kfinvite, operate, kfsetting );
            if ( removeid != _invalid_int )
            {
                removelist.push_back( removeid );
            }
        }

        // 删除申请id
        for ( auto removeid : removelist )
        {
            player->RemoveData( kfrecord, removeid );
        }
    }

    void KFRelationClientModule::ReplyRelationInvite( KFEntity* player, uint64 playerid, uint32 operate, const KFRelationSetting* kfsetting )
    {
        auto kfobject = player->GetData();
        auto kfrecord = kfobject->FindData( kfsetting->_invite_data_name );
        if ( kfrecord == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] recored is null!", kfsetting->_invite_data_name );
        }

        auto kfinvite = kfrecord->FindData( playerid );
        if ( kfinvite == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationInviteNotExist );
        }

        auto inviteid = ReplyRelationInvite( player, kfinvite, operate, kfsetting );
        if ( inviteid != _invalid_int )
        {
            player->RemoveData( kfrecord, inviteid );
        }
    }

    uint64 KFRelationClientModule::ReplyRelationInvite( KFEntity* player, KFData* kfinvite, uint32 operate, const KFRelationSetting* kfsetting )
    {
        auto removeid = _invalid_int;
        switch ( operate )
        {
        case KFMsg::Consent:	// 同意申请
        {
            auto playerid = kfinvite->GetKeyID();
            auto playername = kfinvite->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
            AddRelationToRelation( player, playerid, playername, kfsetting );
            break;
        }
        case KFMsg::Refuse:		// 拒绝, 通知对方
        {
            auto kfobject = player->GetData();
            auto targetserverid = kfinvite->GetValue( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
            auto selfname = kfobject->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
            _kf_display->SendToPlayer( targetserverid, kfinvite->GetKeyID(), KFMsg::RelationRefuseYourInvite, selfname );
        }
        //break; 注释break, 执行下次发送删除消息操作
        case KFMsg::Delete:	// 删除, 不通知对方
        {
            removeid = kfinvite->GetKeyID();

            // 发送到好友集群
            KFMsg::S2SDelRelationInviteToRelationReq req;
            req.set_selfplayerid( player->GetKeyID() );
            req.set_targetplayerid( kfinvite->GetKeyID() );
            req.set_dataname( kfsetting->_data_name );
            SendToRelation( player->GetKeyID(), KFMsg::S2S_DEL_RELATION_INVITE_TO_RELATION_REQ, &req );
        }
        break;
        default:
            break;
        }

        return removeid;
    }

    void KFRelationClientModule::AddRelationToRelation( KFEntity* player, uint64 playerid, const std::string& playername, const KFRelationSetting* kfsetting )
    {
        // 判断自己的好友数量
        auto kfobject = player->GetData();
        auto kfrecord = kfobject->FindData( kfsetting->_data_name );
        if ( kfrecord == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] record is null!", kfsetting->_data_name );
        }

        // 判断数量
        if ( kfrecord->Size() >= kfsetting->_max_count )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationSelfLimit, kfsetting->_max_count );
        }

        // 发送消息到好友集群
        KFMsg::S2SAddRelationToRelationReq req;
        req.set_playerid( playerid );
        req.set_playername( playername );
        req.set_dataname( kfsetting->_data_name );
        auto ok = SendToRelation( player->GetKeyID(), KFMsg::S2S_ADD_RELATION_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddRelationToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SAddRelationToGameAck );

        auto kfsetting = _kf_relation_client_config->FindRelationSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting!", kfmsg.dataname() );
        }

        auto kfobject = player->GetData();
        auto kfrecord = kfobject->FindData( kfmsg.dataname() );
        if ( kfrecord == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no record!", kfmsg.dataname() );
        }

        // 添加关系
        auto kfrelation = _kf_kernel->CreateObject( kfrecord->GetDataSetting() );
        PBRelationToKFData( &kfmsg.pbrelation(), kfrelation );
        player->AddData( kfrecord, kfrelation );

        // 删除邀请
        if ( !kfsetting->_invite_data_name.empty() )
        {
            player->RemoveData( kfsetting->_invite_data_name, kfrelation->GetKeyID() );
        }

        auto relationname = kfrelation->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::RelationAddOk, relationname );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelRelationReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDelRelationReq );

        auto kfobject = player->GetData();
        auto kfrelation = kfobject->FindData( kfmsg.dataname(), kfmsg.playerid() );
        if ( kfrelation == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationNotExist );
        }

        // 发送好友集群处理
        KFMsg::S2SDelRelationToRelationReq req;
        req.set_dataname( kfmsg.dataname() );
        req.set_playerid( kfmsg.playerid() );
        auto ok = SendToRelation( playerid, KFMsg::S2S_DEL_RELATION_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelRelationToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SDelRelationToGameAck );

        auto kfobject = player->GetData();
        auto kfrecord = kfobject->FindData( kfmsg.dataname() );
        auto kfrelation = kfrecord->FindData( kfmsg.relationid() );
        if ( kfrelation == nullptr )
        {
            return;
        }

        auto name = kfrelation->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::RelationDelOk, kfmsg.relationid(), name );

        // 直接删除好友
        player->RemoveData( kfrecord, kfmsg.relationid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleSetRefuseRelationInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetRefuseRelationInviteReq );

        auto kfsetting = _kf_relation_client_config->FindRelationSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting!", kfmsg.dataname() );
        }

        player->UpdateData( kfsetting->_refuse_name, KFEnum::Set, kfmsg.refuse() );
    }

    void KFRelationClientModule::AddFriendLiness( KFEntity* player, uint64 friendid, uint32 type, uint32 value )
    {
        //auto kfobject = player->GetData();
        //auto kffriend = kfobject->FindData( __KF_STRING__( friend ), friendid );
        //if ( kffriend == nullptr )
        //{
        //    return;
        //}

        //// 最大好友度限制
        //static auto _max_friend_liness_option = _kf_option->FindOption( __KF_STRING__( freindlinessmax ) );
        //auto friendliness = kffriend->GetValue( __KF_STRING__( friendliness ) );
        //if ( friendliness >= _max_friend_liness_option->_uint32_value )
        //{
        //    return;
        //}

        //KFMsg::S2SUpdateFriendLinessToRelationReq req;
        //req.set_type( type );
        //req.set_friendliness( value );
        //req.set_targetplayerid( friendid );
        //req.set_selfplayerid( player->GetKeyID() );
        //SendToRelation( player->GetKeyID(), KFMsg::S2S_UPDATE_FRIENDLINESS_TO_RELATION_REQ, &req );
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
        player->UpdateData( kffriend, __KF_STRING__( friendliness ), KFEnum::Add, kfmsg.friendliness() );

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