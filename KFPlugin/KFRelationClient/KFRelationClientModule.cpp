#include "KFRelationClientModule.hpp"

namespace KFrame
{
#define __RELATION_ROUTE_NAME__ __STRING__( relation )

    void KFRelationClientModule::BeforeRun()
    {
        __REGISTER_PLAYER_ENTER__( &KFRelationClientModule::OnEnterQueryRelation );
        __REGISTER_PLAYER_LEAVE__( &KFRelationClientModule::OnLeaveUpdateRelation );

        // 注册属性回调
        _component = _kf_kernel->FindComponent( __STRING__( player ) );
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
    void KFRelationClientModule::SendMessageToRelation( EntityPtr player, const std::string& relation_name, uint32 msg_id, google::protobuf::Message* message )
    {
        auto relation_record = player->Find( relation_name );
        if ( relation_record == nullptr )
        {
            return;
        }

        for ( auto relation_data = relation_record->First(); relation_data != nullptr; relation_data = relation_record->Next() )
        {
            auto server_id = relation_data->Get( __STRING__( basic ), __STRING__( serverid ) );
            if ( server_id == 0u )
            {
                continue;
            }

            _kf_route->RepeatToEntity( player->GetKeyID(), server_id, relation_data->GetKeyID(), msg_id, message );
        }
    }

    void KFRelationClientModule::PBRelationToKFData( const KFMsg::PBRelation* relation_proto, DataPtr relation_data )
    {
        auto basic_proto = &relation_proto->basicdata();
        auto basic_data = relation_data->Find( __STRING__( basic ) );
        for ( auto iter = basic_proto->begin(); iter != basic_proto->end(); ++iter )
        {
            auto data = basic_data->Find( iter->first );
            if ( data == nullptr )
            {
                continue;
            }

            data->Set<std::string>( iter->second );
        }

        auto relation_data_proto = &relation_proto->relationdata();
        for ( auto iter = relation_data_proto->begin(); iter != relation_data_proto->end(); ++iter )
        {
            auto data = relation_data->Find( iter->first );
            if ( data == nullptr )
            {
                continue;
            }

            data->Set<std::string>( iter->second );
        }

        relation_data->SetKeyID( relation_proto->playerid() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFRelationClientModule::OnEnterQueryRelation )
    {
        for ( auto& iter : KFRelationConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( !setting->_online_load )
            {
                continue;
            }

            // 查询关系列表
            {
                KFMsg::S2SQueryRelationToRelationReq req;
                req.set_playerid( player->GetKeyID() );
                req.set_relationname( setting->_id );
                _kf_route->RepeatToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_QUERY_RELATION_TO_RELATION_REQ, &req );
            }

            // 申请列表
            if ( !setting->_invite_data_name.empty() )
            {
                KFMsg::S2SQueryRelationInviteToRelationReq req;
                req.set_playerid( player->GetKeyID() );
                req.set_relationname( setting->_id );
                _kf_route->RepeatToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_QUERY_RELATION_INVITE_TO_RELATION_REQ, &req );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRelationToGameAck, KFMsg::S2SQueryRelationToGameAck )
    {
        auto setting = KFRelationConfig::Instance()->FindSetting( kfmsg->relationname() );
        if ( setting == nullptr )
        {
            return;
        }

        auto relation_record = player->Find( setting->_id );
        if ( relation_record == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no data", setting->_id );
        }

        for ( auto i = 0; i < kfmsg->pbrelation_size(); ++i )
        {
            auto relation_proto = &kfmsg->pbrelation( i );
            auto relation_data = player->CreateData( relation_record );

            PBRelationToKFData( relation_proto, relation_data );
            player->AddRecord( relation_record, relation_data );
        }

        if ( setting->_need_update )
        {
            // 通知关系列表,我上线了
            EnterLeaveUpdateToRelation( player, setting->_id );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRelationInviteToGameAck, KFMsg::S2SQueryRelationInviteToGameAck )
    {
        auto invite_record = player->Find( kfmsg->relationname() );
        if ( invite_record == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no invite", kfmsg->relationname() );
        }

        for ( auto i = 0; i < kfmsg->pbinvite_size(); ++i )
        {
            auto invite_proto = &kfmsg->pbinvite( i );
            auto invite_data = player->CreateData( invite_record );

            PBRelationToKFData( invite_proto, invite_data );
            player->AddRecord( invite_record, invite_data );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFRelationClientModule::OnLeaveUpdateRelation )
    {
        for ( auto& iter : KFRelationConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( !setting->_need_update )
            {
                continue;
            }

            // 通知关系列表,我下线了
            EnterLeaveUpdateToRelation( player, setting->_id );
        }
    }

    void KFRelationClientModule::EnterLeaveUpdateToRelation( EntityPtr player, const std::string& relation_name )
    {
        auto basic_data = player->Find( __STRING__( basic ) );

        StringUInt64 values;
        values[ __STRING__( serverid ) ] = basic_data->Get( __STRING__( serverid ) );
        values[ __STRING__( status ) ] = basic_data->Get( __STRING__( status ) );
        values[ __STRING__( statustime ) ] = basic_data->Get( __STRING__( statustime ) );
        UpdateIntValueToRelation( player, relation_name, values );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRelationClientModule::OnRelationValueUpdate )
    {
        if ( !data->HaveMask( KFDataDefine::DataMaskRelation ) ||
                !data->GetParent()->HaveMask( KFDataDefine::DataMaskRelation ) )
        {
            return;
        }

        // 更新给关系, 属性变化了
        for ( auto& iter : KFRelationConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( !setting->_need_update )
            {
                continue;
            }

            UpdateIntValueToRelation( player, setting->_id, data->_data_setting->_name, new_value );
        }
    }

    __KF_UPDATE_STRING_FUNCTION__( KFRelationClientModule::OnRelationStringUpdate )
    {
        if ( !data->HaveMask( KFDataDefine::DataMaskRelation ) ||
                !data->GetParent()->HaveMask( KFDataDefine::DataMaskRelation ) )
        {
            return;
        }

        // 更新给关系, 属性变化了
        for ( auto& iter : KFRelationConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( !setting->_need_update )
            {
                continue;
            }

            UpdateStrValueToRelation( player, setting->_id, data->_data_setting->_name, new_value );
        }
    }

    void KFRelationClientModule::UpdateIntValueToRelation( EntityPtr player, const std::string& relation_name, const std::string& data_name, uint64 data_value )
    {
        StringUInt64 values;
        values[data_name] = data_value;
        UpdateIntValueToRelation( player, relation_name, values );
    }

    void KFRelationClientModule::UpdateIntValueToRelation( EntityPtr player, const std::string& relation_name, const StringUInt64& values )
    {
        KFMsg::S2SUpdateIntValueToRelationReq req;
        req.set_relationname( relation_name );
        req.set_playerid( player->GetKeyID() );

        auto& data_list = *req.mutable_pbdata();
        __MAP_TO_PROTO__( values, data_list );
        SendMessageToRelation( player, relation_name, KFMsg::S2S_UPDATE_INT_VALUE_TO_RELATION_REQ, &req );
    }

    void KFRelationClientModule::UpdateStrValueToRelation( EntityPtr player, const std::string& relation_name, const std::string& data_name, const std::string& data_value )
    {
        KFMsg::S2SUpdateStrValueToRelationReq req;
        req.set_relationname( relation_name );
        req.set_playerid( player->GetKeyID() );
        ( *req.mutable_pbdata() )[data_name] = data_value;
        SendMessageToRelation( player, relation_name, KFMsg::S2S_UPDATE_STR_VALUE_TO_RELATION_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateIntValueToRelationReq, KFMsg::S2SUpdateIntValueToRelationReq )
    {
        auto relation_data = player->Find( kfmsg->relationname(), kfmsg->playerid() );
        if ( relation_data == nullptr )
        {
            return;
        }

        // 更新关系属性
        auto data_list = &kfmsg->pbdata();
        for ( auto iter = data_list->begin(); iter != data_list->end(); ++iter )
        {
            auto data = relation_data->Find( __STRING__( basic ), iter->first );
            if ( data == nullptr )
            {
                data = relation_data->Find( iter->first );
                if ( data == nullptr )
                {
                    continue;
                }
            }

            player->UpdateData( data, KFEnum::Set, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateStrValueToRelationReq, KFMsg::S2SUpdateStrValueToRelationReq )
    {
        auto relation_data = player->Find( kfmsg->relationname(), kfmsg->playerid() );
        if ( relation_data == nullptr )
        {
            return;
        }

        // 更新关系属性
        auto data_list = &kfmsg->pbdata();
        for ( auto iter = data_list->begin(); iter != data_list->end(); ++iter )
        {
            auto data = relation_data->Find( __STRING__( basic ), iter->first );
            if ( data == nullptr )
            {
                data = relation_data->Find( iter->first );
                if ( data == nullptr )
                {
                    continue;
                }
            }

            player->UpdateData( data, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddRelationReq, KFMsg::MsgAddRelationReq )
    {
        // 不能添加自己
        if ( player->GetKeyID() == kfmsg->playerid() || kfmsg->playerid() == 0u )
        {
            return;
        }

        auto setting = KFRelationConfig::Instance()->FindSetting( kfmsg->relationname() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationSettingError );
        }

        auto relation_record = player->Find( setting->_id );
        if ( relation_record == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationDataError );
        }

        // 已经是关系, 不能申请
        auto relation_data = relation_record->Find( kfmsg->playerid() );
        if ( relation_data != nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationAlready, kfmsg->playername() );
        }

        // 判断自己关系数量
        if ( relation_record->Size() >= setting->_max_count )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationSelfLimit, setting->_max_count );
        }

        // 不需要申请, 直接添加
        if ( setting->_invite_data_name.empty() )
        {
            return AddRelationToRelation( player, kfmsg->playerid(), kfmsg->playername(), relation_record, setting );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 检查屏蔽字
        auto message = kfmsg->message();
        KFUtility::ReplaceString( message, " ", "" );
        if ( _kf_filter->CheckFilter( message ) )
        {
            return _kf_display->SendToClient( player, KFMsg::MessageFilterError );
        }

        // 申请列表
        KFMsg::S2SApplyAddRelationToRelationReq req;
        req.set_message( message );
        req.set_playerid( kfmsg->playerid() );
        req.set_playername( kfmsg->playername() );
        req.set_relationname( kfmsg->relationname() );
        auto ok = _kf_route->SendToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_APPLY_ADD_RELATION_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleApplyAddRelationToGameAck, KFMsg::S2SApplyAddRelationToGameAck )
    {
        auto invite_record = player->Find( kfmsg->relationname() );
        if ( invite_record == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] is null", kfmsg->relationname() );
        }

        auto invite_data = player->CreateData( invite_record );
        PBRelationToKFData( &kfmsg->pbinvite(), invite_data );
        player->AddRecord( invite_record, invite_data );

        // 通知玩家
        auto name = invite_data->Get<std::string>( __STRING__( basic ), __STRING__( name ) );
        _kf_display->DelayToClient( player, KFMsg::RelationInviteReq, name );
    }

    // 查找关系配置
    std::shared_ptr<const KFRelationSetting> KFRelationClientModule::FindRelationSettingByInviteName( const std::string& name )
    {
        for ( auto& iter : KFRelationConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_invite_data_name == name )
            {
                return setting;
            }
        }

        return nullptr;
    }


    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleReplyRelationInviteReq, KFMsg::MsgReplyRelationInviteReq )
    {
        auto setting = FindRelationSettingByInviteName( kfmsg->relationname() );
        if ( setting == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] setting is null", kfmsg->relationname() );
        }

        auto invite_record = player->Find( setting->_invite_data_name );
        if ( invite_record == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] recored is null", setting->_invite_data_name );
        }

        // player_id 为0, 代表全部操作
        if ( kfmsg->playerid() == _invalid_int )
        {
            ReplyRelationAllInvite( player, invite_record, setting, kfmsg->operate() );
        }
        else
        {
            ReplyRelationInvite( player, invite_record, setting, kfmsg->operate(), kfmsg->playerid() );
        }
    }

