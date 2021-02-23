#include "KFClusterShardModule.hpp"

namespace KFrame
{
    void KFClusterShardModule::BeforeRun()
    {
        __REGISTER_TCP_SERVER_LOST__( &KFClusterShardModule::OnServerLostHandle );
        //////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ, &KFClusterShardModule::HandleClusterClientDiscoverToShardReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_CLIENT_LOST_TO_SHARD_REQ, &KFClusterShardModule::HandleClusterClientLostToShardReq );
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
        auto proxy_id = __ROUTE_SERVER_ID__;
        for ( auto i = 0; i < kfmsg->clientid_size(); ++i )
        {
            auto client_id = kfmsg->clientid( i );
            _proxy_client_list[ client_id ] = proxy_id;
            __LOG_INFO__( "route discover client=[{}] proxy=[{}] ", KFAppId::ToString( client_id ), KFAppId::ToString( proxy_id ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterShardModule::HandleClusterClientLostToShardReq, KFMsg::S2SClusterClientLostToShardReq )
    {
        _proxy_client_list.erase( kfmsg->clientid() );
        __LOG_WARN__( "route lost client=[{}]", KFAppId::ToString( kfmsg->clientid() ) );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterShardModule::OnServerLostHandle )
    {
        UInt64Set remove_list;

        __LOG_ERROR__( "route lost proxy=[{}]", net_data->_str_id );

        for ( auto iter : _proxy_client_list )
        {
            if ( iter.second == net_data->_id )
            {
                remove_list.insert( iter.first );
            }
        }

        for ( auto client_id : remove_list )
        {
            _proxy_client_list.erase( client_id );
            __LOG_ERROR__( "route lost client=[{}]", KFAppId::ToString( client_id ) );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterShardModule::SendToProxy( uint32 msg_id, google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msg_id, message );
    }

    bool KFClusterShardModule::SendToProxy( uint64 proxy_id, uint32 msg_id, google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( proxy_id, msg_id, message );
    }

    uint64 KFClusterShardModule::FindProxyId( uint64 client_id )
    {
        auto iter = _proxy_client_list.find( client_id );
        if ( iter == _proxy_client_list.end() )
        {
            return _invalid_int;
        }

        return iter->second;
    }

    bool KFClusterShardModule::SendToClient( uint32 msg_id, google::protobuf::Message* message )
    {
        for ( auto& iter : _proxy_client_list )
        {
            auto client_id = iter.first;
            auto proxy_id = iter.second;
            _kf_tcp_server->SendNetMessage( proxy_id, client_id, msg_id, message );
        }

        return true;
    }

    bool KFClusterShardModule::SendToClient( uint64 client_id, uint32 msg_id, google::protobuf::Message* message )
    {
        auto proxy_id = FindProxyId( client_id );
        if ( proxy_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_server->SendNetMessage( proxy_id, client_id, msg_id, message );
    }

    bool KFClusterShardModule::SendToClient( uint64 proxy_id, uint64 client_id, uint32 msg_id, google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( proxy_id, client_id, msg_id, message );
    }
}