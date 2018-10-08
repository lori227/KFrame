#include "KFGuildClientModule.h"
#include "KFProtocol/KFProtocol.h"
#include "KFGuildClientConfig.h"

namespace KFrame
{
    KFGuildClientModule::KFGuildClientModule()
    {
        _kf_component = nullptr;
    }

    KFGuildClientModule::~KFGuildClientModule()
    {

    }

    void KFGuildClientModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_guild_config, true );
    }

    void KFGuildClientModule::BeforeRun()
    {
        _kf_player->RegisterEnterFunction( this, &KFGuildClientModule::OnEnterQueryGuild );
        _kf_player->RegisterLeaveFunction( this, &KFGuildClientModule::OnLeaveUpdateGuildData );

        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterUpdateDataModule( this, &KFGuildClientModule::OnUpdateDataCallBack );
        _kf_component->RegisterUpdateStringModule( this, &KFGuildClientModule::OnUpdateStringCallBack );
        _kf_component->RegisterUpdateStringFunction( __KF_STRING__( guild ), __KF_STRING__( applicantlist ), this, &KFGuildClientModule::OnGuildApplicantUpdateCallBack );

        __REGISTER_MESSAGE__( KFMsg::MSG_CREATE_GUILD_REQ, &KFGuildClientModule::HandleCreateGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_ACK, &KFGuildClientModule::HandleCreateGuildAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_INVITE_GUILD_REQ, &KFGuildClientModule::HnadleInviteGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_INVITE_GUILD_ACK, &KFGuildClientModule::HnadleInviteGuildAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_APPLY_GUILD_REQ, &KFGuildClientModule::HandleApplyGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_APPLY_GUILD_ACK, &KFGuildClientModule::HandleApplyGuildAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_GUILD_CHANGE_REQ, &KFGuildClientModule::HandlePlayerGuildChangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXIT_GUILD_REQ, &KFGuildClientModule::HandleExitGuildReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRANSFER_MASTER_REQ, &KFGuildClientModule::HandleTransferGuildReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REVIEW_APPLY_REQ, &KFGuildClientModule::HandleReviewApplyReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_DISSOLVE_GUILD_REQ, &KFGuildClientModule::HandleDissolveGuildReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MODIFY_GUILD_MEDAL_REQ, &KFGuildClientModule::HandleModifyMedalReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUILD_LIST_REQ, &KFGuildClientModule::HandleQueryGuildListReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_KICK_MEMBER_REQ, &KFGuildClientModule::HandleKickMemberReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILD_ACK, &KFGuildClientModule::HandleQueryGuildAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPGRADE_GUILD_REQ, &KFGuildClientModule::HandleUpgradeGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPGRADE_GUILD_ACK, &KFGuildClientModule::HandleUpgradeGuildAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_APPOINT_GUILD_MEMBER_REQ, &KFGuildClientModule::HandleAppointGuildMemberReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SEARCH_GUILD_BY_NAME_REQ, &KFGuildClientModule::HandleSearchGuildByNameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_GUILD_SWITCH_REQ, &KFGuildClientModule::HandleSetSwitchReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILDID_ACK, &KFGuildClientModule::HandleLoginQueryGuildIdAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUILD_LOG_REQ, &KFGuildClientModule::HandleQueryGuildLogReq );

        __REGISTER_MESSAGE__( KFMsg::MSG_SYNC_UPDATE_GUILD_DATA, &KFGuildClientModule::HandleUpDateGuildAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_SYNC_ADD_GUILD_DATA, &KFGuildClientModule::HandleAddGuildAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_SYNC_REMOVE_GUILD_DATA, &KFGuildClientModule::HandleRemoveGuildAck );

    }

    void KFGuildClientModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CREATE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_INVITE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_INVITE_GUILD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_APPLY_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_APPLY_GUILD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_GUILD_CHANGE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_EXIT_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_TRANSFER_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REVIEW_APPLY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_DISSOLVE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_MODIFY_GUILD_MEDAL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUILD_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_KICK_MEMBER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_UPGRADE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPGRADE_GUILD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_APPOINT_GUILD_MEMBER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SEARCH_GUILD_BY_NAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_GUILD_SWITCH_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILDID_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUILD_LOG_REQ );

        __UNREGISTER_MESSAGE__( KFMsg::MSG_SYNC_UPDATE_GUILD_DATA );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SYNC_ADD_GUILD_DATA );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SYNC_REMOVE_GUILD_DATA );

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
    }

    void KFGuildClientModule::OnceRun()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFGuildClientModule::SendMessageToGuild( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToShard( __KF_STRING__( guild ), msgid, message );
    }

    bool KFGuildClientModule::SendMessageToGuild( uint64 guildid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendToDynamicObject( __KF_STRING__( guild ), guildid, msgid, message );
    }

    void KFGuildClientModule::OnEnterQueryGuild( KFEntity* player )
    {
        KFMsg::S2SLoginQueryGuildidReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGuild( KFMsg::S2S_LOGIN_QUERY_GUILDID_REQ, &req );

    }

    void KFGuildClientModule::OnEnterUpdateGuildData( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );
        if ( guildid == _invalid_int )
        {
            return;
        }
        MapString values;
        values[__KF_STRING__( serverid )] = __TO_STRING__( KFGlobal::Instance()->_app_id );
        values[__KF_STRING__( id )] = __TO_STRING__( player->GetKeyID() );
        values[__KF_STRING__( status )] = __TO_STRING__( KFMsg::OnlineStatus );
        values[__KF_STRING__( statustime )] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        if ( !UpdatePublicData( guildid, player->GetKeyID(), values ) )
        {
            __LOG_ERROR__( "player[{}] login update guild basic failed!", player->GetKeyID() );
        }

    }

    void KFGuildClientModule::OnLeaveUpdateGuildData( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );
        if ( guildid == _invalid_int )
        {
            return;
        }

        MapString values;
        values[__KF_STRING__( serverid )] = "0";
        values[__KF_STRING__( groupid )] = "0";
        values[__KF_STRING__( status )] = __TO_STRING__( KFMsg::OfflineStatus );
        values[__KF_STRING__( statustime )] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        if ( !UpdatePublicData( guildid, player->GetKeyID(), values ) )
        {
            __LOG_ERROR__( "player[{}] login update guild basic failed!", player->GetKeyID() );
        }
    }

    bool KFGuildClientModule::UpdatePublicData( uint64 guildid, uint32 playerid, const MapString& values )
    {
        KFMsg::S2SUpdateGuildDataReq req;
        req.set_playerid( playerid );
        req.set_guildid( guildid );
        for ( auto& iter : values )
        {
            auto pbstring = req.add_pbdata();
            pbstring->set_name( iter.first );
            pbstring->set_value( iter.second );
        }
        return SendMessageToGuild( guildid, KFMsg::S2S_UPDATE_GUILD_DATA_REQ, &req );

    }

    void KFGuildClientModule::UpdateMemberBasic( uint32 playerid, uint64 guildid, MapString& basics )
    {
        if ( _invalid_int == guildid || _invalid_int == playerid )
        {
            return;
        }
        KFMsg::S2SUpdateMemberBasicDataReq req;
        req.set_guildid( guildid );
        req.set_playerid( playerid );
        for ( auto iter : basics )
        {
            auto pbstring = req.add_pbdata();
            pbstring->set_name( iter.first );
            pbstring->set_value( iter.second );
        }

        SendMessageToGuild( guildid, KFMsg::S2S_UPDATE_MEMBER_BASIC_DATA_REQ, &req );

    }

    __KF_UPDATE_DATA_FUNCTION__( KFGuildClientModule::OnUpdateDataCallBack )
    {
        // 还没有初始化
        if ( !player->IsInited() )
        {
            return;
        }

        // 更新到帮派属性集群
        if ( !kfdata->HaveFlagMask( KFDataDefine::Mask_Guild_Data ) ||
                !kfdata->GetParent()->HaveFlagMask( KFDataDefine::Mask_Guild_Data ) )
        {
            return;
        }
        auto kfobject = player->GetData();
        auto kfbaisc = kfobject->FindData( __KF_STRING__( basic ) );
        auto guildid = kfbaisc->GetValue<uint64>( __KF_STRING__( guildid ) );
        if ( _invalid_int == guildid )
        {
            return;
        }

        MapString guildvalues;
        guildvalues[kfdata->GetName()] = kfdata->ToString();
        UpdateMemberBasic( player->GetKeyID(), guildid, guildvalues );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFGuildClientModule::OnUpdateStringCallBack )
    {
        // 还没有初始化
        if ( !player->IsInited() )
        {
            return;
        }

        if ( !kfdata->HaveFlagMask( KFDataDefine::Mask_Guild_Data ) ||
                !kfdata->GetParent()->HaveFlagMask( KFDataDefine::Mask_Guild_Data ) )
        {
            return;
        }
        auto kfobject = player->GetData();
        auto kfbaisc = kfobject->FindData( __KF_STRING__( basic ) );
        auto guildid = kfbaisc->GetValue<uint64>( __KF_STRING__( guildid ) );
        if ( _invalid_int == guildid )
        {
            return;
        }

        MapString guildvalues;
        guildvalues[kfdata->GetName()] = kfdata->ToString();
        UpdateMemberBasic( player->GetKeyID(), guildid, guildvalues );
    }



    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleCreateGuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCreateGuildReq );

        auto guilddatasetting = _kf_guild_config->FindGuildSetting( KFGuildEnum::InitLevel );
        if ( nullptr == guilddatasetting )
        {
            __LOG_ERROR__( "player[{}] create guild get guild config failed!", playerid );
            return;
        }

        // 输入规则检测
        //TODO:敏感词检测未加
        if ( kfmsg.guildname().empty() || !guilddatasetting->IsValidMedal( kfmsg.medal() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildCreateIllegal );
        }

        if ( kfmsg.guildname().length() > KFGuildEnum::MaxNameLength )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildNameTooLong );
        }

        if ( kfmsg.has_manifesto() && ( kfmsg.manifesto().length() > KFGuildEnum::MaxManifesto
                                        || !IsValidName( kfmsg.manifesto() ) ) )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildManifestoTooLong );
        }

        if ( !IsValidName( kfmsg.guildname() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildNameInvalid );
        }


        // 判断是否已经加入帮派
        auto kfobject = player->GetData();

        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );
        if ( _invalid_int != guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildHadExist );
        }

        //
        if ( !player->CheckAgentData( &guilddatasetting->_cost_item, __FUNC_LINE__ ) )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough );
        }

        // 先扣钱失败回调后在加钱
        auto name = kfobject->GetValue<std::string>( __KF_STRING__( basic ), __KF_STRING__( name ) );
        KFMsg::S2SCreateGuidReq req;
        req.set_playerid( playerid );
        req.set_name( name );
        req.set_guildname( kfmsg.guildname() );
        req.set_medal( kfmsg.medal() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        if ( SendMessageToGuild( KFMsg::S2S_CREATE_GUILD_REQ, &req ) )
        {
            player->RemoveAgentData( &guilddatasetting->_cost_item, __FUNC_LINE__ );
        }
        else
        {
            __LOG_ERROR__( "player[{}] try create guild[{}] failed!", playerid, kfmsg.guildname() );
        }

    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleCreateGuildAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SCreateGuildAck );

        // 创建帮派失败
        if ( KFMsg::Success != kfmsg.code() )
        {
            auto guilddatasetting = _kf_guild_config->FindGuildSetting( KFGuildEnum::InitLevel );
            if ( nullptr == guilddatasetting )
            {
                // log error
                __LOG_ERROR__( "player[{}] create guild ack get guild config failed!", kfmsg.playerid() );
                return;
            }
            // 把玩家扣的钱补回去
            player->AddAgentData( &guilddatasetting->_cost_item, 1.0f, false, __FUNC_LINE__ );
            return _kf_display->SendToClient( player, kfmsg.code() );
        }
        else
        {
            __LOG_INFO__( "player[{}] create guild[{}] success!", kfmsg.playerid(), kfmsg.guildid() );

            player->RemoveData( __KF_STRING__( guildapply ) );

            /*
            auto kfobject = player->GetData();
            auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

            // 创建成功设置玩家的guildid
            player->UpdateData( kfbasic, __KF_STRING__( guildid ), KFUtility::ToString( kfmsg.guildid() ) );
            */
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HnadleInviteGuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgInviteGuildReq );

        //判断自身是否有帮派 // 目前等回调成功才设置邀请列表，会造成玩家多次邀请;
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );

        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildNotExist );
        }

        // 判断好友是否有帮派
        auto kffriend = kfobject->FindData( __KF_STRING__( friend ), kfmsg.inviterid() );
        if ( nullptr != kffriend )
        {
            auto guildid = kffriend->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );
            if ( _invalid_int != guildid )
            {
                return _kf_display->SendToClient( player, KFMsg::Playerisguildmember );
            }
        }
        else
        {
            __LOG_ERROR__( "player[{}] invite toplayer[{}] is not friend!", playerid, kfmsg.inviterid() );
            // TODO:这边限制只能邀请好友;后续如果不做限制，需要限定邀请列表长度
            return;
        }

        auto guildinviterdata = kfobject->FindData( __KF_STRING__( guildinvite ), kfmsg.inviterid() );
        if ( nullptr != guildinviterdata )
        {
            auto invitetime = guildinviterdata->GetValue<uint64>( __KF_STRING__( invitetime ) );
            // 判断是否已经邀请过该玩家
            if ( !KFDate::CheckPassTime( invitetime, KFTimeEnum::OneDaySecond ) )
            {
                return _kf_display->SendToClient( player, KFMsg::GuildHadInvite );
            }
        }
        KFMsg::S2SInviteGuildReq req;
        req.set_guildid( guildid );
        req.set_invitor( playerid );
        req.set_invitedid( kfmsg.inviterid() );
        auto kfgobal = KFGlobal::Instance();
        req.set_serverid( kfgobal->_app_id );
        SendMessageToGuild( guildid, KFMsg::S2S_INVITE_GUILD_REQ, &req );

    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HnadleInviteGuildAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SInviteGuildAck );

        auto kfobject = player->GetData();
        auto kffriend = kfobject->FindData( __KF_STRING__( friend ), kfmsg.invitedid() );
        if ( nullptr == kffriend )
        {
            __LOG_ERROR__( "player[{}] invite toplayer[{}] ack is not friend!", kfmsg.playerid(), kfmsg.invitedid() );
            return;
        }

        auto guildinviterdata = kfobject->FindData( __KF_STRING__( guildinvite ), kfmsg.invitedid() );
        auto kfgobal = KFGlobal::Instance();

        if ( nullptr != guildinviterdata )
        {
            player->UpdateData( guildinviterdata, __KF_STRING__( invitetime ), KFUtility::ToString( kfgobal->_real_time ) );
        }
        else
        {
            auto kfguildinvites = kfobject->FindData( __KF_STRING__( guildinvite ) );
            auto kfguildinvite = _kf_kernel->CreateObject( kfguildinvites->GetDataSetting() );
            kfguildinvite->SetKeyID( kfmsg.invitedid() );
            kfguildinvite->SetValue<uint64>( __KF_STRING__( invitetime ), kfgobal->_real_time );
            player->AddData( kfguildinvites, kfguildinvite );
        }
        // 发送邮件
        auto serverid = kffriend->GetValue<uint32>( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
        _kf_mail->SendMail( player, serverid, kfmsg.invitedid(), KFGuildEnum::InviteGuildMailId, KFUtility::ToString( kfmsg.guildid() ) );

    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleApplyGuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgApplyGuildReq );
        // 判断是否已经加入帮派
        if ( _invalid_int == kfmsg.guildid() )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );

        if ( _invalid_int != guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildHadExist );
        }

        // TODO:玩家申请加入帮派后,帮派刚好解散，这种情况暂不处理
        // 申请列表长度是否控制
        auto kfguildapply = kfobject->FindData( __KF_STRING__( guildapply ), kfmsg.guildid() );
        if ( nullptr != kfguildapply )
        {
            auto applytime = kfguildapply->GetValue<uint64>( __KF_STRING__( applytime ) );
            if ( !KFDate::CheckPassTime( applytime, KFTimeEnum::OneDaySecond ) )
            {
                return _kf_display->SendToClient( player, KFMsg::GuildHadApply );
            }
        }

        KFMsg::S2SApplyGuildReq req;
        req.set_guildid( kfmsg.guildid() );
        req.set_invitor( kfmsg.invitor() );
        req.set_playerid( playerid );
        bool ok = SendMessageToGuild( kfmsg.guildid(), KFMsg::S2S_APPLY_GUILD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}] apply guild[{}] failed!", playerid, guildid );
        }

    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleApplyGuildAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SApplyGuildAck );
        auto kfobject = player->GetData();
        auto kfguildapplyrecord = kfobject->FindData( __KF_STRING__( guildapply ) );
        if ( kfmsg.code() == KFMsg::Success )
        {
            // auto kfguildapplyrecord = kfobject->FindData( __KF_STRING__( guildapply ) );
            auto kfgobal = KFGlobal::Instance();
            player->UpdateData( kfguildapplyrecord, kfmsg.guildid(), __KF_STRING__( applytime ), KFOperateEnum::Set, kfgobal->_real_time );
        }
        else
        {
            _kf_display->SendToClient( player, kfmsg.code() );
        }

    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandlePlayerGuildChangeReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SPlayerGuildChangeReq );
        if ( KFMsg::JoinGuild == kfmsg.code() )
        {
            player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( guildid ), KFUtility::ToString( kfmsg.guildid() ) );
            __LOG_INFO__( "player[{}] join guild[{}] success!", kfmsg.playerid(), kfmsg.guildid() );
        }
        else if ( KFMsg::ExitGuild == kfmsg.code() )
        {
            player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( guildid ), _invalid_str );
            __LOG_INFO__( "player[{}] exit guild[{}] success!", kfmsg.playerid(), kfmsg.guildid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleExitGuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExitGuildReq );
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );

        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }

        KFMsg::S2SExitGuildReq req;
        req.set_guildid( guildid );
        req.set_playerid( playerid );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGuild( guildid, KFMsg::S2S_EXIT_GUILD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleTransferGuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTransferMasterReq );
        if ( kfmsg.newmasterid() == _invalid_int )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );

        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }

        KFMsg::S2STransferMasterReq req;
        req.set_guildid( guildid );
        req.set_playerid( playerid );
        req.set_newmasterid( kfmsg.newmasterid() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGuild( guildid, KFMsg::S2S_TRANSFER_MASTER_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleReviewApplyReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReviewApplyReq );
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );

        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }
        if ( kfmsg.operatortype() != KFMsg::AgreeApply && kfmsg.operatortype() != KFMsg::RefuseApply )
        {
            return;
        }

        KFMsg::S2SReviewApplyReq req;
        if ( kfmsg.has_playerid() )
        {
            req.set_dealplayerid( kfmsg.playerid() );
        }
        req.set_playerid( playerid );
        req.set_operatortype( kfmsg.operatortype() );
        req.set_guildid( guildid );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGuild( guildid, KFMsg::S2S_REVIEW_APPLY_REQ, &req );

    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleDissolveGuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDissolveGuildReq );
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );

        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }

        KFMsg::S2SDissolveGuildReq req;
        req.set_playerid( playerid );
        req.set_guildid( guildid );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGuild( guildid, KFMsg::S2S_DISSOLVE_GUILD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleModifyMedalReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgModifyGuildMedalReq );
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );

        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }

        auto kfguild = kfobject->FindData( __KF_STRING__( guild ) );
        if ( nullptr == kfguild )
        {
            return;
        }
        auto guildlevel = kfguild->GetValue<uint32 >( __KF_STRING__( level ) );

        auto guilddatasetting = _kf_guild_config->FindGuildSetting( guildlevel );
        if ( nullptr == guilddatasetting )
        {
            __LOG_ERROR__( "player[{}] modify guildlevel[{}] medal failed!", playerid, guildlevel );
            return;
        }

        if ( !guilddatasetting->IsValidMedal( kfmsg.newmedal() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildCreateIllegal );
        }

        KFMsg::S2SModifyMedalReq req;
        req.set_playerid( playerid );
        req.set_guildid( guildid );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGuild( guildid, KFMsg::S2S_MODIFY_MEDAL_REQ, &req );

    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleQueryGuildListReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryGuildListReq );

        KFMsg::S2SQueryGuildListReq req;
        req.set_playerid( playerid );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        if ( kfmsg.has_guilds() )
        {
            req.mutable_guilds()->CopyFrom( kfmsg.guilds() );
        }
        req.set_cursor( kfmsg.cursor() );


        SendMessageToGuild( KFMsg::S2S_QUERY_GUILD_LIST_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleKickMemberReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgKickMemberReq );
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );

        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }

        KFMsg::S2SKickMemberReq req;
        req.set_playerid( playerid );
        req.set_guildid( guildid );
        req.set_toplayerid( kfmsg.toplayerid() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGuild( guildid, KFMsg::S2S_KICK_MEMBER_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleUpgradeGuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpgradeGuildReq );
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );
        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }

        auto kfguild = kfobject->FindData( __KF_STRING__( guild ) );
        if ( nullptr == kfguild )
        {
            return;
        }
        auto guildlevel = kfguild->GetValue<uint32 >( __KF_STRING__( level ) );
        if ( guildlevel >= _kf_guild_config->_max_level )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildMaxLevel );
        }

        auto guildsetting = _kf_guild_config->FindGuildSetting( ++guildlevel );
        if ( nullptr == guildsetting )
        {
            return;
        }
        if ( !player->CheckAgentData( &guildsetting->_cost_item, __FUNC_LINE__ ) )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough );
        }

        KFMsg::S2SUpgradeGuildReq req;
        req.set_guildid( guildid );
        req.set_playerid( playerid );

        if ( SendMessageToGuild( guildid, KFMsg::S2S_UPGRADE_GUILD_REQ, &req ) )
        {
            player->RemoveAgentData( &guildsetting->_cost_item, __FUNC_LINE__ );
        }
        else
        {
            __LOG_ERROR__( "player[{}] upgrade guild[{}] level[{}] failed!", playerid, guildid, guildlevel );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleUpgradeGuildAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SUpgradeGuildAck );
        if ( kfmsg.code() != KFMsg::Success )
        {
            auto guildsetting = _kf_guild_config->FindGuildSetting( kfmsg.level() );
            if ( nullptr != guildsetting )
            {
                player->AddAgentData( &guildsetting->_cost_item, 1.0f, false, __FUNC_LINE__ );
            }
            return _kf_display->SendToClient( player, kfmsg.code() );
        }
        else
        {
            __LOG_INFO__( "player[{}] upgrade guild level[{}] success", player->GetKeyID(), kfmsg.level() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleAppointGuildMemberReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgAppointGuildMemberReq );

        auto kfobject = player->GetData();
        auto kfguild = kfobject->FindData( __KF_STRING__( guild ) );
        if ( nullptr == kfguild )
        {
            return;
        }

        auto guildid = kfguild->GetKeyID();
        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }

        if ( !_kf_guild_config->IsValidTitle( kfmsg.title() ) )
        {
            return;
        }

        // TODO:大厅服务先不判断丢给shard判断
        /*
        auto kfguildmember = kfguild->FindData( __KF_STRING__( guildmember ), playerid );
        if ( kfguildmember == nullptr )
        {
            __LOG_ERROR__( "player[{}] get guild[] member is null!", playerid, guildid );
            return;
        }*/

        KFMsg::S2SAppointGuildMemberReq req;
        req.set_guildid( guildid );
        req.set_playerid( playerid );
        req.set_toplayerid( kfmsg.toplayerid() );
        req.set_title( kfmsg.title() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGuild( guildid, KFMsg::S2S_APPOINT_GUILD_MEMBER_REQ, &req );

    }


    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleQueryGuildAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SLoginQueryGuildAck );

        auto kfobject = player->GetData();
        auto kfguild = kfobject->FindData( __KF_STRING__( guild ) );
        kfguild->SetKeyID( kfmsg.playerid() );
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        /*if ( nullptr != kfbasic )
        {
            kfbasic->SetValue<uint64>( __KF_STRING__( guildid ), kfmsg.guildid() );
        }*/

        // 格式化数据
        if ( !_kf_kernel->ParseFromProto( kfguild, &kfmsg.guilddata() ) )
        {
            __LOG_ERROR__( "player[{}] parse login query guild data failed!", kfmsg.playerid() );
            _kf_kernel->ReleaseObject( kfguild );
            return;
        }

        if ( kfmsg.showapplicant() )
        {
            // 玩家需要展示申请列表
            KFMsg::PBApplicationlists applylist;
            auto strapplylist = kfguild->GetValue<std::string>( __KF_STRING__( applicantlist ) );
            auto kfguildapplylists = kfguild->FindData( __KF_STRING__( guildapplylist ) );
            if ( nullptr == kfguildapplylists )
            {
                __LOG_ERROR__( "player[{}] query guild get applylist failed!", kfmsg.playerid() );
                return;
            }
            auto datasetting = kfguildapplylists->GetDataSetting();

            if ( GetGuildApplicantlist( strapplylist, applylist ) )
            {
                for ( auto i = 0; i < applylist.applylists_size(); ++i )
                {
                    auto applydata = &applylist.applylists( i );

                    auto kfdata = GetGuildApplicantFormProto( datasetting, applydata );
                    if ( nullptr != kfdata )
                    {
                        kfguild->AddData( __KF_STRING__( guildapplylist ), kfdata );
                    }
                }
            }
        }

        KFMsg::MsgTellQueryGuild sync;
        _kf_kernel->SerializeToClient( kfguild, sync.mutable_guild() );

        _kf_player->SendMessageToClient( kfmsg.playerid(), KFMsg::MSG_TELL_QUERY_GUILD, &sync );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleAddGuildAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::MsgSyncAddGuildData );
        //std::cout << "HandleAddGuildAck:" << kfmsg.DebugString() << std::endl;
        auto kfobject = player->GetData();
        if ( !kfmsg.pbdata().has_key() || !kfmsg.pbdata().has_name() )
        {
            return;
        }

        auto kfguild = kfobject->FindData( kfmsg.pbdata().name() );
        if ( kfguild == nullptr )
        {
            return;
        }

        ParseAddGuildFromProto( player, kfguild, nullptr, &kfmsg.pbdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleUpDateGuildAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::MsgSyncUpdateGuildData );
        //std::cout << "HandleUpDateGuildAck:" << kfmsg.DebugString() << std::endl;
        auto kfobject = player->GetData();
        if ( !kfmsg.pbdata().has_key() || !kfmsg.pbdata().has_name() )
        {
            __LOG_ERROR__( "player[{}] update guild data failed!", kfmsg.playerid() );
            return;
        }

        auto kfguild = kfobject->FindData( kfmsg.pbdata().name() );
        if ( kfguild == nullptr )
        {
            __LOG_ERROR__( "player[{}] update guild data field[{}] failed!", kfmsg.playerid(), kfmsg.pbdata().name() );
            return;
        }
        ParseUpdateGuildFromProto( player, kfguild, &kfmsg.pbdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleRemoveGuildAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::MsgSyncRemoveGuildData );
        //std::cout << "HandleRemoveGuildAck:" << kfmsg.DebugString() << std::endl;
        auto kfobject = player->GetData();
        if ( !kfmsg.pbdata().has_key() || !kfmsg.pbdata().has_name() )
        {
            __LOG_ERROR__( "player[{}] remove guild data failed!", kfmsg.playerid() );
            return;
        }

        auto kfguild = kfobject->FindData( kfmsg.pbdata().name() );
        if ( kfguild == nullptr )
        {
            __LOG_ERROR__( "player[{}] remove guild data field[{}] failed!", kfmsg.playerid(), kfmsg.pbdata().name() );
            return;
        }
        ParseRemoveGuildProto( player, kfguild, &kfmsg.pbdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleSearchGuildByNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSearchGuildByNameReq );
        if ( !IsValidName( kfmsg.guildname() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildNameInvalid );
        }

        if ( kfmsg.guildname().length() > KFGuildEnum::MaxNameLength )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildNameTooLong );
        }

        KFMsg::S2SSearchGuildByNameReq req;
        req.set_guildname( kfmsg.guildname() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        req.set_playerid( playerid );
        SendMessageToGuild( KFMsg::S2S_SEARCH_GUILD_BY_NAME_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleSetSwitchReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetGuildSwitchReq );
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );
        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }
        if ( !_kf_guild_config->IsVaildSwitch( kfmsg.type(), kfmsg.flag() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoPower );
        }
        KFMsg::S2SSetGuildSwitchReq req;
        req.set_playerid( playerid );
        req.set_guildid( guildid );
        req.set_type( kfmsg.type() );
        req.set_flag( kfmsg.flag() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGuild( guildid, KFMsg::S2S_SET_GUILD_SWITCH_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleLoginQueryGuildIdAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SLoginQueryGuildidAck );
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );
        if ( _invalid_int != kfmsg.guildid() )
        {
            OnEnterUpdateGuildData( player );

            KFMsg::S2SLoginQueryGuildReq req;
            req.set_guildid( kfmsg.guildid() );
            req.set_playerid( player->GetKeyID() );
            req.set_serverid( KFGlobal::Instance()->_app_id );
            if ( !SendMessageToGuild( kfmsg.guildid(), KFMsg::S2S_LOGIN_QUERY_GUILD_REQ, &req ) )
            {
                __LOG_ERROR__( "player[{}] login query guildid[{}] send message failed!", player->GetKeyID(), guildid );
            }
        }
        if ( guildid != kfmsg.guildid() )
        {
            auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
            player->UpdateData( kfbasic, __KF_STRING__( guildid ), KFUtility::ToString( kfmsg.guildid() ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandleQueryGuildLogReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryGuildLogReq );
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );
        if ( _invalid_int == guildid )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerNoGuild );
        }
        auto kfguild = kfobject->FindData( __KF_STRING__( guild ) );
        if ( nullptr == kfguild )
        {
            __LOG_ERROR__( "player[{}] query guild[{}] log failed!", playerid, guildid );
            return;
        }

        auto kfsetting = _kf_guild_config->FindGuildSetting( kfguild->GetValue<uint32>( __KF_STRING__( level ) ) );
        if ( nullptr == kfsetting )
        {
            return;
        }

        uint32 beginpos = kfmsg.page() * _kf_guild_config->_max_guild_log_page;
        if ( beginpos >= kfsetting->_max_log )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildLogOutPage );
        }

        // 显示声明
        int endpos = beginpos + _kf_guild_config->_max_guild_log_page - 1;
        if ( endpos < _invalid_int || endpos < beginpos )
        {
            return;
        }

        KFMsg::S2SQueryGuildLogReq req;
        req.set_playerid( playerid );
        req.set_guildid( guildid );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        req.set_beginpos( beginpos );
        req.set_endpos( endpos );
        req.set_page( kfmsg.page() );
        SendMessageToGuild( guildid, KFMsg::S2S_QUERY_GUILD_LOG_REQ, &req );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFGuildClientModule::OnGuildApplicantUpdateCallBack )
    {
        auto kfobject = player->GetData();
        auto kfguild = kfobject->FindData( __KF_STRING__( guild ) );
        if ( nullptr == kfguild )
        {
            return;
        }
        auto kfguildmember = kfguild->FindData( __KF_STRING__( guildmember ), player->GetKeyID() );
        if ( nullptr == kfguildmember )
        {
            return;
        }

        auto title = kfguildmember->GetValue<uint32>( __KF_STRING__( title ) );
        if ( !_kf_guild_config->IsOwnPower( title, KFGuildEnum::Mask_Review_Data ) )
        {
            return;
        }
        auto kfguildapplylists = kfguild->FindData( __KF_STRING__( guildapplylist ) );
        auto datasetting = kfguildapplylists->GetDataSetting();
        player->RemoveData( __KF_STRING__( guild ), __KF_STRING__( guildapplylist ) );

        KFMsg::PBApplicationlists applylist;
        if ( GetGuildApplicantlist( value, applylist ) )
        {
            for ( auto i = 0; i < applylist.applylists_size(); ++i )
            {
                auto applydata = &applylist.applylists( i );
                auto kfdata = GetGuildApplicantFormProto( datasetting, applydata );
                if ( nullptr != kfdata )
                {
                    player->AddData( kfguildapplylists, kfdata );
                }
            }
        }
    }

    bool KFGuildClientModule::GetGuildApplicantlist( const std::string& strapplicantlist, KFMsg::PBApplicationlists& applylist )
    {
        applylist.Clear();
        if ( !strapplicantlist.empty() )
        {
            return KFProto::Parse( &applylist, strapplicantlist, KFCompressEnum::Compress );

        }
        return false;
    }

    KFData* KFGuildClientModule::GetGuildApplicantFormProto( const KFDataSetting* datasetting, const KFMsg::PBApplicationlist* applydata )
    {
        auto kfguildapplylist = _kf_kernel->CreateObject( datasetting );
        if ( nullptr == kfguildapplylist  || nullptr == applydata )
        {
            return nullptr;
        }

        kfguildapplylist->SetKeyID( applydata->playerid() );
        kfguildapplylist->SetValue<uint64>( __KF_STRING__( time ), applydata->time() );

        auto basic = &applydata->basic();
        if ( nullptr == basic )
        {
            return kfguildapplylist;
        }

        auto kfbasic = kfguildapplylist->FindData( __KF_STRING__( basic ) );
        if ( nullptr == kfbasic )
        {
            return kfguildapplylist;
        }

        for ( auto j = 0; j < basic->pbstring_size(); ++j )
        {
            auto pbstring = &basic->pbstring( j );
            auto kfdata = kfbasic->FindData( pbstring->name() );
            if ( kfdata != nullptr )
            {
                kfdata->SetValue< std::string >( pbstring->value() );
            }
        }
        return kfguildapplylist;

    }



    void KFGuildClientModule::ParseUpdateGuildFromProto( KFEntity* player, KFData* kfparent, const KFMsg::PBObject* proto )
    {
        if ( nullptr == player || nullptr == kfparent )
        {
            return;
        }

        for ( auto i = 0; i < proto->pbint32_size(); ++i )
        {
            auto pbint32 = &proto->pbint32( i );
            player->UpdateData( kfparent, pbint32->name(), __TO_STRING__( pbint32->value() ) );
        }

        for ( auto i = 0; i < proto->pbuint32_size(); ++i )
        {
            auto pbuint32 = &proto->pbuint32( i );
            player->UpdateData( kfparent, pbuint32->name(), __TO_STRING__( pbuint32->value() ) );
        }

        // kfint64
        for ( auto i = 0; i < proto->pbint64_size(); ++i )
        {
            auto pbint64 = &proto->pbint64( i );
            player->UpdateData( kfparent, pbint64->name(), __TO_STRING__( pbint64->value() ) );
        }

        // kfuint64
        for ( auto i = 0; i < proto->pbuint64_size(); ++i )
        {
            auto pbuint64 = &proto->pbuint64( i );
            player->UpdateData( kfparent, pbuint64->name(), __TO_STRING__( pbuint64->value() ) );
        }

        // kfdouble
        for ( auto i = 0; i < proto->pbdouble_size(); ++i )
        {
            auto pbdouble = &proto->pbdouble( i );
            player->UpdateData( kfparent, pbdouble->name(), __TO_STRING__( pbdouble->value() ) );
        }

        // kfstring
        for ( auto i = 0; i < proto->pbstring_size(); ++i )
        {
            auto pbstring = &proto->pbstring( i );
            player->UpdateData( kfparent, pbstring->name(), pbstring->value() );
        }

        // kfobject
        for ( auto i = 0; i < proto->pbobject_size(); ++i )
        {
            auto pbobject = &proto->pbobject( i );
            auto kfobject = kfparent->FindData( pbobject->name(), pbobject->key() );
            ParseUpdateGuildFromProto( player, kfobject, pbobject );
        }

        for ( auto i = 0; i < proto->pbrecord_size(); ++i )
        {
            auto pbrecord = &proto->pbrecord( i );
            for ( auto j = 0; j < pbrecord->pbobject_size(); ++j )
            {
                auto pbobject = &pbrecord->pbobject( j );
                auto kfobject = kfparent->FindData( pbobject->name(), pbobject->key() );
                ParseUpdateGuildFromProto( player, kfobject, pbobject );
            }

        }

    }

    void KFGuildClientModule::ParseAddGuildFromProto( KFEntity* player, KFData* kfparent, KFData* kfdata, const KFMsg::PBObject* proto )
    {
        if ( nullptr == kfparent || nullptr == player  )
        {
            return;
        }
        if ( nullptr != kfdata && nullptr != proto )
        {
            _kf_kernel->ParseFromProto( kfdata, proto );
            player->AddData( kfparent, kfdata );
        }

        /*
        for ( auto i = 0 ; i < proto->pbobject_size(); ++i )
        {
            auto pbobject = &proto->pbobject( i );
            if ( nullptr == kfdata )
            {
                continue;
            }
            auto kfobject = kfdata->FindData( pbobject->name() );
            if ( nullptr == kfobject )
            {
                continue;
            }

            _kf_kernel->ParseFromProto( kfdata, pbobject );
            player->AddData( kfparent, kfdata );
        }*/

        for ( auto i = 0; i < proto->pbrecord_size(); ++i )
        {
            auto pbrecord = &proto->pbrecord( i );
            for ( auto j = 0; j < pbrecord->pbobject_size(); ++j )
            {
                auto pbobject = &pbrecord->pbobject( j );
                auto kfobjectdata = kfparent->FindData( pbobject->name(), pbobject->key() );
                // 已经存在不应该在add
                if ( kfobjectdata != nullptr )
                {
                    continue;
                }
                auto kfobject = kfparent->FindData( pbobject->name() );
                if ( nullptr == kfobject )
                {
                    continue;
                }

                auto kfchild = _kf_kernel->CreateObject( kfobject->GetDataSetting() );

                if ( nullptr == kfchild )
                {
                    continue;
                }

                ParseAddGuildFromProto( player, kfobject, kfchild, pbobject );
            }
        }
    }

    void KFGuildClientModule::ParseRemoveGuildProto( KFEntity* player, KFData* kfparent, const KFMsg::PBObject* proto )
    {
        if ( nullptr == kfparent || nullptr == player )
        {
            return;
        }

        for ( auto i = 0; i < proto->pbobject_size(); ++i )
        {
            auto pbobject = &proto->pbobject( i );
            player->RemoveData( kfparent, pbobject->key() );
        }

        for ( auto i = 0; i < proto->pbrecord_size(); ++i )
        {
            auto pbrecord = &proto->pbrecord( i );
            for ( auto j = 0; j < pbrecord->pbobject_size(); ++j )
            {
                auto pbobject = &pbrecord->pbobject( j );
                auto kfobject = kfparent->FindData( pbobject->name() );
                if ( nullptr == kfobject )
                {
                    continue;
                }
                player->RemoveData( kfobject, pbobject->key() );
            }
        }
    }

    void KFGuildClientModule::CheckStartApplicantlistTimer( KFEntity* player )
    {
        if ( nullptr == player )
        {
            return;
        }
        auto kfobject = player->GetData();
        auto guildid = kfobject->GetValue<uint64>( __KF_STRING__( basic ), __KF_STRING__( guildid ) );
        if ( _invalid_int != guildid )
        {
            return;
        }
        auto kfguildapplyrecord = kfobject->FindData( __KF_STRING__( guildapply ) );
        auto kfguildapply = kfguildapplyrecord->FirstData();
        uint32 _min_valid_time = 0xFFFFFFFF;
        auto keeptime = _kf_guild_config->GetKeepTime();
        while ( kfguildapply != nullptr )
        {
            auto createtime = kfguildapply->GetValue( __KF_STRING__( time ) );
            auto validtime = createtime + keeptime * KFTimeEnum::OneHourMicSecond;
            if ( validtime != 0 && validtime < _min_valid_time )
            {
                _min_valid_time = validtime;
            }
            kfguildapply = kfguildapplyrecord->NextData();
        }

        auto kfguildinviterecord = kfobject->FindData( __KF_STRING__( guildinvite ) );
        auto kfguildinvite = kfguildinviterecord->FirstData();
        while ( kfguildinvite != nullptr )
        {
            auto createtime = kfguildinvite->GetValue( __KF_STRING__( time ) );
            auto validtime = createtime + keeptime * KFTimeEnum::OneHourMicSecond;
            if ( validtime != 0 && validtime < _min_valid_time )
            {
                _min_valid_time = validtime;
            }
            kfguildinvite = kfguildinviterecord->NextData();
        }

        if ( _min_valid_time == 0xFFFFFFFF )
        {
            return;
        }

        if ( _min_valid_time < KFGlobal::Instance()->_real_time )
        {
            _min_valid_time = KFGlobal::Instance()->_real_time;
        }
        auto intervaltime = ( _min_valid_time - KFGlobal::Instance()->_real_time + 1 ) * 1000;

        __REGISTER_LIMIT_TIMER__( player->GetKeyID(), intervaltime, 1, &KFGuildClientModule::OnTimerCheckApplicantlistValidTime );
    }

    __KF_TIMER_FUNCTION__( KFGuildClientModule::OnTimerCheckApplicantlistValidTime )
    {
        auto player = _kf_player->FindPlayer( objectid, __FUNC_LINE__ );
        if ( player == nullptr )
        {
            return;
        }

        // 删除过期的邀请列表
        RemoveVaildApplicantList( player );
        // 删除过期的申请列表
        RemoveVaildInviteList( player );

        CheckStartApplicantlistTimer( player );
    }

    void KFGuildClientModule::RemoveVaildApplicantList( KFEntity* player )
    {
        auto kfobject = player->GetData();

        auto kfguildapplyrecord = kfobject->FindData( __KF_STRING__( guildapply ) );
        if ( kfguildapplyrecord == nullptr )
        {
            return;
        }

        std::list< KFData*> vaildapplylist;
        auto keeptime = _kf_guild_config->GetKeepTime();
        auto kfguildapply = kfguildapplyrecord->FirstData();
        while ( kfguildapply != nullptr )
        {
            auto createtime = kfguildapply->GetValue( __KF_STRING__( time ) );
            auto validtime = createtime + keeptime * KFTimeEnum::OneHourMicSecond;
            if ( validtime < KFGlobal::Instance()->_real_time )
            {
                vaildapplylist.push_back( kfguildapply );
            }
            kfguildapply = kfguildapplyrecord->NextData();
        }

        for ( auto& iter : vaildapplylist )
        {
            player->RemoveData( kfguildapplyrecord, kfguildapply->GetKeyID() );
        }

    }

    void KFGuildClientModule::RemoveVaildInviteList( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfguildinviterecord = kfobject->FindData( __KF_STRING__( guildinvite ) );
        if ( kfguildinviterecord == nullptr )
        {
            return;
        }

        std::list< KFData*> vaildapplylist;
        auto keeptime = _kf_guild_config->GetKeepTime();
        auto kfguildinvite = kfguildinviterecord->FirstData();
        while ( kfguildinvite != nullptr )
        {
            auto createtime = kfguildinvite->GetValue( __KF_STRING__( time ) );
            auto validtime = createtime + keeptime * KFTimeEnum::OneHourMicSecond;
            if ( validtime < KFGlobal::Instance()->_real_time )
            {
                vaildapplylist.push_back( kfguildinvite );
            }
            kfguildinvite = kfguildinviterecord->NextData();
        }

        for ( auto& iter : vaildapplylist )
        {
            player->RemoveData( kfguildinviterecord, kfguildinvite->GetKeyID() );
        }
    }

    bool KFGuildClientModule::IsValidName( const std::string& name )
    {
        if ( name.empty() )
        {
            return false;
        }
        if ( name.find( ' ' ) != std::string::npos || name.find( '%' ) != std::string::npos )
        {
            return false;
        }
        return true;
    }


}