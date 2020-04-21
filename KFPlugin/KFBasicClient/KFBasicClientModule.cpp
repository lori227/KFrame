#include "KFBasicClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __ROUTE_NAME__ __STRING__( basic )

    void KFBasicClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA__( &KFBasicClientModule::OnUpdateDataCallBack );
        __REGISTER_UPDATE_STRING__( &KFBasicClientModule::OnUpdateStringCallBack );

        __REGISTER_AFTER_ENTER_PLAYER__( &KFBasicClientModule::OnEnterUpdateBasicAttribute );
        __REGISTER_LEAVE_PLAYER__( &KFBasicClientModule::OnLeaveUpdateBasicAttribute );

        _kf_route->RegisterConnectionFunction( this, &KFBasicClientModule::OnRouteConnectCluster );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_SEX_REQ, &KFBasicClientModule::HandleSetSexReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_NAME_REQ, &KFBasicClientModule::HandleSetNameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_TO_GAME_ACK, &KFBasicClientModule::HandleSetPlayerNameToGameAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_BASIC_REQ, &KFBasicClientModule::HandleQueryBasicReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_ATTRIBUTE_TO_GAME_ACK, &KFBasicClientModule::HandleQueryAttributeToGameAck );
    }

    void KFBasicClientModule::BeforeShut()
    {
        __UN_UPDATE_DATA__();
        __UN_UPDATE_STRING__();
        __UN_AFTER_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();

        _kf_route->UnRegisterConnectionFunction( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SET_NAME_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_SEX_REQ );
        __UN_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::MSG_QUERY_BASIC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_ATTRIBUTE_TO_GAME_ACK );
    }

    void KFBasicClientModule::OnRouteConnectCluster( uint64 serverid )
    {
        if ( !_need_refresh_online )
        {
            return;
        }

        _need_refresh_online = false;

        // 清空该服务器上的玩家基础属性(serverid)
        KFMsg::S2SClearOnlineToBasicReq req;
        req.set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_route->RepeatToRand( __ROUTE_NAME__, KFMsg::S2S_CLEAR_ONLINE_TO_BASIC_REQ, &req );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFBasicClientModule::UpdateBasicIntValueToBasic( uint64 playerid, KeyValue& values )
    {
        KFMsg::S2SUpdateIntValueToBasicReq req;
        req.mutable_pbdata()->insert( values.begin(), values.end() );
        _kf_route->RepeatToRand( playerid, __ROUTE_NAME__, KFMsg::S2S_UPDATE_INT_VALUE_TO_BASIC_REQ, &req );
    }

    void KFBasicClientModule::UpdateBasicIntValueToBasic( uint64 playerid, const std::string& dataname, uint64 datavalue )
    {
        KeyValue values;
        values[ dataname ] = datavalue;
        UpdateBasicIntValueToBasic( playerid, values );
    }

    void KFBasicClientModule::UpdateBasicStrValueToBasic( uint64 playerid, const std::string& dataname, const std::string& datavalue )
    {
        KFMsg::S2SUpdateStrValueToBasicReq req;
        ( *req.mutable_pbdata() )[ dataname ] = datavalue;
        _kf_route->RepeatToRand( playerid, __ROUTE_NAME__, KFMsg::S2S_UPDATE_STR_VALUE_TO_BASIC_REQ, &req );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFBasicClientModule::OnEnterUpdateBasicAttribute )
    {
        EnterLeaveUpdateAttributeToBasic( player );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFBasicClientModule::OnLeaveUpdateBasicAttribute )
    {
        EnterLeaveUpdateAttributeToBasic( player );
    }

    void KFBasicClientModule::EnterLeaveUpdateAttributeToBasic( KFEntity* player )
    {
        auto kfbasic = player->Find( __STRING__( basic ) );

        KeyValue values;
        values[ __STRING__( serverid ) ] = kfbasic->Get( __STRING__( serverid ) );
        values[ __STRING__( status ) ] = kfbasic->Get( __STRING__( status ) );
        values[ __STRING__( statustime ) ] = kfbasic->Get( __STRING__( statustime ) );
        UpdateBasicIntValueToBasic( player->GetKeyID(), values );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFBasicClientModule::OnUpdateDataCallBack )
    {
        // 只有公共属性才更新
        if ( !player->IsInited() ||
                !kfdata->HaveMask( KFDataDefine::Mask_Basic ) ||
                !kfdata->GetParent()->HaveMask( KFDataDefine::Mask_Basic ) )
        {
            return;
        }

        UpdateBasicIntValueToBasic( player->GetKeyID(), kfdata->_data_setting->_name, newvalue );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFBasicClientModule::OnUpdateStringCallBack )
    {
        // 只有公共属性才更新
        if ( !player->IsInited() ||
                !kfdata->HaveMask( KFDataDefine::Mask_Basic ) ||
                !kfdata->GetParent()->HaveMask( KFDataDefine::Mask_Basic ) )
        {
            return;
        }

        UpdateBasicStrValueToBasic( player->GetKeyID(), kfdata->_data_setting->_name, newvalue );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleQueryBasicReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryBasicReq );

        // 屏蔽字检查
        auto filter = _kf_filter->CheckFilter( kfmsg.name() );
        if ( filter )
        {
            return _kf_display->SendToClient( player, KFMsg::NameFilterError );
        }

        // 发送到basic
        KFMsg::S2SQueryAttributeToBasicReq req;
        req.set_name( kfmsg.name() );
        req.set_zoneid( KFZoneConfig::Instance()->ZoneSetting()->_logic_id );
        _kf_route->SendToRand( playerid, __ROUTE_NAME__, KFMsg::S2S_QUERY_ATTRIBUTE_TO_BASIC_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleQueryAttributeToGameAck )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2SQueryAttributeToGameAck );

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

    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleSetSexReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetSexReq );

        _kf_display->SendToClient( player, KFMsg::SexSetOK );
        player->UpdateData( __STRING__( basic ), __STRING__( sex ), KFEnum::Set, kfmsg.sex() );
    }

    uint32 KFBasicClientModule::CheckNameValid( const std::string& name, uint32 maxlength )
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
    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleSetNameReq )
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
        KFMsg::S2SSetPlayerNameToBasicReq req;
        req.set_playerid( playerid );
        req.set_oldname( name );
        req.set_newname( kfmsg.name() );
        req.set_costdata( _invalid_string );
        req.set_zoneid( KFZoneConfig::Instance()->ZoneSetting()->_logic_id );
        auto ok = _kf_route->SendToRand( playerid, __ROUTE_NAME__, KFMsg::S2S_SET_PLAYER_NAME_TO_BASIC_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::PublicServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBasicClientModule::HandleSetPlayerNameToGameAck )
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
