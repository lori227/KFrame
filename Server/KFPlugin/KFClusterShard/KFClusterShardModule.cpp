#include "KFClusterShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFClusterShardModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFClusterShardModule::OnServerLostHandle );
        //////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ, &KFClusterShardModule::HandleClusterClientDiscoverToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_LOST_TO_SHARD_REQ, &KFClusterShardModule::HandleClusterClientLostToShardReq );
    }

    void KFClusterShardModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_LOST_TO_SHARD_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterShardModule::HandleClusterClientDiscoverToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterClientDiscoverToShardReq );

        auto proxyid = __KF_HEAD_ID__( kfid );
        for ( auto i = 0; i < kfmsg.clientid_size(); ++i )
        {
            auto clientid = kfmsg.clientid( i );
            _proxy_client_list[ clientid ] = proxyid;
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterShardModule::HandleClusterClientLostToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterClientLostToShardReq );

        _proxy_client_list.erase( kfmsg.clientid() );
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


    uint64 KFClusterShardModule::FindProxyId( uint64 clientid )
    {
        auto iter = _proxy_client_list.find( clientid );
        if ( iter == _proxy_client_list.end() )
        {
            return _invalid_int;
        }

        return iter->second;
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
}