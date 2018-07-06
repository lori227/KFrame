#include "KFRouteProxyModule.h"
#include "KFRouteZoneManage.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFRouteProxyModule::KFRouteProxyModule()
    {
    }

    KFRouteProxyModule::~KFRouteProxyModule()
    {
    }

    void KFRouteProxyModule::InitModule()
    {
        ///////////////////////////////////////////////////////////////////////////////
    }

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
        if ( servertype != KFField::_shard )
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
        _kf_cluster_proxy->SendMessageToShard( serverid, KFMsg::S2S_REGISTER_ROUTE_PROXY_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRegisterZoneReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterRouteZoneReq );

        auto handleid = __KF_HEAD_ID__( kfguid );
        auto zonedata = &kfmsg.zonedata();

        // 注册到管理器
        _kf_zone_manage->AddRouteZone( zonedata->zoneid(), zonedata->serverid(), handleid );

        KFLogger::LogLogic( KFLogger::Info, "[%s] register route server[%u=>%u]!",
                            __FUNCTION__, zonedata->serverid(), handleid );

        // 通知route shard
        KFMsg::S2SRegisterRouteProxyReq req;
        auto addzonedata = req.add_zonedata();
        addzonedata->CopyFrom( *zonedata );
        _kf_cluster_proxy->SendMessageToShard( KFMsg::S2S_REGISTER_ROUTE_PROXY_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleTransmitRouteZoneMessageReq )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitRouteZoneMessageReq );

        // 选择一个client 发送消息
        auto handleid = __KF_HEAD_ID__( kfguid );
        uint32 shardserverid = _kf_cluster_proxy->SelectClusterShard( handleid );
        if ( shardserverid == _invalid_int )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can not select route shard!",
                                       __FUNCTION__ );
        }

        KFMsg::S2STransmitRouteProxyMessageReq req;
        req.mutable_transmitdata()->CopyFrom( kfmsg.transmitdata() );
        _kf_cluster_proxy->SendMessageToShard( shardserverid, KFMsg::S2S_TRANSMIT_ROUTE_PROXY_MESSAGE_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleTransmitRouteProxyMessageAck )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitRouteProxyMessageAck );

        auto transmitdata = &kfmsg.transmitdata();
        auto kfroutezone = _kf_zone_manage->FindRouteZone( transmitdata->serverid() );
        if ( kfroutezone == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't route server[%u] !",
                                       __FUNCTION__, transmitdata->serverid() );
        }

        KFMsg::S2STransmitRouteZoneMessageAck ack;
        ack.mutable_transmitdata()->CopyFrom( *transmitdata );
        _kf_cluster_proxy->SendMessageToClient( kfroutezone->_handle_id, KFMsg::S2S_TRANSMIT_ROUTE_ZONE_MESSAGE_ACK, &ack );
    }
}