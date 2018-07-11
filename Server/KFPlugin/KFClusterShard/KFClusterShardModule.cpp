#include "KFClusterShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFClusterShardModule::KFClusterShardModule()
    {

    }

    KFClusterShardModule::~KFClusterShardModule()
    {
    }

    void KFClusterShardModule::InitModule()
    {

    }

    void KFClusterShardModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFClusterShardModule::OnServerLostHandle );
        //////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_LIST_REQ, &KFClusterShardModule::HandleClusterClientListReq );

    }

    void KFClusterShardModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_LIST_REQ );
    }

    uint32 KFClusterShardModule::FindProxyId( uint32 clientid )
    {
        auto iter = _proxy_client_list.find( clientid );
        if ( iter == _proxy_client_list.end() )
        {
            return _invalid_int;
        }

        return iter->second;
    }

    __KF_MESSAGE_FUNCTION__( KFClusterShardModule::HandleClusterClientListReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterClientListReq );

        auto proxyid = __KF_HEAD_ID__( kfguid );
        for ( auto i = 0; i < kfmsg.clientid_size(); ++i )
        {
            auto clientid = kfmsg.clientid( i );
            _proxy_client_list[ clientid ] = proxyid;
        }
    }

    __KF_SERVER_LOST_FUNCTION__( KFClusterShardModule::OnServerLostHandle )
    {
        std::set< uint32 > removelist;

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
    void KFClusterShardModule::SendMessageToProxy( uint32 msgid, const char* data, uint32 length )
    {
        _kf_tcp_server->SendNetMessage( msgid, data, length );
    }

    void KFClusterShardModule::SendMessageToProxy( uint32 msgid, google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msgid, message );
    }

    bool KFClusterShardModule::SendMessageToProxy( uint32 handleid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_server->SendNetMessage( handleid, msgid, data, length );
    }

    bool KFClusterShardModule::SendMessageToProxy( uint32 handleid, uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( handleid, msgid, message );
    }

    bool KFClusterShardModule::SendMessageToClient( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length )
    {
        auto proxyid = __KF_HEAD_ID__( kfguid );
        auto clientid = __KF_DATA_ID__( kfguid );
        if ( proxyid == _invalid_int )
        {
            proxyid = FindProxyId( clientid );
        }

        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, data, length );
    }

    bool KFClusterShardModule::SendMessageToClient( const KFGuid& kfguid, uint32 msgid, google::protobuf::Message* message )
    {
        auto proxyid = __KF_HEAD_ID__( kfguid );
        auto clientid = __KF_DATA_ID__( kfguid );
        if ( proxyid == _invalid_int )
        {
            proxyid = FindProxyId( clientid );
        }

        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, message );
    }

    bool KFClusterShardModule::SendMessageToClient( uint32 clientid, uint32 msgid, const char* data, uint32 length )
    {
        auto proxyid = FindProxyId( clientid );
        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, data, length );
    }

    bool KFClusterShardModule::SendMessageToClient( uint32 clientid, uint32 msgid, google::protobuf::Message* message )
    {
        auto proxyid = FindProxyId( clientid );
        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, message );
    }

    bool KFClusterShardModule::SendMessageToClient( uint32 proxyid, uint32 clientid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, data, length );
    }

    bool KFClusterShardModule::SendMessageToClient( uint32 proxyid, uint32 clientid, uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, message );
    }

    bool KFClusterShardModule::SendMessageToPlayer( uint32 clientid, uint32 playerid, uint32 msgid, google::protobuf::Message* message )
    {
        KFMsg::S2STransmitRouteZoneMessageAck ack;
        auto transmitdata = ack.mutable_transmitdata();
        transmitdata->set_serverid( clientid );
        transmitdata->set_playerid( playerid );
        transmitdata->set_msgid( msgid );
        transmitdata->set_msgdata( message->SerializeAsString() );
        return SendMessageToClient( clientid, KFMsg::S2S_TRANSMIT_ROUTE_ZONE_MESSAGE_ACK, &ack );
    }

    void KFClusterShardModule::AddObjectToProxy( uint64 objectid )
    {
        KFMsg::S2SAddObjectToProxyReq req;
        req.add_objectid( objectid );
        SendMessageToProxy( KFMsg::S2S_ADD_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFClusterShardModule::AddObjectToProxy( uint32 proxyid, const std::list<uint64>& objectlist )
    {
        KFMsg::S2SAddObjectToProxyReq req;
        for ( auto objectid : objectlist )
        {
            req.add_objectid( objectid );
        }
        SendMessageToProxy( proxyid, KFMsg::S2S_ADD_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFClusterShardModule::RemoveObjectToProxy( uint64 objectid )
    {
        KFMsg::S2SRemoveObjectToProxyReq req;
        req.add_objectid( objectid );
        SendMessageToProxy( KFMsg::S2S_REMOVE_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFClusterShardModule::RemoveObjectToProxy( const std::list<uint64>& objectlist )
    {
        KFMsg::S2SRemoveObjectToProxyReq req;
        for ( auto objectid : objectlist )
        {
            req.add_objectid( objectid );
        }
        SendMessageToProxy( KFMsg::S2S_REMOVE_OBJECT_TO_PROXY_REQ, &req );
    }
}