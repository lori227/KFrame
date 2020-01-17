#include "KFRelationClientModule.hpp"

namespace KFrame
{
    void KFRelationClientModule::BeforeRun()
    {
        __REGISTER_ENTER_PLAYER__( &KFRelationClientModule::OnEnterQueryRelation );
        __REGISTER_LEAVE_PLAYER__( &KFRelationClientModule::OnLeaveUpdateRelation );

        // 注册属性回调
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA__( &KFRelationClientModule::OnRelationValueUpdate );
        __REGISTER_UPDATE_STRING__( &KFRelationClientModule::OnRelationStringUpdate );
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
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();

        __UN_UPDATE_DATA__();
        __UN_UPDATE_STRING__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_ADD_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::MSG_DEL_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REPLY_RELATION_INVITE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_REFUSE_RELATION_INVITE_REQ );

        __UN_MESSAGE__( KFMsg::S2S_QUERY_RELATION_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_DATA_TO_RELATION_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ADD_RELATION_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_DEL_RELATION_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_APPLY_ADD_RELATION_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_RELATION_INVITE_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_FRIENDLINESS_TO_GAME_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRelationClientModule::SendToRelation( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_route->SendToRand( playerid, __STRING__( relation ), msgid, message, true );
    }

    void KFRelationClientModule::SendMessageToRelation( KFEntity* player, const std::string& dataname, uint32 msgid, google::protobuf::Message* message )
    {
        auto kfrecord = player->Find( dataname );
        for ( auto kfrelation = kfrecord->First(); kfrelation != nullptr; kfrelation = kfrecord->Next() )
        {
            auto kfbasic = kfrelation->Find( __STRING__( basic ) );
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
            req.set_dataname( kfsetting->_id );
            SendToRelation( playerid, KFMsg::S2S_QUERY_RELATION_TO_RELATION_REQ, &req );
        }

        // 申请列表
        if ( !kfsetting->_invite_data_name.empty() )
        {
            KFMsg::S2SQueryRelationInviteToRelationReq req;
            req.set_playerid( playerid );
            req.set_dataname( kfsetting->_id );
            SendToRelation( playerid, KFMsg::S2S_QUERY_RELATION_INVITE_TO_RELATION_REQ, &req );
        }
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFRelationClientModule::OnEnterQueryRelation )
    {
        for ( auto& iter : KFRelationConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_online_load )
            {
                continue;
            }

            SendQueryToRelation( player->GetKeyID(), kfsetting );
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFRelationClientModule::OnLeaveUpdateRelation )
    {
        for ( auto& iter : KFRelationConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_need_update )
            {
                continue;
            }

            // 通知对方,我下线了
            StringMap values;
            values[ __STRING__( status ) ] = __TO_STRING__( KFMsg::FrameOfflineStatus );
            values[ __STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
            SendUpdateToRelation( player, kfsetting->_id, values );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRelationClientModule::OnRelationValueUpdate )
    {
        UpdateValueToRelation( player, kfdata );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFRelationClientModule::OnRelationStringUpdate )
    {
        UpdateValueToRelation( player, kfdata );
    }

    void KFRelationClientModule::UpdateValueToRelation( KFEntity* player, KFData* kfdata )
    {
        if ( !kfdata->HaveMask( KFDataDefine::Mask_Relation ) || !kfdata->GetParent()->HaveMask( KFDataDefine::Mask_Relation ) )
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

            StringMap values;
            values[ kfdata->_data_setting->_name ] = kfdata->ToString();
            SendUpdateToRelation( player, kfsetting->_id, values );
        }
    }

    void KFRelationClientModule::SendUpdateToRelation( KFEntity* player, const std::string& dataname, StringMap& values )
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

        auto kfrelation = player->Find( kfmsg.dataname(), kfmsg.playerid() );
        if ( kfrelation == nullptr )
        {
            return;
        }

        // 更新关系属性
        auto pbdata = &kfmsg.pbdata();
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

            player->UpdateData( kfdata, iter->second );
        }
    }

    void KFRelationClientModule::PBRelationToKFData( const KFMsg::PBRelation* pbrelation, KFData* kfrelation )
    {
        kfrelation->SetKeyID( pbrelation->playerid() );

        // 玩家基础属性
        auto kfbasic = kfrelation->Find( __STRING__( basic ) );
        auto pbplayerdata = &pbrelation->playerdata();
        for ( auto iter = pbplayerdata->begin(); iter != pbplayerdata->end(); ++iter )
        {
            auto kfdata = kfbasic->Find( iter->first );
            if ( kfdata != nullptr )
            {
                kfdata->Set< std::string >( iter->second );
            }
        }

        // 好友关系属性
        auto pbrelationdata = &pbrelation->relationdata();
        for ( auto iter = pbrelationdata->begin(); iter != pbrelationdata->end(); ++iter )
        {
            auto kfdata = kfrelation->Find( iter->first );
            if ( kfdata != nullptr )
            {
                kfdata->Set< std::string >( iter->second );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRelationToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryRelationToGameAck );

        auto kfrecord = player->Find( kfmsg.dataname() );
        for ( auto i = 0; i < kfmsg.pbrelation_size(); ++i )
        {
            auto pbrelation = &kfmsg.pbrelation( i );
            auto kfdata = player->CreateData( kfrecord );

            PBRelationToKFData( pbrelation, kfdata );
            player->AddData( kfrecord, kfdata );
        }

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting != nullptr && kfsetting->_need_update )
        {
            // 通知关系,我上线了

            StringMap values;
            values[ __STRING__( status ) ] = __TO_STRING__( KFMsg::FrameOnlineStatus );
            values[ __STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
            values[ __STRING__( serverid ) ] = __TO_STRING__( KFGlobal::Instance()->_app_id->GetId() );
            SendUpdateToRelation( player, kfmsg.dataname(), values );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleQueryRelationInviteToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryRelationInviteToGameAck );
        auto kfinviterecord = player->Find( kfmsg.dataname() );

        for ( auto i = 0; i < kfmsg.pbinvite_size(); ++i )
        {
            auto pbinvite = &kfmsg.pbinvite( i );
            auto kfinvite = player->CreateData( kfinviterecord );

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

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationSettingError );
        }

        auto kffriendrecord = player->Find( kfmsg.dataname() );
        if ( kffriendrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RelationDataError );
        }

        // 已经是好友, 不能申请
        auto kffriend = kffriendrecord->Find( kfmsg.playerid() );
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

        auto kfinviterecord = player->Find( kfmsg.dataname() );
        if ( kfinviterecord == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] is null", kfmsg.dataname() );
        }

        auto kfinvite = player->CreateData( kfinviterecord );

        PBRelationToKFData( &kfmsg.pbinvite(), kfinvite );
        player->AddData( kfinviterecord, kfinvite );

        auto name = kfinvite->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::RelationInviteReq, name );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleReplyRelationInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReplyRelationInviteReq );

        auto kfsetting = KFRelationConfig::Instance()->FindRelationSettingByInviteName( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] setting is null", kfmsg.dataname() );
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
        auto kfrecord = player->Find( kfsetting->_invite_data_name );
        if ( kfrecord == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] recored is null", kfsetting->_invite_data_name );
        }

        UInt64Set removelist;
        for ( auto kfinvite = kfrecord->First(); kfinvite != nullptr; kfinvite = kfrecord->Next() )
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
        auto kfrecord = player->Find( kfsetting->_invite_data_name );
        if ( kfrecord == nullptr )
        {
            return __LOG_ERROR__( "invitelist=[{}] recored is null", kfsetting->_invite_data_name );
        }

        auto kfinvite = kfrecord->Find( playerid );
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
            auto playername = kfinvite->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
            AddRelationToRelation( player, playerid, playername, kfsetting );
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

            // 发送到好友集群
            KFMsg::S2SDelRelationInviteToRelationReq req;
            req.set_selfplayerid( player->GetKeyID() );
            req.set_targetplayerid( kfinvite->GetKeyID() );
            req.set_dataname( kfsetting->_id );
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
        auto kfrecord = player->Find( kfsetting->_id );
        if ( kfrecord == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] record is null", kfsetting->_id );
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
        req.set_dataname( kfsetting->_id );
        auto ok = SendToRelation( player->GetKeyID(), KFMsg::S2S_ADD_RELATION_TO_RELATION_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RelationServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleAddRelationToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SAddRelationToGameAck );

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.dataname() );
        }

        auto kfrecord = player->Find( kfmsg.dataname() );
        if ( kfrecord == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no record", kfmsg.dataname() );
        }

        // 添加关系
        auto kfrelation = player->CreateData( kfrecord );
        PBRelationToKFData( &kfmsg.pbrelation(), kfrelation );
        player->AddData( kfrecord, kfrelation );

        // 删除邀请
        if ( !kfsetting->_invite_data_name.empty() )
        {
            player->RemoveData( kfsetting->_invite_data_name, kfrelation->GetKeyID() );
        }

        auto relationname = kfrelation->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::RelationAddOk, relationname );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleDelRelationReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDelRelationReq );

        auto kfrelation = player->Find( kfmsg.dataname(), kfmsg.playerid() );
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

        auto kfrecord = player->Find( kfmsg.dataname() );
        auto kfrelation = kfrecord->Find( kfmsg.relationid() );
        if ( kfrelation == nullptr )
        {
            return;
        }

        auto name = kfrelation->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::RelationDelOk, kfmsg.relationid(), name );

        // 直接删除好友
        player->RemoveData( kfrecord, kfmsg.relationid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleSetRefuseRelationInviteReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetRefuseRelationInviteReq );

        auto kfsetting = KFRelationConfig::Instance()->FindSetting( kfmsg.dataname() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "relation=[{}] no setting", kfmsg.dataname() );
        }

        player->UpdateData( kfsetting->_refuse_name, KFEnum::Set, kfmsg.refuse() );
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

    __KF_MESSAGE_FUNCTION__( KFRelationClientModule::HandleUpdateFriendLinessToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SUpdateFriendLinessToGameAck );

        auto kffriend = player->Find( __STRING__( friend ), kfmsg.targetplayerid() );
        if ( kffriend == nullptr )
        {
            return;
        }

        // 更新好友度
        player->UpdateData( kffriend, __STRING__( friendliness ), KFEnum::Add, kfmsg.friendliness() );

        // 显示提示
        auto friendname = kffriend->Get< std::string >( __STRING__( basic ), __STRING__( name ) );
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
    //        for ( auto i = 0; i < kfmsg.pbrelation_size(); ++i )
    //        {
    //            auto pbrelation = &kfmsg.pbrelation( i );
    //
    //            auto kfrelation = player->CreateData( kfrecentplayer);
    //            PBRelationToKFData( pbrelation, kfrelation );
    //            player->AddData( kfrecentplayer, kfrelation );
    //        }
    //    }
}