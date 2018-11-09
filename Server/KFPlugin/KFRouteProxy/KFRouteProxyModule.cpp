#include "KFRouteProxyModule.h"
#include "KFRouteZoneManage.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRouteProxyModule::BeforeRun()
    {
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFRouteProxyModule::OnClientConnectionRouteShard );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_ROUTE_ZONE_REQ, &KFRouteProxyModule::HandleRegisterZoneReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_ROUTE_ZONE_MESSAGE_REQ, &KFRouteProxyModule::HandleTransmitRouteZoneMessageReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_ROUTE_PROXY_MESSAGE_ACK, &KFRouteProxyModule::HandleTransmitRouteProxyMessageAck );

    }

    void KFRouteProxyModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_ROUTE_ZONE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_ROUTE_ZONE_MESSAGE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_ROUTE_PROXY_MESSAGE_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFRouteProxyModule::OnClientConnectionRouteShard )
    {
        if ( servertype != __KF_STRING__( shard ) )
        {
            return;
        }

        // 把所有的分区信息注册到route shard
        KFMsg::S2SRegisterRouteProxyReq req;
        for ( auto iter : _kf_zone_manage->_kf_route_zone._objects )
        {
            auto zonedata = iter.second;

            auto pbzonedata = req.add_zonedata();
            pbzonedata->set_zoneid( zonedata->_zone_id );
            pbzonedata->set_serverid( zonedata->_server_id );
        }
        _kf_cluster_proxy->SendToShard( serverid, KFMsg::S2S_REGISTER_ROUTE_PROXY_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRegisterZoneReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterRouteZoneReq );

        auto handleid = __KF_HEAD_ID__( kfguid );
        auto zonedata = &kfmsg.zonedata();

        // 注册到管理器
        _kf_zone_manage->AddRouteZone( zonedata->zoneid(), zonedata->serverid(), handleid );

        __LOG_INFO__( "register route server[{}=>{}]!",
                      KFAppID::ToString( zonedata->serverid() ), KFAppID::ToString(  handleid ) );

        // 通知route shard
        KFMsg::S2SRegisterRouteProxyReq req;
        auto addzonedata = req.add_zonedata();
        addzonedata->CopyFrom( *zonedata );
        _kf_cluster_proxy->SendToShard( KFMsg::S2S_REGISTER_ROUTE_PROXY_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleTransmitRouteZoneMessageReq )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitRouteZoneMessageReq );

        // 选择一个client 发送消息
        auto handleid = __KF_HEAD_ID__( kfguid );
        uint32 shardserverid = _kf_cluster_proxy->SelectClusterShard( handleid );
        if ( shardserverid == _invalid_int )
        {
            return __LOG_ERROR__( "msgid[{}] can't select route shard!", kfmsg.transmitdata().msgid() );
        }

        KFMsg::S2STransmitRouteProxyMessageReq req;
        req.mutable_transmitdata()->CopyFrom( kfmsg.transmitdata() );
        _kf_cluster_proxy->SendToShard( shardserverid, KFMsg::S2S_TRANSMIT_ROUTE_PROXY_MESSAGE_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleTransmitRouteProxyMessageAck )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitRouteProxyMessageAck );

        auto transmitdata = &kfmsg.transmitdata();
        auto kfroutezone = _kf_zone_manage->FindRouteZone( transmitdata->serverid() );
        if ( kfroutezone == nullptr )
        {
            return __LOG_ERROR__( "msgid[{}] can't route server[{}]!",
                                  transmitdata->msgid(), KFAppID::ToString( transmitdata->serverid() ) );
        }

        KFMsg::S2STransmitRouteZoneMessageAck ack;
        ack.mutable_transmitdata()->CopyFrom( *transmitdata );
        _kf_cluster_proxy->SendToClient( kfroutezone->_handle_id, KFMsg::S2S_TRANSMIT_ROUTE_ZONE_MESSAGE_ACK, &ack );
    }
}
