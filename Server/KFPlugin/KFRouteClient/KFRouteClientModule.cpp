#include "KFRouteClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRouteClientModule::BeforeRun()
    {
        _kf_cluster_client->RegisterConnectionFunction( this, &KFRouteClientModule::OnRouteConnectCluster );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_DISCOVER_TO_CLIENT_REQ, &KFRouteClientModule::HandleRouteDiscoverToClientReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK, &KFRouteClientModule::HandleRouteMessageToClientAck );
    }

    void KFRouteClientModule::BeforeShut()
    {
        _kf_cluster_client->UnRegisterConnectionFunction( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_DISCOVER_TO_CLIENT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK );
    }

    void KFRouteClientModule::OnceRun()
    {
        // 找到route 地址自动连接, 不需要bus对每个节点都配置一条连接信息
        auto* kfaddress = _kf_ip_address->FindIpAddress( __KF_STRING__( route ), __KF_STRING__( master ), _globbing_str );
        if ( kfaddress == nullptr )
        {
            return __LOG_ERROR__( "can't find [{}:{}] ip setting!", __KF_STRING__( route ), __KF_STRING__( master ) );
        }

        _kf_tcp_client->StartClient( kfaddress->_app_name, kfaddress->_app_type, kfaddress->_app_id, kfaddress->_ip, kfaddress->_port );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::SetTransmitFunction( KFTransmitFunction& function )
    {
        _kf_transmit_function = function;
    }

    void KFRouteClientModule::OnRouteConnectCluster( uint64 serverid )
    {
        for ( auto& iter : _service_object_list )
        {
            RouteSyncObjectToProxy( iter.first, iter.second );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToAll( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToNameAllReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id._union._id );

        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &req );
    }

    bool KFRouteClientModule::SendToRand( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToNameRandReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id._union._id );

        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &req );
    }

    bool KFRouteClientModule::SendToBalance( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToNameBalanceReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id._union._id );

        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &req );
    }

    bool KFRouteClientModule::SendToObject( uint64 sendid, const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToNameObjectReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( objectid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id._union._id );

        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &req );
    }

    bool KFRouteClientModule::SendToServer( uint64 sendid, uint64 serverid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToServerReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( serverid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id._union._id );

        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &req );
    }

    bool KFRouteClientModule::SendToPlayer( uint64 sendid, uint64 serverid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToPlayerReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( playerid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id._union._id );

        req.set_targetid( serverid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ, &req );
    }

    bool KFRouteClientModule::SendToRoute( const Route& route, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto serverid = __ROUTE_SERVER_ID__;
        auto sendid = __ROUTE_RECV_ID__;
        auto recvid = __ROUTE_SEND_ID__;

        return SendToPlayer( sendid, serverid, recvid, msgid, message );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteMessageToClientAck )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToClientAck );

        auto pbroute = &kfmsg.pbroute();
        auto msgdata = kfmsg.msgdata().data();
        auto msglength = static_cast< uint32 >( kfmsg.msgdata().length() );

        Route temproute( pbroute->serverid(), pbroute->sendid(), pbroute->recvid() );
        bool ok = _kf_message->CallFunction( temproute, kfmsg.msgid(), msgdata, msglength );
        if ( ok )
        {
            return;
        }

        if ( _kf_transmit_function != nullptr )
        {
            auto ok = _kf_transmit_function( temproute, kfmsg.msgid(), msgdata, msglength );
            if ( !ok )
            {
                __LOG_ERROR__( "route msgid[{}] failed!", kfmsg.msgid() );
            }
        }
        else
        {
            __LOG_ERROR__( "msgid[{}] can't find handle", kfmsg.msgid() );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::RegisterService( const std::string& name )
    {
        RouteObjectList temp;
        SyncObject( name, temp );
    }

    void KFRouteClientModule::SyncObject( const std::string& name, RouteObjectList& objectlist )
    {
        auto& savelist = _service_object_list[ name ];
        savelist.swap( objectlist );

        // 如果在服务中, 同步给转发集群
        if ( _kf_cluster_client->IsInService() )
        {
            RouteSyncObjectToProxy( name, savelist );
        }
    }

    void KFRouteClientModule::RouteSyncObjectToProxy( const std::string& name, RouteObjectList& objectlist )
    {
        // 发送给shard
        KFMsg::S2SRouteSyncObjectToProxyReq req;
        req.set_name( name );
        req.set_shardid( _invalid_int );
        req.set_clientid( KFGlobal::Instance()->_app_id._union._id );

        for ( auto objectid : objectlist )
        {
            req.add_objectid( objectid );
        }

        auto ok = _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_PROXY_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "service=[{}] sync object list failed!", name );
        }
    }

    void KFRouteClientModule::AddObject( const std::string& name, uint64 objectid )
    {
        auto iter = _service_object_list.find( name );
        if ( iter != _service_object_list.end() )
        {
            iter->second.insert( objectid );
        }

        KFMsg::S2SRouteAddObjectToProxyReq req;
        req.set_name( name );
        req.set_objectid( objectid );
        req.set_clientid( KFGlobal::Instance()->_app_id._union._id );
        auto ok = _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_PROXY_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "service[{}] add object[{}] failed!", name, objectid );
        }
    }

    void KFRouteClientModule::RemoveObject( const std::string& name, uint64 objectid )
    {
        auto iter = _service_object_list.find( name );
        if ( iter != _service_object_list.end() )
        {
            iter->second.erase( objectid );
        }

        KFMsg::S2SRouteRemoveObjectToProxyReq req;
        req.set_name( name );
        req.set_objectid( objectid );
        req.set_clientid( KFGlobal::Instance()->_app_id._union._id );
        auto ok = _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_PROXY_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "service[{}] remove object[{}] failed!", name, objectid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteDiscoverToClientReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteDiscoverToClientReq )

        // 把所有对象列表同步到shard
        for ( auto& iter : _service_object_list )
        {
            // 发送给shard
            KFMsg::S2SRouteSyncObjectToProxyReq req;
            req.set_name( iter.first );
            req.set_shardid( kfmsg.shardid() );
            req.set_clientid( KFGlobal::Instance()->_app_id._union._id );

            for ( auto objectid : iter.second )
            {
                req.add_objectid( objectid );
            }

            auto ok = _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_SHARD_REQ, &req );
            if ( !ok )
            {
                __LOG_ERROR__( "service[{}] sync object failed!", iter.first );
            }
        }
    }
}