    void KFRelationClientModule::ReplyRelationAllInvite( EntityPtr player, DataPtr invite_record, std::shared_ptr<const KFRelationSetting> setting, uint32 operate )
    {
        UInt64Set remove_list;
        for ( auto invite_data = invite_record->First(); invite_data != nullptr; invite_data = invite_record->Next() )
        {
            auto remove_id = ReplyInvite( player, setting, operate, invite_data );
            if ( remove_id != _invalid_int )
            {
                remove_list.insert( remove_id );
            }
        }

        // 删除申请id
        for ( auto remove_id : remove_list )
        {
            player->RemoveRecord( invite_record, remove_id );
        }
    }

    void KFRelationClientModule::ReplyRelationInvite( EntityPtr player, DataPtr invite_record, std::shared_ptr<const KFRelationSetting> setting, uint32 operate, uint64 player_id )
    {
        auto invite_data = invite_record->Find( player_id );
        if ( invite_data == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationInviteNotExist );
        }

        auto invite_id = ReplyInvite( player, setting, operate, invite_data );
        if ( invite_id != _invalid_int )
        {
            player->RemoveRecord( invite_record, invite_id );
        }
    }

    uint64 KFRelationClientModule::ReplyInvite( EntityPtr player, std::shared_ptr<const KFRelationSetting> setting, uint32 operate, DataPtr invite_data )
    {
        auto remove_id = _invalid_int;
        switch ( operate )
        {
        case KFMsg::Consent:	// 同意申请
        {
            auto relation_record = player->Find( setting->_id );
            if ( relation_record != nullptr )
            {
                auto player_id = invite_data->GetKeyID();
                auto player_name = invite_data->Get<std::string>( __STRING__( basic ), __STRING__( name ) );
                AddRelationToRelation( player, player_id, player_name, relation_record, setting );
            }
            else
            {
                remove_id = invite_data->GetKeyID();
            }
            break;
        }
        case KFMsg::Refuse:		// 拒绝, 通知对方
        {
            auto target_server_id = invite_data->Get( __STRING__( basic ), __STRING__( serverid ) );
            auto self_name = player->Get<std::string>( __STRING__( basic ), __STRING__( name ) );
            _kf_display->SendToPlayer( target_server_id, invite_data->GetKeyID(), KFMsg::RelationRefuseYourInvite, self_name );
        }
        //break; 注释break, 执行下次发送删除消息操作
        case KFMsg::Delete:	// 删除, 不通知对方
        {
            remove_id = invite_data->GetKeyID();

            // 发送到关系集群
            KFMsg::S2SDelRelationInviteToRelationReq req;
            req.set_relationname( setting->_id );
            req.set_selfplayerid( player->GetKeyID() );
            req.set_targetplayerid( invite_data->GetKeyID() );
            _kf_route->SendToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_DEL_RELATION_INVITE_TO_RELATION_REQ, &req );
        }
        break;
        default:
            break;
        }

        return remove_id;
    }

    void KFRelationClientModule::AddRelationToRelation( EntityPtr player, uint64 player_id, const std::string& player_name, DataPtr relation_record, std::shared_ptr<const KFRelationSetting> setting )
    {
        // 判断数量
        if ( relation_record->Size() >= setting->_max_count )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationSelfLimit, setting->_max_count );
        }

        // 发送消息到好友集群
        KFMsg::S2SAddRelationToRelationReq req;
        req.set_playerid( player_id );
        req.set_playername( player_name );
        req.set_relationname( setting->_id );
        auto ok = _kf_route->SendToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_ADD_RELATION_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddRelationToGameAck, KFMsg::S2SAddRelationToGameAck )
    {
        auto setting = KFRelationConfig::Instance()->FindSetting( kfmsg->relationname() );
        if ( setting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg->relationname() );
        }

        auto relation_record = player->Find( setting->_id );
        if ( relation_record == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no record", setting->_id );
        }

        // 添加关系
        auto relation_data = player->CreateData( relation_record );
        PBRelationToKFData( &kfmsg->pbrelation(), relation_data );
        player->AddRecord( relation_record, relation_data );

        // 删除邀请
        if ( !setting->_invite_data_name.empty() )
        {
            player->RemoveRecord( setting->_invite_data_name, relation_data->GetKeyID() );
        }

        auto relation_name = relation_data->Get<std::string>( __STRING__( basic ), __STRING__( name ) );
        _kf_display->DelayToClient( player, KFMsg::RelationAddOk, relation_name );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelRelationReq, KFMsg::MsgDelRelationReq )
    {
        auto relation_data = player->Find( kfmsg->relationname(), kfmsg->playerid() );
        if ( relation_data == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationNotExist );
        }

        // 发送关系集群处理
        KFMsg::S2SDelRelationToRelationReq req;
        req.set_playerid( kfmsg->playerid() );
        req.set_relationname( kfmsg->relationname() );
        auto ok = _kf_route->SendToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_DEL_RELATION_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelRelationToGameAck, KFMsg::S2SDelRelationToGameAck )
    {
        auto relation_record = player->Find( kfmsg->relationname() );
        auto relation_data = relation_record->Find( kfmsg->relationid() );
        if ( relation_data == nullptr )
        {
            return;
        }

        auto name = relation_data->Get<std::string>( __STRING__( basic ), __STRING__( name ) );
        _kf_display->DelayToClient( player, KFMsg::RelationDelOk, kfmsg->relationid(), name );

        // 直接删除关系
        player->RemoveRecord( relation_record, kfmsg->relationid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleSetRefuseRelationInviteReq, KFMsg::MsgSetRefuseRelationInviteReq )
    {
        auto data = player->Find( kfmsg->refusename() );
        if ( data == nullptr )
        {
            return;
        }

        player->UpdateData( data, KFEnum::Set, kfmsg->refuse() );

        KFMsg::S2SRefuseRelationToRelationReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_refusename( kfmsg->refusename() );
        req.set_refusevalue( kfmsg->refuse() );
        _kf_route->RepeatToRand( player->GetKeyID(), __RELATION_ROUTE_NAME__, KFMsg::S2S_REFUSE_RELATION_TO_RELATION_REQ, &req );
    }

    void KFRelationClientModule::AddFriendliness( EntityPtr player, uint64 friend_id, uint32 type, uint32 value )
    {
        //auto object_data = player->GetData();
        //auto friend_data = player->Find( __STRING__( friend ), friendid );
        //if ( friend_data == nullptr )
        //{
        //    return;
        //}

        //// 最大好友度限制
        //static auto _max_friend_liness_option = _kf_option->FindOption( __STRING__( freindlinessmax ) );
        //auto friendliness = friend_data->GetValue( __STRING__( friendliness ) );
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

    void KFRelationClientModule::AddFriendlinessOnce( EntityPtr player, uint64 friend_id, uint32 type, uint32 value )
    {
        // 好友度是双向的, 数据只保存了一条, 所以更新的时候使用id比较小的一个来做逻辑
        if ( player->GetKeyID() > friend_id )
        {
            return;
        }

        AddFriendliness( player, friend_id, type, value );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateFriendLinessToGameAck, KFMsg::S2SUpdateFriendLinessToGameAck )
    {
        auto friend_data = player->Find( __STRING__( friend ), kfmsg->targetplayerid() );
        if ( friend_data == nullptr )
        {
            return;
        }

        // 更新好友度
        player->UpdateObject( friend_data, __STRING__( friendliness ), KFEnum::Add, kfmsg->friendliness() );

        // 显示提示
        auto friend_name = friend_data->Get<std::string>( __STRING__( basic ), __STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::FriendLinessAdd, friend_name, kfmsg->friendliness() );
    }

    //    void KFRelationClientModule::AddRecentPlayer( EntityPtr player, uint64 roomid, const KFMsg::PBBattleScore* pbscore )
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
    //        auto object_data = player->GetData();
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
    //        req.set_playerid( player_id );
    //        SendMessageToRelation( KFMsg::S2S_QUERY_RECENT_LIST_REQ, &req );
    //    }
    //
    //    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRecentListAck )
    //    {
    //        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryRecentListAck );
    //
    //        auto object_data = player->GetData();
    //        auto kfrecentplayer = player->Find( __STRING__( recentplayer ) );
    //
    //        for ( auto i = 0; i < kfmsg->pbrelation_size(); ++i )
    //        {
    //            auto relation_proto = &kfmsg->relation_proto( i );
    //
    //            auto relation_data = player->CreateData( kfrecentplayer);
    //            PBRelationToKFData( relation_proto, relation_data );
    //            player->AddData( kfrecentplayer, relation_data );
    //        }
    //    }
}