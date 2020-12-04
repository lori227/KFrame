#include "KFRouteProxyModule.hpp"

namespace KFrame
{
    void KFRouteProxyModule::BeforeRun()
    {
        __REGISTER_TCP_SERVER_LOST__( &KFRouteProxyModule::OnServerLostClient );
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFRouteProxyModule::OnClientConnectServer );

        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_PROXY_REQ, &KFRouteProxyModule::HandleRouteSyncObjectToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_PROXY_REQ, &KFRouteProxyModule::HandleRouteAddObjectToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_PROXY_REQ, &KFRouteProxyModule::HandleRouteRemoveObjectToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &KFRouteProxyModule::HandleRouteMessageToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ, &KFRouteProxyModule::HandleRouteMessageToPlayerReq );
    }

    void KFRouteProxyModule::BeforeShut()
    {
        __UN_TCP_SERVER_LOST__();
        __UN_TCP_CLIENT_CONNECTION__();

        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_PROXY_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_PROXY_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_PROXY_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ );

    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFRouteProxyModule::OnServerLostClient )
    {
        // 通知到shard
        KFMsg::S2SRouteClientLostToShardReq req;
        req.set_name( netdata->_name );
        req.set_clientid( netdata->_id );
        _kf_tcp_client->SendMessageToType( __STRING__( shard ), KFMsg::S2S_ROUTE_CLIENT_LOST_TO_SHARD_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFRouteProxyModule::OnClientConnectServer )
    {
        if ( netdata->_type == __STRING__( shard ) )
        {
            // 通知所有客户端, 请求同步信息
            KFMsg::S2SRouteDiscoverToClientReq req;
            req.set_shardid( netdata->_id );
            _kf_tcp_server->SendNetMessage( KFMsg::S2S_ROUTE_DISCOVER_TO_CLIENT_REQ, &req );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRouteSyncObjectToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteSyncObjectToProxyReq );

        KFMsg::S2SRouteSyncObjectToShardReq req;
        req.set_name( kfmsg->name() );
        req.set_clientid( kfmsg->clientid() );
        req.mutable_objectid()->CopyFrom( kfmsg->objectid() );

        if ( kfmsg->shardid() != _invalid_int )
        {
            _kf_tcp_client->SendNetMessage( kfmsg->shardid(), KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_SHARD_REQ, &req );
        }
        else
        {
            _kf_tcp_client->SendMessageToType( __STRING__( shard ), KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_SHARD_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRouteAddObjectToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteAddObjectToProxyReq );

        KFMsg::S2SRouteAddObjectToShardReq req;
        req.set_name( kfmsg->name() );
        req.set_clientid( kfmsg->clientid() );
        req.set_objectid( kfmsg->objectid() );
        req.set_objectcount( kfmsg->objectcount() );
        _kf_tcp_client->SendMessageToType( __STRING__( shard ), KFMsg::S2S_ROUTE_ADD_OBJECT_TO_SHARD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRouteRemoveObjectToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteRemoveObjectToProxyReq );

        KFMsg::S2SRouteRemoveObjectToShardReq req;
        req.set_name( kfmsg->name() );
        req.set_clientid( kfmsg->clientid() );
        req.set_objectid( kfmsg->objectid() );
        req.set_objectcount( kfmsg->objectcount() );
        _kf_tcp_client->SendMessageToType( __STRING__( shard ), KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_SHARD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRouteMessageToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToServerReq );
        auto pbroute = kfmsg->mutable_pbroute();

        // 判断是否在本服务器上
        bool havehandle = _kf_tcp_server->HaveHandle( kfmsg->targetid() );
        if ( havehandle )
        {
            KFMsg::S2SRouteMessageToClientAck ack;
            ack.set_msgid( kfmsg->msgid() );
            ack.set_msgdata( kfmsg->msgdata() );
            ack.mutable_pbroute()->CopyFrom( *pbroute );
            _kf_tcp_server->SendNetMessage( kfmsg->targetid(), KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK, &ack );
        }
        else
        {
            _kf_cluster_proxy->TranspondToShard( route, KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, data, length );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRouteProxyModule::HandleRouteMessageToPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToPlayerReq );

        // 判断是否在本服务器上
        bool havehandle = _kf_tcp_server->HaveHandle( kfmsg->targetid() );
        if ( havehandle )
        {
            KFMsg::S2SRouteMessageToClientAck ack;
            ack.set_msgid( kfmsg->msgid() );
            ack.set_msgdata( kfmsg->msgdata() );
            ack.mutable_pbroute()->CopyFrom( kfmsg->pbroute() );
            _kf_tcp_server->SendNetMessage( kfmsg->targetid(), KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK, &ack );
        }
        else
        {
            _kf_cluster_proxy->TranspondToShard( route, KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ, data, length );
        }
    }
}
