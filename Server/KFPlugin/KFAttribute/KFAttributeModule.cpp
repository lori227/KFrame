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
        _kf_player->BindAfterSetNameFunction( this, &KFAttributeModule::OnAfterSetPlayerName );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ, &KFAttributeModule::HandleQueryPlayerReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_NAME_REQ, &KFAttributeModule::HandleSetNameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_SEX_REQ, &KFAttributeModule::HandleSetSexReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_REQ, &KFAttributeModule::HandleChangeIconReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ, &KFAttributeModule::HandleRemoveDataReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_BOX_REQ, &KFAttributeModule::HandleChangeIconBoxReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_MOTTO_REQ, &KFAttributeModule::HandleChangeMottoReq );
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
        _kf_player->UnBindAfterSetNameFunction( this );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_NAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_SEX_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_BOX_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_MOTTO_REQ );
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
        auto ok = _kf_player->QueryPlayer( playerid, kfmsg.playerid() );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RouteServerBusy );
        }
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
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFAttributeModule::CheckNameValid( const std::string& name )
    {
        static auto* _option = _kf_option->FindOption( __KF_STRING__( playernamelength ) );
        if ( name.size() > _option->_uint32_value )
        {
            return KFMsg::NameLengthError;
        }

        auto ok = _kf_filter->CheckFilter( name );
        if ( ok )
        {
            return KFMsg::NameFilterError;
        }

        return KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleSetNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetNameReq );

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
        auto result = CheckNameValid( kfmsg.name() );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }

        // 修改名字
        auto ok = _kf_player->SetName( playerid, name, kfmsg.name(), _invalid_int );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RouteServerBusy );
        }
    }

    void KFAttributeModule::OnAfterSetPlayerName( uint32 result, KFEntity* player, const std::string& name, uint64 itemguid )
    {
        _kf_display->SendToClient( player, result, name );
        if ( result != KFMsg::NameSetOK )
        {
            return;
        }

        if ( itemguid != _invalid_int )
        {
            // 删除改名卡
            player->RemoveData( __KF_STRING__( item ), itemguid );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleSetSexReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeSexReq );

        _kf_display->SendToClient( player, KFMsg::SexSetOK );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( sex ), KFOperateEnum::Set, kfmsg.sex() );
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
            //return _kf_display->SendToClient( player, KFMsg::InvalidFilter );
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

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleQuerySettingReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQuerySettingReq );

        auto kfobject = player->GetData();
        auto kfsetting = kfobject->FindData( __KF_STRING__( setting ) );

        KFMsg::MsgQuerySettingAck ack;
        _kf_kernel->SerializeToData( kfsetting, ack.mutable_pbsetting() );
        _kf_game->SendToClient( player, KFMsg::MSG_QUERY_SETTING_ACK, &ack );
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
            player->AddElement( __FUNC_LINE__, &kfelements, true );
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
            player->AddElement( __FUNC_LINE__, &kfelements, true );
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
            player->RemoveElement( __FUNC_LINE__, &kfelements );
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