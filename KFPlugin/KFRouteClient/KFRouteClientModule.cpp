#include "KFRouteClientModule.hpp"
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
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_DISCOVER_TO_CLIENT_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK );
    }

    void KFRouteClientModule::PrepareRun()
    {
        // 注册服务
        RegisterService();

        // 找到route 地址自动连接, 不需要bus对每个节点都配置一条连接信息
        auto* kfaddress = _kf_ip_address->FindIpAddress( __KF_STRING__( route ), __KF_STRING__( master ), _globbing_str );
        if ( kfaddress == nullptr )
        {
            return __LOG_ERROR__( "can't find [{}:{}] ip setting!", __KF_STRING__( route ), __KF_STRING__( master ) );
        }

        auto port = _kf_ip_address->CalcListenPort( kfaddress->_port_type, kfaddress->_port, kfaddress->_id );
        _kf_tcp_client->StartClient( kfaddress->_name, kfaddress->_type, kfaddress->_id, kfaddress->_ip, port );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function )
    {
        auto kffunction = _kf_connection_function.Create( name );
        kffunction->_function = function;
    }

    void KFRouteClientModule::RemoveConnectionFunction( const std::string& name )
    {
        _kf_connection_function.Remove( name );
    }

    void KFRouteClientModule::SetTranspondFunction( KFTranspondFunction& function )
    {
        _kf_transpond_function = function;
    }

    void KFRouteClientModule::OnRouteConnectCluster( uint64 serverid )
    {
        RouteSyncObjectToProxy( _invalid_int );

        // 连接成功回调
        for ( auto& iter : _kf_connection_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( serverid );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToAll( const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        return SendToAll( 0u, name, msgid, message, resend );
    }

    bool KFRouteClientModule::SendToAll( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        KFMsg::S2SRouteMessageToNameAllReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &req, resend );
    }

    bool KFRouteClientModule::SendToRand( const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        return SendToRand( 0u, name, msgid, message, resend );
    }

    bool KFRouteClientModule::SendToRand( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        KFMsg::S2SRouteMessageToNameRandReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &req, resend );
    }

    bool KFRouteClientModule::SendToBalance( const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        return SendToBalance( 0u, name, msgid, message, resend );
    }

    bool KFRouteClientModule::SendToBalance( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        KFMsg::S2SRouteMessageToNameBalanceReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &req, resend );
    }

    bool KFRouteClientModule::SendToObject( const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        return SendToObject( 0u, name, objectid, msgid, message, resend );
    }

    bool KFRouteClientModule::SendToObject( uint64 sendid, const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        KFMsg::S2SRouteMessageToNameObjectReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( objectid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( name );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &req, resend );
    }

    bool KFRouteClientModule::SendToServer( uint64 serverid, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        return SendToServer( 0u, serverid, msgid, message, resend );
    }

    bool KFRouteClientModule::SendToServer( uint64 sendid, uint64 serverid, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        KFMsg::S2SRouteMessageToServerReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( serverid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_targetid( serverid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &req, resend );
    }

    bool KFRouteClientModule::SendToPlayer( uint64 serverid, uint64 recvid, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        return SendToPlayer( recvid, serverid, recvid, msgid, message, resend );
    }

    bool KFRouteClientModule::SendToPlayer( uint64 sendid, uint64 serverid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message, bool resend )
    {
        KFMsg::S2SRouteMessageToPlayerReq req;

        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( playerid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_targetid( serverid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ, &req, resend );
    }

    bool KFRouteClientModule::SendToRoute( const Route& route, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto serverid = __ROUTE_SERVER_ID__;
        auto sendid = __ROUTE_RECV_ID__;
        auto recvid = __ROUTE_SEND_ID__;

        return SendToPlayer( sendid, serverid, recvid, msgid, message, true );
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

        if ( _kf_transpond_function != nullptr )
        {
            auto ok = _kf_transpond_function( temproute, kfmsg.msgid(), msgdata, msglength );
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
    void KFRouteClientModule::RegisterService()
    {
        _service_object_list.clear();
        _service_name = KFGlobal::Instance()->_app_name;
    }

    void KFRouteClientModule::SyncObject( RouteObjectList& objectlist )
    {
        _service_object_list.swap( objectlist );

        // 如果在服务中, 同步给转发集群
        if ( _kf_cluster_client->IsInService() )
        {
            RouteSyncObjectToProxy( _invalid_int );
        }
    }

    void KFRouteClientModule::RouteSyncObjectToProxy( uint64 shardid )
    {
        // 发送给shard
        KFMsg::S2SRouteSyncObjectToProxyReq req;
        req.set_name( _service_name );
        req.set_shardid( shardid );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );

        for ( auto objectid : _service_object_list )
        {
            req.add_objectid( objectid );
        }

        auto ok = _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_PROXY_REQ, &req, true );
        if ( !ok )
        {
            __LOG_ERROR__( "service=[{}] sync object list failed!", _service_name );
        }
    }

    void KFRouteClientModule::AddObject( uint64 objectid )
    {
        _service_object_list.insert( objectid );

        KFMsg::S2SRouteAddObjectToProxyReq req;
        req.set_name( _service_name );
        req.set_objectid( objectid );
        req.set_objectcount( _service_object_list.size() );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );
        auto ok = _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_PROXY_REQ, &req, true );
        if ( !ok )
        {
            __LOG_ERROR__( "service[{}] add object[{}] failed!", _service_name, objectid );
        }
    }

    void KFRouteClientModule::RemoveObject( uint64 objectid )
    {
        _service_object_list.erase( objectid );

        KFMsg::S2SRouteRemoveObjectToProxyReq req;
        req.set_name( _service_name );
        req.set_objectid( objectid );
        req.set_objectcount( _service_object_list.size() );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );
        auto ok = _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_PROXY_REQ, &req, true );
        if ( !ok )
        {
            __LOG_ERROR__( "service[{}] remove object[{}] failed!", _service_name, objectid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteDiscoverToClientReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteDiscoverToClientReq )

        // 把所有对象列表同步到shard
        RouteSyncObjectToProxy( kfmsg.shardid() );
    }
}
