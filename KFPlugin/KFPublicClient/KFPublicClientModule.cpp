#include "KFPublicClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFPublicClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA__( &KFPublicClientModule::OnUpdateDataCallBack );
        __REGISTER_UPDATE_STRING__( &KFPublicClientModule::OnUpdateStringCallBack );

        __REGISTER_ENTER_PLAYER__( &KFPublicClientModule::OnEnterUpdatePublicData );
        __REGISTER_LEAVE_PLAYER__( &KFPublicClientModule::OnLeaveUpdatePublicData );

        _kf_route->RegisterConnectionFunction( this, &KFPublicClientModule::OnRouteConnectCluster );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_SEX_REQ, &KFPublicClientModule::HandleSetSexReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_NAME_REQ, &KFPublicClientModule::HandleSetNameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYERNAME_TO_GAME_ACK, &KFPublicClientModule::HandleSetPlayerNameToGameAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_BASIC_REQ, &KFPublicClientModule::HandleQueryBasicReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_BASIC_TO_GAME_ACK, &KFPublicClientModule::HandleQueryBasicToGameAck );
    }

    void KFPublicClientModule::BeforeShut()
    {
        __UN_UPDATE_DATA__();
        __UN_UPDATE_STRING__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();

        _kf_route->UnRegisterConnectionFunction( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SET_NAME_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_SEX_REQ );
        __UN_MESSAGE__( KFMsg::S2S_SET_PLAYERNAME_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::MSG_QUERY_BASIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_BASIC_TO_GAME_ACK );
    }

    void KFPublicClientModule::OnRouteConnectCluster( uint64 serverid )
    {
        if ( !_need_refresh_online )
        {
            return;
        }

        _need_refresh_online = false;

        // 刷新在线逻辑
        KFMsg::S2SClearOnlineToPublicReq req;
        req.set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_route->RepeatToRand( __STRING__( logic ), KFMsg::S2S_CLEAR_ONLINE_TO_PUBLIC_REQ, &req );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPublicClientModule::UpdatePublicData( KFEntity* player, StringMap& values )
    {
        UpdatePublicData( player->GetKeyID(), values );
    }

    void KFPublicClientModule::UpdatePublicData( uint64 playerid, StringMap& values )
    {
        KFMsg::S2SUpdateDataToPublicReq req;
        req.mutable_pbdata()->insert( values.begin(), values.end() );
        _kf_route->RepeatToRand( playerid, __STRING__( logic ), KFMsg::S2S_UPDATE_DATA_TO_PUBLIC_REQ, &req );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPublicClientModule::OnUpdateDataToPublic( KFEntity* player, KFData* kfdata )
    {
        // 只有公共属性才更新
        if ( !player->IsInited() ||
                !kfdata->HaveMask( KFDataDefine::Mask_Public ) ||
                !kfdata->GetParent()->HaveMask( KFDataDefine::Mask_Public ) )
        {
            return;
        }

        // 更新到公共属性集群
        StringMap values;
        values[ kfdata->_data_setting->_name ] = kfdata->ToString();
        UpdatePublicData( player, values );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFPublicClientModule::OnUpdateDataCallBack )
    {
        OnUpdateDataToPublic( player, kfdata );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFPublicClientModule::OnUpdateStringCallBack )
    {
        OnUpdateDataToPublic( player, kfdata );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFPublicClientModule::OnEnterUpdatePublicData )
    {
        auto kfglobal = KFGlobal::Instance();

        StringMap values;
        values[ __STRING__( id ) ] = __TO_STRING__( player->GetKeyID() );
        values[ __STRING__( status ) ] = __TO_STRING__( KFMsg::FrameOnlineStatus );
        values[ __STRING__( statustime ) ] = __TO_STRING__( kfglobal->_real_time );
        values[ __STRING__( serverid ) ] = __TO_STRING__( kfglobal->_app_id->GetId() );
        UpdatePublicData( player, values );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFPublicClientModule::OnLeaveUpdatePublicData )
    {
        StringMap values;
        values[ __STRING__( serverid ) ] = "0";
        values[ __STRING__( status ) ] = __TO_STRING__( KFMsg::FrameOfflineStatus );
        values[ __STRING__( statustime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        UpdatePublicData( player, values );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicClientModule::HandleQueryBasicReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryBasicReq );

        // 屏蔽字检查
        auto filter = _kf_filter->CheckFilter( kfmsg.name() );
        if ( filter )
        {
            return _kf_display->SendToClient( player, KFMsg::NameFilterError );
        }

        // 发送到public
        KFMsg::S2SQueryBasicToPublicReq req;
        req.set_name( kfmsg.name() );
        req.set_zoneid( KFZoneConfig::Instance()->ZoneSetting()->_logic_id );
        _kf_route->SendToRand( playerid, __STRING__( logic ), KFMsg::S2S_QUERY_BASIC_TO_PUBLIC_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicClientModule::HandleQueryBasicToGameAck )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2SQueryBasicToGameAck );

        if ( kfmsg.result() != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, kfmsg.result(), kfmsg.name() );
        }

        auto kfbasic = player->CreateData( __STRING__( basic ) );
        for ( auto iter = kfmsg.pbdata().begin(); iter != kfmsg.pbdata().end(); ++iter )
        {
            auto kfdata = kfbasic->Find( iter->first );
            if ( kfdata != nullptr )
            {
                kfdata->Set< std::string >( iter->second );
            }
        }
        auto pbplayerdata = _kf_kernel->SerializeToView( kfbasic );

        // 发送给客户端
        KFMsg::MsgQueryBasicAck ack;
        ack.mutable_player()->CopyFrom( *pbplayerdata );
        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_BASIC_ACK, &ack );
    }


    __KF_MESSAGE_FUNCTION__( KFPublicClientModule::HandleSetSexReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetSexReq );

        _kf_display->SendToClient( player, KFMsg::SexSetOK );
        player->UpdateData( __STRING__( basic ), __STRING__( sex ), KFEnum::Set, kfmsg.sex() );
    }

    uint32 KFPublicClientModule::CheckNameValid( const std::string& name, uint32 maxlength )
    {
        if ( name.size() > maxlength )
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPublicClientModule::HandleSetNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetNameReq );

        if ( kfmsg.name().empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::NameEmpty );
        }

        auto kfname = player->Find( __STRING__( basic ), __STRING__( name ) );
        auto name = kfname->Get<std::string>();
        if ( !name.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::NameAlreadySet );
        }

        // 检查名字的有效性
        auto result = CheckNameValid( kfmsg.name(), kfname->_data_setting->_int_max_value );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }

        // 修改名字
        KFMsg::S2SSetPlayerNameToDataReq req;
        req.set_playerid( playerid );
        req.set_oldname( name );
        req.set_newname( kfmsg.name() );
        req.set_costdata( _invalid_string );
        req.set_zoneid( KFZoneConfig::Instance()->ZoneSetting()->_logic_id );
        auto ok = _kf_route->SendToRand( playerid, __STRING__( logic ), KFMsg::S2S_SET_PLAYERNAME_TO_DATA_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::PublicServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPublicClientModule::HandleSetPlayerNameToGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SSetPlayerNameToGameAck );

        auto player = _kf_player->FindPlayer( kfmsg.playerid() );
        if ( player == nullptr )
        {
            return __LOG_ERROR__( "player[{}] set name[{}] item[{}] failed", kfmsg.playerid(), kfmsg.name(), kfmsg.costdata() );
        }

        _kf_display->SendToClient( player, kfmsg.result(), kfmsg.name() );
        if ( kfmsg.result() != KFMsg::NameSetOk )
        {
            return;
        }

        player->UpdateData( __STRING__( basic ), __STRING__( name ), kfmsg.name() );
        if ( kfmsg.costdata() != _invalid_string )
        {
            KFElements kfelements;
            auto ok = kfelements.Parse( kfmsg.costdata(), __FUNC_LINE__ );
            if ( ok )
            {
                player->RemoveElement( &kfelements, _default_multiple, __STRING__( name ), 0u, __FUNC_LINE__ );
            }
        }
    }
}
