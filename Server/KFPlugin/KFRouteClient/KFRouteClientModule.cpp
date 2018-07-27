#include "KFRouteClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFRouteClientModule::KFRouteClientModule()
    {
        _kf_transmit_function = nullptr;
    }

    KFRouteClientModule::~KFRouteClientModule()
    {
    }

    void KFRouteClientModule::InitModule()
    {
        ///////////////////////////////////////////////////////////////////////////////
    }

    void KFRouteClientModule::BeforeRun()
    {
        _kf_cluster->RegisterConnectionFunction( __KF_STRING__( route ), this, &KFRouteClientModule::OnConnectionRouteProxy );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_ROUTE_ZONE_MESSAGE_ACK, &KFRouteClientModule::HandleTransmitRouteZoneMessageAck );
    }

    void KFRouteClientModule::BeforeShut()
    {
        _kf_cluster->UnRegisterConnectionFunction( __KF_STRING__( route ) );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_ROUTE_ZONE_MESSAGE_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::SetTransmitFunction( KFTransmitFunction& function )
    {
        _kf_transmit_function = function;
    }

    void KFRouteClientModule::UnRegisterTransmitFunction()
    {
        _kf_transmit_function = nullptr;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::OnConnectionRouteProxy( uint32 serverid )
    {
        // 只有Game才注册路由信息
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_type != __KF_STRING__( game ) )
        {
            return;
        }

        // 连接到Route Proxy, 发送区信息到Proxy
        KFMsg::S2SRegisterRouteZoneReq req;
        auto zonedata = req.mutable_zonedata();
        zonedata->set_serverid( kfglobal->_app_id );
        zonedata->set_zoneid( _kf_zone->GetZone()->_id );
        _kf_cluster->SendMessageToShard( __KF_STRING__( route ), KFMsg::S2S_REGISTER_ROUTE_ZONE_REQ, &req );
    }

    bool KFRouteClientModule::SendMessageToRoute( uint32 serverid, uint32 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        if ( serverid == _invalid_int || playerid == _invalid_int )
        {
            return false;
        }

        bool ok = false;
        if ( _kf_zone->IsServerSameZone( serverid ) )
        {
            ok = _kf_game->SendMessageToTransmit( playerid, msgid, message );
        }
        else
        {
            auto data = message->SerializeAsString();
            ok = SendMessageToRoute( serverid, playerid, msgid, data.data(), static_cast< uint32 >( data.length() ) );
        }

        return ok;
    }

    bool KFRouteClientModule::SendMessageToRoute( uint32 serverid, uint32 playerid, uint32 msgid, const char* data, uint32 length )
    {
        KFMsg::S2STransmitRouteZoneMessageReq req;
        auto transmitdata = req.mutable_transmitdata();
        transmitdata->set_serverid( serverid );
        transmitdata->set_playerid( playerid );
        transmitdata->set_msgid( msgid );
        transmitdata->set_msgdata( data, length );
        return _kf_cluster->SendMessageToShard( __KF_STRING__( route ), KFMsg::S2S_TRANSMIT_ROUTE_ZONE_MESSAGE_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleTransmitRouteZoneMessageAck )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitRouteZoneMessageAck );

        auto transmitdata = &kfmsg.transmitdata();

        auto msgid = transmitdata->msgid();

        KFGuid tempguid = kfguid;
        tempguid._data_id = transmitdata->playerid();
        auto msgdata = transmitdata->msgdata().data();
        auto msglength = static_cast< uint32 >( transmitdata->msgdata().length() );

        bool ok = _kf_message->CallFunction( tempguid, msgid, msgdata, msglength );
        if ( ok )
        {
            return;
        }

        if ( _kf_transmit_function != nullptr )
        {
            auto ok = _kf_transmit_function( tempguid, msgid, msgdata, msglength );
            if ( !ok )
            {
                KFLogger::LogNet( KFLogger::Error, "[%s] route transmit msgid[%u] failed!",
                                  __FUNCTION__, msgid );
            }
        }
        else
        {
            KFLogger::LogSystem( KFLogger::Error, "[%s] msgid[%u] can't find handle",
                                 __FUNCTION__, msgid );
        }
    }
}
