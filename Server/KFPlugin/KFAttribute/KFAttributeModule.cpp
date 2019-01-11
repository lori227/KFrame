#include "KFAttributeModule.h"

namespace KFrame
{
    void KFAttributeModule::BeforeRun()
    {
        // 注册Debug函数
        __REGISTER_DEBUG_FUNCTION__( __KF_STRING__( adddata ), &KFAttributeModule::DebugAddData );
        __REGISTER_DEBUG_FUNCTION__( __KF_STRING__( setdata ), &KFAttributeModule::DebugSetData );
        __REGISTER_DEBUG_FUNCTION__( __KF_STRING__( decdata ), &KFAttributeModule::DebugDecData );

        _kf_player->BindAfterQueryFunction( this, &KFAttributeModule::OnAfterQueryPlayerData );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ, &KFAttributeModule::HandleQueryPlayerReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_SEX_REQ, &KFAttributeModule::HandleChangeSexReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_NAME_REQ, &KFAttributeModule::HandleChangeNameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CREATE_ROLE_REQ, &KFAttributeModule::HandleCreateRoleReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROLE_ACK, &KFAttributeModule::HandleCreateRoleAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_ACK, &KFAttributeModule::HandleSetPlayerNameAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_REQ, &KFAttributeModule::HandleChangeIconReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ, &KFAttributeModule::HandleRemoveDataReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_BOX_REQ, &KFAttributeModule::HandleChangeIconBoxReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_MOTTO_REQ, &KFAttributeModule::HandleChangeMottoReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUEST_REQ, &KFAttributeModule::HandleQueryGuestReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUEST_ACK, &KFAttributeModule::HandleQueryGuestAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_SETTING_REQ, &KFAttributeModule::HandleQuerySettingReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_SETTING_REQ, &KFAttributeModule::HandleUpdateSettingReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TITLE_CHANGE_REQ, &KFAttributeModule::HandleTitleChangeReq );
    }

    void KFAttributeModule::BeforeShut()
    {

        // 取消注册debug函数
        __UNREGISTER_DEBUG_FUNCTION__( __KF_STRING__( adddata ) );
        __UNREGISTER_DEBUG_FUNCTION__( __KF_STRING__( setdata ) );
        __UNREGISTER_DEBUG_FUNCTION__( __KF_STRING__( decdata ) );

        _kf_player->UnBindAfterQueryFunction( this );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_SEX_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_NAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CREATE_ROLE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROLE_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_BOX_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_MOTTO_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUEST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUEST_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_SETTING_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_UPDATE_SETTING_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_TITLE_CHANGE_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleQueryPlayerReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryPlayerReq );

        // 不能查询自己的数据，客户端本地可以获取到
        if ( playerid == kfmsg.playerid() )
        {
            return;
        }

        //查询玩家数据
        KFMsg::S2SQueryPlayerReq req;
        req.set_playerid( playerid );
        _kf_player->QueryPlayer( playerid, kfmsg.playerid() );
    }

    void KFAttributeModule::OnAfterQueryPlayerData( uint32 result, uint64 playerid, KFMsg::PBObject* pbplayerdata )
    {
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( playerid, result );
        }

        KFMsg::MsgQueryPlayerAck ack;
        ack.mutable_player()->CopyFrom( *pbplayerdata );
        _kf_game->SendToClient( player, KFMsg::MSG_QUERY_PLAYER_ACK, &ack );

        // 更新访客信息
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleCreateRoleReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCreateRoleReq );

        if ( kfmsg.name().empty() || kfmsg.model() == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::NameEmpty );
        }

        // 如果名字已经存在了, 说明已经创建过角色
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        auto modleid = kfbasic->GetValue( __KF_STRING__( model ) );
        if ( modleid != _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::CreateRoleAlready );
        }

        // 检查名字的有效性
        if ( !CheckNameValid( kfmsg.name() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::InvalidFilter );
        }

        KFMsg::S2SCreateRoleReq req;
        req.set_playerid( playerid );
        req.set_newname( kfmsg.name() );
        req.set_modleid( kfmsg.model() );
        req.set_sex( kfmsg.sex() );
        req.set_inviterid( kfmsg.inviterid() );
        auto ok = _kf_public->SendMessageToPublic( KFMsg::S2S_CREATE_ROLE_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] create role to public failed!", playerid, kfmsg.name() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleCreateRoleAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SCreateRoleAck );

        if ( kfmsg.result() == KFMsg::Ok )
        {
            kfmsg.set_result( KFMsg::CreateRoleOK );

            // 设置模型
            player->UpdateData( __KF_STRING__( modelid ), KFOperateEnum::Set, kfmsg.modleid() );
            player->UpdateData( __KF_STRING__( model ), kfmsg.modleid(), __KF_STRING__( id ), KFOperateEnum::Set, kfmsg.modleid() );

            // 名字
            player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( name ), kfmsg.newname() );
            // 性别
            player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( sex ), KFOperateEnum::Set, kfmsg.sex() );
            // 保存邀请人
            player->UpdateData( __KF_STRING__( inviterid ), KFOperateEnum::Set, kfmsg.inviterid() );

            // 保存玩家
            _kf_player->SavePlayer( player );
        }

        _kf_display->SendToClient( player, kfmsg.result(), kfmsg.newname() );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleChangeSexReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeSexReq );

        _kf_display->SendToClient( player, KFMsg::SexSetOK );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( sex ), KFOperateEnum::Set, kfmsg.sex() );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleChangeNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeNameReq );

        if ( kfmsg.name().empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::NameEmpty );
        }

        auto kfobject = player->GetData();
        auto name = kfobject->GetValue<std::string>( __KF_STRING__( basic ), __KF_STRING__( name ) );
        if ( !name.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::NameAlreadySet );
        }

        // 检查名字的有效性
        // 检查名字的有效性
        if ( !CheckNameValid( kfmsg.name() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::InvalidFilter );
        }

        KFMsg::S2SSetPlayerNameReq req;
        req.set_playerid( playerid );
        req.set_oldname( name );
        req.set_newname( kfmsg.name() );
        auto ok = _kf_public->SendMessageToPublic( KFMsg::S2S_SET_PLAYER_NAME_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}:{}] set name to public failed!", playerid, name, kfmsg.name() );
        }
    }

    bool KFAttributeModule::CheckNameValid( const std::string& name )
    {
        // 不能有空格
        if ( name.find( " " ) != std::string::npos )
        {
            return false;
        }

        // 不能有%
        if ( name.find( "%" ) != std::string::npos )
        {
            return false;
        }

        return !_kf_filter->CheckFilter( name );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleSetPlayerNameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SSetPlayerNameAck );

        if ( kfmsg.result() == KFMsg::Ok )
        {
            kfmsg.set_result( KFMsg::NameSetOK );
            player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( name ), kfmsg.newname() );
        }

        _kf_display->SendToClient( player, kfmsg.result(), kfmsg.newname() );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleChangeIconReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeIconReq );

        _kf_display->SendToClient( player, KFMsg::ChangeIconOK );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( icon ), kfmsg.icon() );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleChangeMottoReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeMottoReq );

        auto filter = _kf_filter->CheckFilter( kfmsg.motto() );
        if ( filter )
        {
            return _kf_display->SendToClient( player, KFMsg::InvalidFilter );
        }

        _kf_display->SendToClient( player, KFMsg::ChangeMottoOK );
        player->UpdateData( __KF_STRING__( motto ), kfmsg.motto() );
    }


    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleRemoveDataReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveDataReq );

        player->UpdateData( kfmsg.dataname(), kfmsg.key(), __KF_STRING__( count ), KFOperateEnum::Dec, kfmsg.count() );

        __LOG_INFO__( "remove data[{}:{}:{}] ok!", kfmsg.dataname(), kfmsg.key(), kfmsg.count() );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleChangeIconBoxReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeIconBoxReq );

        _kf_display->SendToClient( player, KFMsg::ChangeIconBoxOK );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( iconbox ), kfmsg.iconbox() );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleQueryGuestReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryGuestReq );

        //查询访客信息
        KFMsg::S2SQueryGuestReq req;
        req.set_playerid( playerid );
        req.set_queryid( kfmsg.playerid() );
        req.set_querytime( KFGlobal::Instance()->_real_time );
        _kf_public->SendMessageToPublic( KFMsg::S2S_QUERY_GUEST_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleQueryGuestAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryGuestAck );

        KFMsg::MsgQueryGuestAck ack;
        ack.set_playerid( kfmsg.queryid() );
        ack.set_guestcount( kfmsg.guestcount() );
        ack.mutable_guestdata()->CopyFrom( kfmsg.guestdata() );
        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_GUEST_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleQuerySettingReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQuerySettingReq );

        auto kfobject = player->GetData();
        auto kfsetting = kfobject->FindData( __KF_STRING__( setting ) );

        KFMsg::MsgQuerySettingAck ack;
        _kf_kernel->SerializeToData( kfsetting, ack.mutable_pbsetting() );
        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_SETTING_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleUpdateSettingReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateSettingReq );

        if ( kfmsg.settingvalue().size() >= KFBufferEnum::Buff_1M )
        {
            return;
        }

        auto kfobject = player->GetData();
        auto kfsetting = kfobject->FindData( __KF_STRING__( setting ) );
        player->UpdateData( kfsetting, kfmsg.settingkey(), kfmsg.settingvalue() );
    }

    __KF_DEBUG_FUNCTION__( KFAttributeModule::DebugAddData )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        auto& strdata = params[ 0 ];

        KFElements kfelements;
        auto ok = kfelements.Parse( strdata, __FUNC_LINE__ );
        if ( ok )
        {
            player->AddElement( &kfelements, true, __FUNC_LINE__ );
        }
    }

    __KF_DEBUG_FUNCTION__( KFAttributeModule::DebugSetData )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        auto& strdata = params[ 0 ];

        KFElements kfelements;
        auto ok = kfelements.Parse( strdata, __FUNC_LINE__ );
        if ( ok )
        {
            kfelements.SetOperate( KFOperateEnum::Set );
            player->AddElement( &kfelements, true, __FUNC_LINE__ );
        }
    }

    __KF_DEBUG_FUNCTION__( KFAttributeModule::DebugDecData )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        auto& stragent = params[ 0 ];

        KFElements kfelements;
        auto ok = kfelements.Parse( stragent, __FUNC_LINE__ );
        if ( ok )
        {
            player->RemoveElement( &kfelements, __FUNC_LINE__ );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleTitleChangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTitleChangeReq );

        auto kfobject = player->GetData();

        auto kftitle = kfobject->FindData( __KF_STRING__( title ), kfmsg.titleid() );
        if ( kftitle == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TitleNotExist );
        }

        player->UpdateData( __KF_STRING__( titleid ), KFOperateEnum::Set, kfmsg.titleid() );
    }

}