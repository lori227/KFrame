#include "KFClusterShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFClusterShardModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFClusterShardModule::OnServerLostHandle );
        //////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_REQ, &KFClusterShardModule::HandleClusterClientDiscoverReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_LOST_REQ, &KFClusterShardModule::HandleClusterClientLostReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ALLOC_OBJECT_TO_SHARD_ACK, &KFClusterShardModule::HandleAllocObjectToShardAck );

    }

    void KFClusterShardModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_LOST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ALLOC_OBJECT_TO_SHARD_ACK );
    }

    uint64 KFClusterShardModule::FindProxyId( uint64 clientid )
    {
        auto iter = _proxy_client_list.find( clientid );
        if ( iter == _proxy_client_list.end() )
        {
            return _invalid_int;
        }

        return iter->second;
    }

    __KF_MESSAGE_FUNCTION__( KFClusterShardModule::HandleClusterClientDiscoverReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterClientDiscoverReq );

        auto proxyid = __KF_HEAD_ID__( kfid );
        for ( auto i = 0; i < kfmsg.clientid_size(); ++i )
        {
            auto clientid = kfmsg.clientid( i );
            _proxy_client_list[ clientid ] = proxyid;
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterShardModule::HandleClusterClientLostReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterClientLostReq );

        auto proxyid = __KF_HEAD_ID__( kfid );
        for ( auto i = 0; i < kfmsg.clientid_size(); ++i )
        {
            auto clientid = kfmsg.clientid( i );
            _proxy_client_list.erase( clientid );
        }
    }

    __KF_SERVER_LOST_FUNCTION__( KFClusterShardModule::OnServerLostHandle )
    {
        std::set< uint64 > removelist;

        for ( auto iter : _proxy_client_list )
        {
            if ( iter.second == handleid )
            {
                removelist.insert( iter.first );
            }
        }

        for ( auto clientid : removelist )
        {
            _proxy_client_list.erase( clientid );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterShardModule::SendToProxy( uint32 msgid, const char* data, uint32 length )
    {
        _kf_tcp_server->SendNetMessage( msgid, data, length );
    }

    void KFClusterShardModule::SendToProxy( uint32 msgid, google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msgid, message );
    }

    bool KFClusterShardModule::SendToProxy( uint64 handleid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_server->SendNetMessage( handleid, msgid, data, length );
    }

    bool KFClusterShardModule::SendToProxy( uint64 handleid, uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( handleid, msgid, message );
    }

    bool KFClusterShardModule::SendToClient( const KFId& kfid, uint32 msgid, const char* data, uint32 length )
    {
        auto proxyid = __KF_HEAD_ID__( kfid );
        auto clientid = __KF_DATA_ID__( kfid );
        if ( proxyid == _invalid_int )
        {
            proxyid = FindProxyId( clientid );
        }

        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, data, length );
    }

    bool KFClusterShardModule::SendToClient( const KFId& kfid, uint32 msgid, google::protobuf::Message* message )
    {
        auto proxyid = __KF_HEAD_ID__( kfid );
        auto clientid = __KF_DATA_ID__( kfid );
        if ( proxyid == _invalid_int )
        {
            proxyid = FindProxyId( clientid );
        }

        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, message );
    }

    bool KFClusterShardModule::SendToClient( uint64 clientid, uint32 msgid, const char* data, uint32 length )
    {
        auto proxyid = FindProxyId( clientid );
        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, data, length );
    }

    bool KFClusterShardModule::SendToClient( uint64 clientid, uint32 msgid, google::protobuf::Message* message )
    {
        auto proxyid = FindProxyId( clientid );
        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, message );
    }

    bool KFClusterShardModule::SendToClient( uint64 proxyid, uint64 clientid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, data, length );
    }

    bool KFClusterShardModule::SendToClient( uint64 proxyid, uint64 clientid, uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, message );
    }

    bool KFClusterShardModule::SendToPlayer( uint64 clientid, uint64 playerid, uint32 msgid, google::protobuf::Message* message )
    {
        KFMsg::S2STransmitRouteZoneMessageAck ack;
        auto transmitdata = ack.mutable_transmitdata();
        transmitdata->set_serverid( clientid );
        transmitdata->set_playerid( playerid );
        transmitdata->set_msgid( msgid );
        transmitdata->set_msgdata( message->SerializeAsString() );
        return SendToClient( clientid, KFMsg::S2S_TRANSMIT_ROUTE_ZONE_MESSAGE_ACK, &ack );
    }

    void KFClusterShardModule::AddObjectToProxy( uint64 objectid )
    {
        KFMsg::S2SAddObjectToProxyReq req;
        req.add_objectid( objectid );
        SendToProxy( KFMsg::S2S_ADD_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFClusterShardModule::AddObjectToProxy( uint64 proxyid, const std::set<uint64>& objectlist )
    {
        KFMsg::S2SAddObjectToProxyReq req;
        for ( auto objectid : objectlist )
        {
            req.add_objectid( objectid );
        }
        SendToProxy( proxyid, KFMsg::S2S_ADD_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFClusterShardModule::RemoveObjectToProxy( uint64 objectid )
    {
        KFMsg::S2SRemoveObjectToProxyReq req;
        req.add_objectid( objectid );
        SendToProxy( KFMsg::S2S_REMOVE_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFClusterShardModule::RemoveObjectToProxy( const std::set<uint64>& objectlist )
    {
        KFMsg::S2SRemoveObjectToProxyReq req;
        for ( auto objectid : objectlist )
        {
            req.add_objectid( objectid );
        }
        SendToProxy( KFMsg::S2S_REMOVE_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFClusterShardModule::AllocObjectToMaster( const std::set<uint64>& objectlist )
    {
        KFMsg::S2SAllocObjectToMasterReq req;
        for ( auto objectid : objectlist )
        {
            req.add_objectid( objectid );
        }

        auto kfglobal = KFGlobal::Instance();
        _kf_tcp_client->SendMessageToServer( kfglobal->_app_name, __KF_STRING__( master ), KFMsg::S2S_ALLOC_OBJECT_TO_MASTER_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterShardModule::HandleAllocObjectToShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SAllocObjectToShardAck );
        _object_list.clear();

        for ( auto i = 0; i < kfmsg.objectid_size(); ++i )
        {
            _object_list.insert( kfmsg.objectid( i ) );
        }

        for ( auto& iter : _kf_alloc_object_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( _object_list );
        }
    }

    const std::set< uint64 >& KFClusterShardModule::GetAllocObjectList()
    {
        return _object_list;
    }

    void KFClusterShardModule::AddAllocObjectFunction( const std::string& module, KFAllocObjectFunction& function )
    {
        auto kffunction = _kf_alloc_object_function.Create( module );
        kffunction->_function = function;
    }

    void KFClusterShardModule::RemoveAllocObjectFunction( const std::string& module )
    {
        _kf_alloc_object_function.Remove( module );
    }
}