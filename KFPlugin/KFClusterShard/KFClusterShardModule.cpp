#include "KFClusterShardModule.hpp"

namespace KFrame
{
    void KFClusterShardModule::BeforeRun()
    {
        __REGISTER_TCP_SERVER_LOST__( &KFClusterShardModule::OnServerLostHandle );
        //////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFClusterShardModule, KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ, KFMsg::S2SClusterClientDiscoverToShardReq, HandleClusterClientDiscoverToShardReq );
        __REGISTER_MESSAGE__( KFClusterShardModule, KFMsg::S2S_CLUSTER_CLIENT_LOST_TO_SHARD_REQ, KFMsg::S2SClusterClientLostToShardReq, HandleClusterClientLostToShardReq );
    }

    void KFClusterShardModule::BeforeShut()
    {
        __UN_TCP_SERVER_LOST__();
        //////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_CLIENT_LOST_TO_SHARD_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterShardModule::HandleClusterClientDiscoverToShardReq, KFMsg::S2SClusterClientDiscoverToShardReq )
    {
        auto proxyid = __ROUTE_SERVER_ID__;
        for ( auto i = 0; i < kfmsg->clientid_size(); ++i )
        {
            auto clientid = kfmsg->clientid( i );
            _proxy_client_list[ clientid ] = proxyid;
            __LOG_INFO__( "route discover client=[{}] proxy=[{}] ", KFAppId::ToString( clientid ), KFAppId::ToString( proxyid ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterShardModule::HandleClusterClientLostToShardReq, KFMsg::S2SClusterClientLostToShardReq )
    {
        _proxy_client_list.erase( kfmsg->clientid() );
        __LOG_WARN__( "route lost client=[{}]", KFAppId::ToString( kfmsg->clientid() ) );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterShardModule::OnServerLostHandle )
    {
        UInt64Set removelist;

        __LOG_ERROR__( "route lost proxy=[{}]", netdata->_str_id );

        for ( auto iter : _proxy_client_list )
        {
            if ( iter.second == netdata->_id )
            {
                removelist.insert( iter.first );
            }
        }

        for ( auto clientid : removelist )
        {
            _proxy_client_list.erase( clientid );
            __LOG_ERROR__( "route lost client=[{}]", KFAppId::ToString( clientid ) );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterShardModule::SendToProxy( uint32 msgid, google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msgid, message );
    }

    bool KFClusterShardModule::SendToProxy( uint64 proxyid, uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( proxyid, msgid, message );
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

    bool KFClusterShardModule::SendToClient( uint32 msgid, google::protobuf::Message* message )
    {
        for ( auto& iter : _proxy_client_list )
        {
            auto clientid = iter.first;
            auto proxyid = iter.second;
            _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, message );
        }

        return true;
    }

    bool KFClusterShardModule::SendToClient( uint64 clientid, uint32 msgid, google::protobuf::Message* message )
    {
        auto proxyid = FindProxyId( clientid );
        if ( proxyid == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, message );
    }

    bool KFClusterShardModule::SendToClient( uint64 proxyid, uint64 clientid, uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( proxyid, clientid, msgid, message );
    }
}