#include "KFRouteProxyModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRouteProxyModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFRouteProxyModule::OnServerLostClient );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFRouteProxyModule::OnClientConnectServer );

        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_PROXY_REQ, &KFRouteProxyModule::HandleRouteSyncObjectToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_PROXY_REQ, &KFRouteProxyModule::HandleRouteAddObjectToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_PROXY_REQ, &KFRouteProxyModule::HandleRouteRemoveObjectToProxyReq );

    }

    void KFRouteProxyModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();

        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_PROXY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_PROXY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_PROXY_REQ );

    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFRouteProxyModule::OnServerLostClient )
    {
        // 通知到shard
        KFMsg::S2SRouteClientLostToShardReq req;
        req.set_name( handlename );
        req.set_clientid( handleid );
        _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), KFMsg::S2S_ROUTE_CLIENT_LOST_TO_SHARD_REQ, &req );
    }

    __KF_CLIENT_CONNECT_FUNCTION__( KFRouteProxyModule::OnClientConnectServer )
    {
        if ( servername == __KF_STRING__( shard ) )
        {
            // 通知所有客户端, 请求同步信息
            KFMsg::S2SRouteDiscoverToClientReq req;
            req.set_shardid( serverid );
            _kf_tcp_server->SendNetMessage( KFMsg::S2S_ROUTE_DISCOVER_TO_CLIENT_REQ, &req );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRouteSyncObjectToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteSyncObjectToProxyReq );

        KFMsg::S2SRouteSyncObjectToShardReq req;
        req.set_name( kfmsg.name() );
        req.set_clientid( kfmsg.clientid() );
        req.mutable_objectid()->CopyFrom( kfmsg.objectid() );

        if ( kfmsg.shardid() != _invalid_int )
        {
            _kf_tcp_client->SendNetMessage( kfmsg.shardid(), KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_SHARD_REQ, &req );
        }
        else
        {
            _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_SHARD_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRouteAddObjectToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteAddObjectToProxyReq );

        KFMsg::S2SRouteAddObjectToShardReq req;
        req.set_name( kfmsg.name() );
        req.set_clientid( kfmsg.clientid() );
        req.set_objectid( kfmsg.objectid() );
        _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), KFMsg::S2S_ROUTE_ADD_OBJECT_TO_SHARD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRouteRemoveObjectToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteRemoveObjectToProxyReq );

        KFMsg::S2SRouteRemoveObjectToShardReq req;
        req.set_name( kfmsg.name() );
        req.set_clientid( kfmsg.clientid() );
        req.set_objectid( kfmsg.objectid() );
        _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_SHARD_REQ, &req );
    }
}
