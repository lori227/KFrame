#include "KFRelationClientModule.hpp"

namespace KFrame
{
#define __RELATION_ROUTE_NAME__ __STRING__( relation )

    void KFRelationClientModule::BeforeRun()
    {
        __REGISTER_PLAYER_ENTER__( &KFRelationClientModule::OnEnterQueryRelation );
        __REGISTER_PLAYER_LEAVE__( &KFRelationClientModule::OnLeaveUpdateRelation );

        // 注册属性回调
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA__( &KFRelationClientModule::OnRelationValueUpdate );
        __REGISTER_UPDATE_STRING__( &KFRelationClientModule::OnRelationStringUpdate );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_ADD_RELATION_REQ, &KFRelationClientModule::HandleAddRelationReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_DEL_RELATION_REQ, &KFRelationClientModule::HandleDelRelationReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_REPLY_RELATION_INVITE_REQ, &KFRelationClientModule::HandleReplyRelationInviteReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_SET_REFUSE_RELATION_INVITE_REQ, &KFRelationClientModule::HandleSetRefuseRelationInviteReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_UPDATE_INT_VALUE_TO_RELATION_REQ, &KFRelationClientModule::HandleUpdateIntValueToRelationReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_UPDATE_STR_VALUE_TO_RELATION_REQ, &KFRelationClientModule::HandleUpdateStrValueToRelationReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_QUERY_RELATION_TO_GAME_ACK, &KFRelationClientModule::HandleQueryRelationToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_ADD_RELATION_TO_GAME_ACK, &KFRelationClientModule::HandleAddRelationToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_DEL_RELATION_TO_GAME_ACK, &KFRelationClientModule::HandleDelRelationToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_APPLY_ADD_RELATION_TO_GAME_ACK, &KFRelationClientModule::HandleApplyAddRelationToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_QUERY_RELATION_INVITE_TO_GAME_ACK, &KFRelationClientModule::HandleQueryRelationInviteToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK, &KFRelationClientModule::HandleUpdateFriendLinessToGameAck );
    }

    void KFRelationClientModule::BeforeShut()
    {
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();

        __UN_UPDATE_DATA__();
        __UN_UPDATE_STRING__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_ADD_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::MSG_DEL_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REPLY_RELATION_INVITE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_REFUSE_RELATION_INVITE_REQ );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_INT_VALUE_TO_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_STR_VALUE_TO_RELATION_REQ );

        __UN_MESSAGE__( KFMsg::S2S_QUERY_RELATION_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_ADD_RELATION_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_DEL_RELATION_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_APPLY_ADD_RELATION_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_RELATION_INVITE_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRelationClientModule::SendMessageToRelation( KFEntity* player, const std::string& relationname, uint32 msgid, google::protobuf::Message* message )
    {
        auto kfrecord = player->Find( relationname );
        if ( kfrecord == nullptr )
        {
            return;
        }

        for ( auto kfrelation = kfrecord->First(); kfrelation != nullptr; kfrelation = kfrecord->Next() )
        {
            auto serverid = kfrelation->Get( __STRING__( basic ), __STRING__( serverid ) );
            if ( serverid == 0u )
            {
                continue;
            }

            _kf_route->RepeatToEntity( player->GetKeyID(), serverid, kfrelation->GetKeyID(), msgid, message );
        }
    }

    void KFRelationClientModule::PBRelationToKFData( const KFMsg::PBRelation* pbrelation, KFData* kfrelation )
    {
        auto pbbasicdata = &pbrelation->basicdata();
        auto kfbasic = kfrelation->Find( __STRING__( basic ) );
        for ( auto iter = pbbasicdata->begin(); iter != pbbasicdata->end(); ++iter )
        {
            auto kfdata = kfbasic->Find( iter->first );
            if ( kfdata == nullptr )
            {
                continue;
            }

            kfdata->Set< std::string >( iter->second );
        }

        auto pbrelationdata = &pbrelation->relationdata();
        for ( auto iter = pbrelationdata->begin(); iter != pbrelationdata->end(); ++iter )
        {
            auto kfdata = kfrelation->Find( iter->first );
            if ( kfdata == nullptr )
            {
                continue;
            }

            kfdata->Set< std::string >( iter->second );
        }

        kfrelation->SetKeyID( pbrelation->playerid() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFRelationClientModule::OnEnterQueryRelation )
    {
        for ( auto& iter : KFRelationConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_online_load )
            {
                continue;
            }

            // 查询关系列表
            {
                KFMsg::S2SQueryRelationToRelationReq req;
                req.set_playerid( player->GetKeyID() );
                req.set_relationname( kfsetting->_id );
                _kf_route->RepeatToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_QUERY_RELATION_TO_RELATION_REQ, &req );
            }

            // 申请列表
            if ( !kfsetting->_invite_data_name.empty() )
            {
                KFMsg::S2SQueryRelationInviteToRelationReq req;
                req.set_playerid( player->GetKeyID() );
                req.set_relationname( kfsetting->_id );
                _kf_route->RepeatToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_QUERY_RELATION_INVITE_TO_RELATION_REQ, &req );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRelationToGameAck, KFMsg::S2SQueryRelationToGameAck )
    {
        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg->relationname() );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto kfrecord = kfentity->Find( kfsetting->_id );
        if ( kfrecord == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no data", kfsetting->_id );
        }

        for ( auto i = 0; i < kfmsg->pbrelation_size(); ++i )
        {
            auto pbrelation = &kfmsg->pbrelation( i );
            auto kfdata = kfentity->CreateData( kfrecord );

            PBRelationToKFData( pbrelation, kfdata );
            kfentity->AddRecord( kfrecord, kfdata );
        }

        if ( kfsetting->_need_update )
        {
            // 通知关系列表,我上线了
            EnterLeaveUpdateToRelation( kfentity, kfsetting->_id );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRelationInviteToGameAck, KFMsg::S2SQueryRelationInviteToGameAck )
    {
        auto kfinviterecord = kfentity->Find( kfmsg->relationname() );
        if ( kfinviterecord == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no invite", kfmsg->relationname() );
        }

        for ( auto i = 0; i < kfmsg->pbinvite_size(); ++i )
        {
            auto pbinvite = &kfmsg->pbinvite( i );
            auto kfinvite = kfentity->CreateData( kfinviterecord );

            PBRelationToKFData( pbinvite, kfinvite );
            kfentity->AddRecord( kfinviterecord, kfinvite );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFRelationClientModule::OnLeaveUpdateRelation )
    {
        for ( auto& iter : KFRelationConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_need_update )
            {
                continue;
            }

            // 通知关系列表,我下线了
            EnterLeaveUpdateToRelation( player, kfsetting->_id );
        }
    }

    void KFRelationClientModule::EnterLeaveUpdateToRelation( KFEntity* player, const std::string& relationname )
    {
        auto kfbasic = player->Find( __STRING__( basic ) );

        StringUInt64 values;
        values[ __STRING__( serverid ) ] = kfbasic->Get( __STRING__( serverid ) );
        values[ __STRING__( status ) ] = kfbasic->Get( __STRING__( status ) );
        values[ __STRING__( statustime ) ] = kfbasic->Get( __STRING__( statustime ) );
        UpdateIntValueToRelation( player, relationname, values );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRelationClientModule::OnRelationValueUpdate )
    {
        if ( !kfdata->HaveMask( KFDataDefine::DataMaskRelation ) ||
                !kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskRelation ) )
        {
            return;
        }

        // 更新给关系, 属性变化了
        for ( auto& iter : KFRelationConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_need_update )
            {
                continue;
            }

            UpdateIntValueToRelation( player, kfsetting->_id, kfdata->_data_setting->_name, newvalue );
        }
    }

    __KF_UPDATE_STRING_FUNCTION__( KFRelationClientModule::OnRelationStringUpdate )
    {
        if ( !kfdata->HaveMask( KFDataDefine::DataMaskRelation ) ||
                !kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskRelation ) )
        {
            return;
        }

        // 更新给关系, 属性变化了
        for ( auto& iter : KFRelationConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_need_update )
            {
                continue;
            }

            UpdateStrValueToRelation( player, kfsetting->_id, kfdata->_data_setting->_name, newvalue );
        }
    }

    void KFRelationClientModule::UpdateIntValueToRelation( KFEntity* player, const std::string& relationname, const std::string& dataname, uint64 datavalue )
    {
        StringUInt64 values;
        values[ dataname ] = datavalue;
        UpdateIntValueToRelation( player, relationname, values );
    }

    void KFRelationClientModule::UpdateIntValueToRelation( KFEntity* player, const std::string& relationname, const StringUInt64& values )
    {
        KFMsg::S2SUpdateIntValueToRelationReq req;
        req.set_relationname( relationname );
        req.set_playerid( player->GetKeyID() );

        auto pbdata = req.mutable_pbdata();
        for ( auto& iter : values )
        {
            ( *pbdata )[ iter.first ] = iter.second;
        }
        SendMessageToRelation( player, relationname, KFMsg::S2S_UPDATE_INT_VALUE_TO_RELATION_REQ, &req );
    }

    void KFRelationClientModule::UpdateStrValueToRelation( KFEntity* player, const std::string& relationname, const std::string& dataname, const std::string& datavalue )
    {
        KFMsg::S2SUpdateStrValueToRelationReq req;
        req.set_relationname( relationname );
        req.set_playerid( player->GetKeyID() );
        ( *req.mutable_pbdata() )[ dataname ] = datavalue;
        SendMessageToRelation( player, relationname, KFMsg::S2S_UPDATE_STR_VALUE_TO_RELATION_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateIntValueToRelationReq, KFMsg::S2SUpdateIntValueToRelationReq )
    {
        auto kfrelation = kfentity->Find( kfmsg->relationname(), kfmsg->playerid() );
        if ( kfrelation == nullptr )
        {
            return;
        }

        // 更新关系属性
        auto pbdata = &kfmsg->pbdata();
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )
        {
            auto kfdata = kfrelation->Find( __STRING__( basic ), iter->first );
            if ( kfdata == nullptr )
            {
                kfdata = kfrelation->Find( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }
            }

            kfentity->UpdateData( kfdata, KFEnum::Set, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateStrValueToRelationReq, KFMsg::S2SUpdateStrValueToRelationReq )
    {
        auto kfrelation = kfentity->Find( kfmsg->relationname(), kfmsg->playerid() );
        if ( kfrelation == nullptr )
        {
            return;
        }

        // 更新关系属性
        auto pbdata = &kfmsg->pbdata();
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )
        {
            auto kfdata = kfrelation->Find( __STRING__( basic ), iter->first );
            if ( kfdata == nullptr )
            {
                kfdata = kfrelation->Find( iter->first );
                if ( kfdata == nullptr )
                {
                    continue;
                }
            }

            kfentity->UpdateData( kfdata, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddRelationReq, KFMsg::MsgAddRelationReq )
    {
        // 不能添加自己
        if ( kfentity->GetKeyID() == kfmsg->playerid() || kfmsg->playerid() == 0u )
        {
            return;
        }

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg->relationname() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::RelationSettingError );
        }

        auto kfrelationrecord = kfentity->Find( kfsetting->_id );
        if ( kfrelationrecord == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::RelationDataError );
        }

        // 已经是关系, 不能申请
        auto kfrelation = kfrelationrecord->Find( kfmsg->playerid() );
        if ( kfrelation != nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::RelationAlready, kfmsg->playername() );
        }

        // 判断自己关系数量
        if ( kfrelationrecord->Size() >= kfsetting->_max_count )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::RelationSelfLimit, kfsetting->_max_count );
        }

        // 不需要申请, 直接添加
        if ( kfsetting->_invite_data_name.empty() )
        {
            return AddRelationToRelation( kfentity, kfmsg->playerid(), kfmsg->playername(), kfrelationrecord, kfsetting );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 检查屏蔽字
        auto message = kfmsg->message();
        KFUtility::ReplaceString( message, " ", "" );
        if ( _kf_filter->CheckFilter( message ) )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::MessageFilterError );
        }

        // 申请列表
        KFMsg::S2SApplyAddRelationToRelationReq req;
        req.set_message( message );
        req.set_playerid( kfmsg->playerid() );
        req.set_playername( kfmsg->playername() );
        req.set_relationname( kfmsg->relationname() );
        auto ok = _kf_route->SendToRand( kfentity->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_APPLY_ADD_RELATION_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( kfentity, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleApplyAddRelationToGameAck, KFMsg::S2SApplyAddRelationToGameAck )
    {
        auto kfinviterecord = kfentity->Find( kfmsg->relationname() );
        if ( kfinviterecord == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] is null", kfmsg->relationname() );
        }

        auto kfinvite = kfentity->CreateData( kfinviterecord );
        PBRelationToKFData( &kfmsg->pbinvite(), kfinvite );
        kfentity->AddRecord( kfinviterecord, kfinvite );

        // 通知玩家
        auto name = kfinvite->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
        _kf_display->DelayToClient( kfentity, KFMsg::RelationInviteReq, name );
    }

    // 查找关系配置
    const KFRelationSetting* KFRelationClientModule::FindRelationSettingByInviteName( const std::string& name )
    {
        for ( auto& iter : KFRelationConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_invite_data_name == name )
            {
                return kfsetting;
            }
        }

        return nullptr;
    }


    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleReplyRelationInviteReq, KFMsg::MsgReplyRelationInviteReq )
    {
        auto kfsetting = FindRelationSettingByInviteName( kfmsg->relationname () );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] setting is null", kfmsg->relationname() );
        }

        auto kfinviterecord = kfentity->Find( kfsetting->_invite_data_name );
        if ( kfinviterecord == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] recored is null", kfsetting->_invite_data_name );
        }

        // playerid 为0, 代表全部操作
        if ( kfmsg->playerid() == _invalid_int )
        {
            ReplyRelationAllInvite( kfentity, kfinviterecord, kfsetting, kfmsg->operate() );
        }
        else
        {
            ReplyRelationInvite( kfentity, kfinviterecord, kfsetting, kfmsg->operate(), kfmsg->playerid() );
        }
    }

    void KFRelationClientModule::ReplyRelationAllInvite( KFEntity* player, KFData* kfinviterecord, const KFRelationSetting* kfsetting, uint32 operate )
    {
        UInt64Set removelist;
        for ( auto kfinvite = kfinviterecord->First(); kfinvite != nullptr; kfinvite = kfinviterecord->Next() )
        {
            auto removeid = ReplyInvite( player, kfsetting, operate, kfinvite );
            if ( removeid != _invalid_int )
            {
                removelist.insert( removeid );
            }
        }

        // 删除申请id
        for ( auto removeid : removelist )
        {
            player->RemoveRecord( kfinviterecord, removeid );
        }
    }

    void KFRelationClientModule::ReplyRelationInvite( KFEntity* player, KFData* kfinviterecord, const KFRelationSetting* kfsetting, uint32 operate, uint64 playerid )
    {
        auto kfinvite = kfinviterecord->Find( playerid );
        if ( kfinvite == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationInviteNotExist );
        }

        auto inviteid = ReplyInvite( player, kfsetting, operate, kfinvite );
        if ( inviteid != _invalid_int )
        {
            player->RemoveRecord( kfinviterecord, inviteid );
        }
    }

    uint64 KFRelationClientModule::ReplyInvite( KFEntity* player, const KFRelationSetting* kfsetting, uint32 operate, KFData* kfinvite )
    {
        auto removeid = _invalid_int;
        switch ( operate )
        {
        case KFMsg::Consent:	// 同意申请
        {
            auto kfrelationrecord = player->Find( kfsetting->_id );
            if ( kfrelationrecord != nullptr )
            {
                auto playerid = kfinvite->GetKeyID();
                auto playername = kfinvite->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
                AddRelationToRelation( player, playerid, playername, kfrelationrecord, kfsetting );
            }
            else
            {
                removeid = kfinvite->GetKeyID();
            }
            break;
        }
        case KFMsg::Refuse:		// 拒绝, 通知对方
        {
            auto targetserverid = kfinvite->Get( __STRING__( basic ), __STRING__( serverid ) );
            auto selfname = player->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
            _kf_display->SendToPlayer( targetserverid, kfinvite->GetKeyID(), KFMsg::RelationRefuseYourInvite, selfname );
        }
        //break; 注释break, 执行下次发送删除消息操作
        case KFMsg::Delete:	// 删除, 不通知对方
        {
            removeid = kfinvite->GetKeyID();

            // 发送到关系集群
            KFMsg::S2SDelRelationInviteToRelationReq req;
            req.set_relationname( kfsetting->_id );
            req.set_selfplayerid( player->GetKeyID() );
            req.set_targetplayerid( kfinvite->GetKeyID() );
            _kf_route->SendToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_DEL_RELATION_INVITE_TO_RELATION_REQ, &req );
        }
        break;
        default:
            break;
        }

        return removeid;
    }

    void KFRelationClientModule::AddRelationToRelation( KFEntity* player, uint64 playerid, const std::string& playername, KFData* kfrelationrecord, const KFRelationSetting* kfsetting )
    {
        // 判断数量
        if ( kfrelationrecord->Size() >= kfsetting->_max_count )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationSelfLimit, kfsetting->_max_count );
        }

        // 发送消息到好友集群
        KFMsg::S2SAddRelationToRelationReq req;
        req.set_playerid( playerid );
        req.set_playername( playername );
        req.set_relationname( kfsetting->_id );
        auto ok = _kf_route->SendToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_ADD_RELATION_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddRelationToGameAck, KFMsg::S2SAddRelationToGameAck )
    {
        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg->relationname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg->relationname() );
        }

        auto kfrelationrecord = kfentity->Find( kfsetting->_id );
        if ( kfrelationrecord == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no record", kfsetting->_id );
        }

        // 添加关系
        auto kfrelation = kfentity->CreateData( kfrelationrecord );
        PBRelationToKFData( &kfmsg->pbrelation(), kfrelation );
        kfentity->AddRecord( kfrelationrecord, kfrelation );

        // 删除邀请
        if ( !kfsetting->_invite_data_name.empty() )
        {
            kfentity->RemoveRecord( kfsetting->_invite_data_name, kfrelation->GetKeyID() );
        }

        auto relationname = kfrelation->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
        _kf_display->DelayToClient( kfentity, KFMsg::RelationAddOk, relationname );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelRelationReq, KFMsg::MsgDelRelationReq )
    {
        auto kfrelation = kfentity->Find( kfmsg->relationname(), kfmsg->playerid() );
        if ( kfrelation == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::RelationNotExist );
        }

        // 发送关系集群处理
        KFMsg::S2SDelRelationToRelationReq req;
        req.set_playerid( kfmsg->playerid() );
        req.set_relationname( kfmsg->relationname() );
        auto ok = _kf_route->SendToRand( kfentity->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_DEL_RELATION_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( kfentity, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelRelationToGameAck, KFMsg::S2SDelRelationToGameAck )
    {
        auto kfrelationrecord = kfentity->Find( kfmsg->relationname() );
        auto kfrelation = kfrelationrecord->Find( kfmsg->relationid() );
        if ( kfrelation == nullptr )
        {
            return;
        }

        auto name = kfrelation->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
        _kf_display->DelayToClient( kfentity, KFMsg::RelationDelOk, kfmsg->relationid(), name );

        // 直接删除关系
        kfentity->RemoveRecord( kfrelationrecord, kfmsg->relationid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleSetRefuseRelationInviteReq, KFMsg::MsgSetRefuseRelationInviteReq )
    {
        auto kfdata = kfentity->Find( kfmsg->refusename() );
        if ( kfdata == nullptr )
        {
            return;
        }

        kfentity->UpdateData( kfdata, KFEnum::Set, kfmsg->refuse() );

        KFMsg::S2SRefuseRelationToRelationReq req;
        req.set_playerid( kfentity->GetKeyID() );
        req.set_refusename( kfmsg->refusename() );
        req.set_refusevalue( kfmsg->refuse() );
        _kf_route->RepeatToRand( kfentity->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_REFUSE_RELATION_TO_RELATION_REQ, &req );
    }

    void KFRelationClientModule::AddFriendLiness( KFEntity* player, uint64 friendid, uint32 type, uint32 value )
    {
        //auto kfobject = player->GetData();
        //auto kffriend = player->Find( __STRING__( friend ), friendid );
        //if ( kffriend == nullptr )
        //{
        //    return;
        //}

        //// 最大好友度限制
        //static auto _max_friend_liness_option = _kf_option->FindOption( __STRING__( freindlinessmax ) );
        //auto friendliness = kffriend->GetValue( __STRING__( friendliness ) );
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

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateFriendLinessToGameAck, KFMsg::S2SUpdateFriendLinessToGameAck )
    {
        auto kffriend = kfentity->Find( __STRING__( friend ), kfmsg->targetplayerid() );
        if ( kffriend == nullptr )
        {
            return;
        }

        // 更新好友度
        kfentity->UpdateObject( kffriend, __STRING__( friendliness ), KFEnum::Add, kfmsg->friendliness() );

        // 显示提示
        auto friendname = kffriend->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
        _kf_display->SendToClient( kfentity, KFMsg::FriendLinessAdd, friendname, kfmsg->friendliness() );
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
    //        __ADD_PB_STRING__( __STRING__( id ), __TO_STRING__( pbscore->playerid() ) );
    //        __ADD_PB_STRING__( __STRING__( score ), __TO_STRING__( pbscore->score() ) );
    //        __ADD_PB_STRING__( __STRING__( rank ), __TO_STRING__( pbscore->ranking() ) );
    //        __ADD_PB_STRING__( __STRING__( totalcount ), __TO_STRING__( pbscore->playercount() ) );
    //        __ADD_PB_STRING__( __STRING__( time ), __TO_STRING__( KFGlobal::Instance()->_real_time ) );
    //
    //        // 其他战斗数据属性
    //        auto kfobject = player->GetData();
    //        auto kfrecentrecord = player->Find( __STRING__( recentplayer ) );
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
    //        player->RemoveData( __STRING__( recentplayer ) );
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
    //        auto kfrecentplayer = player->Find( __STRING__( recentplayer ) );
    //
    //        for ( auto i = 0; i < kfmsg->pbrelation_size(); ++i )
    //        {
    //            auto pbrelation = &kfmsg->pbrelation( i );
    //
    //            auto kfrelation = player->CreateData( kfrecentplayer);
    //            PBRelationToKFData( pbrelation, kfrelation );
    //            player->AddData( kfrecentplayer, kfrelation );
    //        }
    //    }
}