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
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterUpdateDataModule( this, &KFGuildClientModule::OnUpdateDataCallBack );
        _kf_component->RegisterUpdateStringModule( this, &KFGuildClientModule::OnUpdateStringCallBack );

        __REGISTER_MESSAGE__( KFMsg::MSG_CREATE_GUILD_REQ, &KFGuildClientModule::HandleCreateGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_ACK, &KFGuildClientModule::HandleCreateGuildAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_INVITE_GUILD_REQ, &KFGuildClientModule::HnadleInviteGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_INVITE_GUILD_ACK, &KFGuildClientModule::HnadleInviteGuildAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_APPLY_GUILD_REQ, &KFGuildClientModule::HandleApplyGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_GUILD_CHANGE_REQ, &KFGuildClientModule::HandlePlayerGuildChangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXIT_GUILD_REQ, &KFGuildClientModule::HandleExitGuildReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRANSFER_MASTER_REQ, &KFGuildClientModule::HandleTransferGuildReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REVIEW_APPLY_REQ, &KFGuildClientModule::HandleReviewApplyReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_DISSOLVE_GUILD_REQ, &KFGuildClientModule::HandleDissolveGuildReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MODIFY_GUILD_MEDAL_REQ, &KFGuildClientModule::HandleModifyMedalReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUILD_LIST_REQ, &KFGuildClientModule::HandleQueryGuildListReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_KICK_MEMBER_REQ, &KFGuildClientModule::HandleKickMemberReq );
    }

    void KFGuildClientModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CREATE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_INVITE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_INVITE_GUILD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_APPLY_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_GUILD_CHANGE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_EXIT_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_TRANSFER_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REVIEW_APPLY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_DISSOLVE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_MODIFY_GUILD_MEDAL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUILD_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_KICK_MEMBER_REQ );
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
            // log error
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

        if ( kfmsg.has_manifesto() && kfmsg.manifesto().length() > KFGuildEnum::MaxManifesto )
        {
            return _kf_display->SendToClient( player, KFMsg::GuildManifestoTooLong );
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
        player->RemoveAgentData( &guilddatasetting->_cost_item, __FUNC_LINE__ );
        auto name = kfobject->GetValue<std::string>( __KF_STRING__( basic ), __KF_STRING__( name ) );
        KFMsg::S2SCreateGuidReq req;
        req.set_playerid( playerid );
        req.set_name( name );
        req.set_guildname( kfmsg.guildname() );
        req.set_medal( kfmsg.medal() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        auto ok = SendMessageToGuild( KFMsg::S2S_CREATE_GUILD_REQ, &req );

        if ( !ok )
        {
            // log error
            player->AddAgentData( &guilddatasetting->_cost_item, 1.0f, false, __FUNC_LINE__ );
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
                return;
            }
            // 把玩家扣的钱补回去
            player->AddAgentData( &guilddatasetting->_cost_item, 1.0f, false, __FUNC_LINE__ );
            return _kf_display->SendToClient( player, kfmsg.code() );
        }
        else
        {
            auto kfobject = player->GetData();
            auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

            // 创建成功设置玩家的guildid
            player->UpdateData( kfbasic, __KF_STRING__( guildid ), KFUtility::ToString( kfmsg.guildid() ) );

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
        _kf_mail->SendMail( player, serverid, kfmsg.invitedid(), 6, KFUtility::ToString( kfmsg.guildid() ) );

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
        if ( ok )
        {
            auto kfguildapplyrecord = kfobject->FindData( __KF_STRING__( guildapply ) );
            auto kfgobal = KFGlobal::Instance();
            player->UpdateData( kfguildapplyrecord, kfmsg.guildid(), __KF_STRING__( applytime ), KFOperateEnum::Set, kfgobal->_real_time );
        }

    }

    __KF_MESSAGE_FUNCTION__( KFGuildClientModule::HandlePlayerGuildChangeReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SPlayerGuildChangeReq );
        if ( KFMsg::JoinGuild == kfmsg.code() )
        {
            player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( guildid ), KFUtility::ToString( kfmsg.guildid() ) );
        }
        else if ( KFMsg::ExitGuild == kfmsg.code() )
        {
            player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( guildid ), _invalid_str );
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

        // 取最高等级所能设置的徽章
        auto guilddatasetting = _kf_guild_config->FindGuildSetting( KFGuildEnum::MaxLevel );
        if ( nullptr == guilddatasetting )
        {
            // log error
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

}