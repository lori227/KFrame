#include "KFClusterProxyModule.hpp"

namespace KFrame
{
    void KFClusterProxyModule::BeforeRun()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFClusterProxyModule::OnClientLostServer );
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFClusterProxyModule::OnClientConnectionServer );
        __REGISTER_TCP_SERVER_DISCOVER__( &KFClusterProxyModule::OnServerDiscoverClient );
        __REGISTER_TCP_SERVER_LOST__( &KFClusterProxyModule::OnServerLostClient );

        __REGISTER_TCP_SERVER_FORWARD__( &KFClusterProxyModule::ForwardToShard );
        __REGISTER_TCP_CLIENT_FORWARD__( &KFClusterProxyModule::ForwardToClient );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ, &KFClusterProxyModule::HandleClusterTokenToProxyReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_VERIFY_TO_PROXY_REQ, &KFClusterProxyModule::HandleClusterVerifyToProxyReq );
    }

    void KFClusterProxyModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_TCP_CLIENT_SHUTDOWN__();
        __UN_TCP_CLIENT_CONNECTION__();
        __UN_TCP_CLIENT_FORWARD__();

        __UN_TCP_SERVER_DISCOVER__();
        __UN_TCP_SERVER_LOST__();
        __UN_TCP_SERVER_FORWARD__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_TO_PROXY_REQ );
    }

    void KFClusterProxyModule::Run()
    {
        // 删除过期的token信息
        StringSet remove_list;
        auto now_time = KFGlobal::Instance()->_game_time;
        for ( auto& iter : _kf_token_list._objects )
        {
            auto token_data = iter.second;
            if ( token_data->_valid_time < now_time )
            {
                remove_list.insert( token_data->_token );
            }
        }

        if ( !remove_list.empty() )
        {
            for ( auto& token : remove_list )
            {
                _kf_token_list.Remove( token );
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFClusterProxyModule::OnClientConnectionServer )
    {
        auto global = KFGlobal::Instance();
        if ( global->_app_name != net_data->_name )
        {
            return;
        }

        if ( net_data->_type == __STRING__( master ) )
        {
            OnClientConnectionClusterMaster( net_data->_name, net_data->_id );
        }
        else if ( net_data->_type == __STRING__( shard ) )
        {
            OnClientConnectionClusterShard( net_data->_name, net_data->_id );
        }
    }

    void KFClusterProxyModule::OnClientConnectionClusterMaster( const std::string& servername, uint64 server_id )
    {
        // 注册定时器
        __LOOP_TIMER_1__( server_id, 5000, 1, &KFClusterProxyModule::OnTimerSendClusterRegisterMessage );
    }

    __KF_TIMER_FUNCTION__( KFClusterProxyModule::OnTimerSendClusterRegisterMessage )
    {
        if ( !_in_service )
        {
            return;
        }

        auto global = KFGlobal::Instance();

        KFMsg::S2SClusterRegisterToMasterReq req;
        auto listen_data = req.mutable_listen();

        listen_data->set_appname( global->_app_name );
        listen_data->set_apptype( global->_app_type );
        listen_data->set_appid( global->_app_id->GetId() );
        listen_data->set_ip( global->_internet_ip );
        listen_data->set_port( global->_listen_port );
        auto ok = _kf_tcp_client->SendNetMessage( objectid, KFMsg::S2S_CLUSTER_REGISTER_TO_MASTER_REQ, &req );
        if ( ok )
        {
            // 取消定时器
            __UN_TIMER_1__( objectid );
            __LOG_INFO__( "cluster proxy register to master ok" );
        }
    }

    void KFClusterProxyModule::OnClientConnectionClusterShard( const std::string& servername, uint64 server_id )
    {
        _in_service = true;
        _kf_hash.AddHashNode( servername, server_id, 100 );

        // 自己所有的连接注册到Cluster中
        NetDataList client_list;
        _kf_tcp_server->GetHandleList( client_list );

        KFMsg::S2SClusterClientDiscoverToShardReq req;
        for ( auto ip_address : client_list )
        {
            req.add_clientid( ip_address->_id );
        }
        _kf_tcp_client->SendNetMessage( server_id, KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterProxyModule::OnServerDiscoverClient )
    {
        // 判断是否有token验证
        for ( auto& iter : _kf_token_list._objects )
        {
            auto token_data = iter.second;
            if ( token_data->_client_id == net_data->_id )
            {
                // 启动定时器, 10秒钟内不验证, 关闭连接
                __COUNT_TIMER_1__( net_data->_id, 10000, 1, &KFClusterProxyModule::OnTimerClusterAuthTimeOut );
                return;
            }
        }

        // 没有则认为是非法的连接, 直接关闭
        _kf_tcp_server->CloseNetHandle( net_data->_id, 1000, __FUNC_LINE__ );
    }

    __KF_TIMER_FUNCTION__( KFClusterProxyModule::OnTimerClusterAuthTimeOut )
    {
        // 认证超时, 关闭连接
        _kf_tcp_server->CloseNetHandle( objectid, 1000, __FUNC_LINE__ );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterProxyModule::OnServerLostClient )
    {
        KFMsg::S2SClusterClientLostToShardReq req;
        req.set_clientid( net_data->_id );
        _kf_tcp_client->SendMessageToType( __STRING__( shard ), KFMsg::S2S_CLUSTER_CLIENT_LOST_TO_SHARD_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterProxyModule::OnClientLostServer )
    {
        auto global = KFGlobal::Instance();
        if ( global->_app_name != net_data->_name )
        {
            return;
        }

        if ( net_data->_type == __STRING__( master ) )
        {
            OnClientLostClusterMaster( net_data->_name, net_data->_id );
        }
        else if ( net_data->_type == __STRING__( shard ) )
        {
            OnClientLostClusterShard( net_data->_name, net_data->_id );
        }
    }

    void KFClusterProxyModule::OnClientLostClusterMaster( const std::string& servername, uint64 server_id )
    {
        __UN_TIMER_1__( server_id );
    }

    void KFClusterProxyModule::OnClientLostClusterShard( const std::string& servername, uint64 server_id )
    {
        _kf_hash.RemoveHashNode( server_id );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleClusterTokenToProxyReq, KFMsg::S2SClusterTokenToProxyReq )
    {
        auto token_data = _kf_token_list.Create( kfmsg->token() );
        token_data->_token = kfmsg->token();
        token_data->_client_id = kfmsg->clientid();
        token_data->_valid_time = KFGlobal::Instance()->_game_time + 60000;

        KFMsg::S2SClusterTokenToProxyAck ack;
        ack.set_token( kfmsg->token() );
        ack.set_proxyid( kfmsg->proxyid() );
        ack.set_clientid( kfmsg->clientid() );
        ack.set_masterid( kfmsg->masterid() );
        _kf_tcp_client->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_ACK, &ack );

        __LOG_DEBUG__( "update client[{}] token[{}]", KFAppId::ToString( token_data->_client_id ), token_data->_token );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleClusterVerifyToProxyReq, KFMsg::S2SClusterVerifyToProxyReq )
    {
        auto client_id = __ROUTE_SERVER_ID__;
        auto server_id = ClusterVerifyLogin( kfmsg->token(), kfmsg->serverid() );

        KFMsg::S2SClusterVerifyToClientAck ack;
        ack.set_serverid( server_id );
        _kf_tcp_server->SendNetMessage( client_id, KFMsg::S2S_CLUSTER_VERIFY_TO_CLIENT_ACK, &ack );

        if ( server_id == _invalid_int )
        {
            return __LOG_ERROR__( "cluster client[{}] verify failed", KFAppId::ToString( kfmsg->serverid() ) );
        }

        // 删除定时器
        __UN_TIMER_1__( server_id );

        // 通知shard
        KFMsg::S2SClusterClientDiscoverToShardReq req;
        req.add_clientid( server_id );
        _kf_tcp_client->SendMessageToType( __STRING__( shard ), KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ, &req );
        __LOG_DEBUG__( "cluster client [{}] verify ok", KFAppId::ToString( kfmsg->serverid() ) );
    }

    uint64 KFClusterProxyModule::ClusterVerifyLogin( const std::string& token, uint64 server_id )
    {
        auto token_data = _kf_token_list.Remove( token );
        if ( token_data == nullptr || token_data->_client_id != server_id )
        {
            return _invalid_int;
        }

        return server_id;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFClusterProxyModule::ForwardToShard( const Route& route, uint32 msg_id, const google::protobuf::Message* message )
    {
        auto data = message->SerializeAsString();
        return ForwardToShard( route, msg_id, data.data(), data.length() );
    }

    bool KFClusterProxyModule::ForwardToShard( const Route& route, uint32 msg_id, const char* data, uint32 length )
    {
        auto client_id = __ROUTE_SERVER_ID__;
        auto shard_id = __ROUTE_RECV_ID__;
        if ( shard_id == _invalid_int )
        {
            shard_id = _kf_hash.FindHashNode( client_id, true );
            if ( shard_id == _invalid_int )
            {
                __LOG_ERROR__( "msg_id[{}] forward failed", msg_id );
                return false;
            }
        }

        return _kf_tcp_client->SendNetMessage( shard_id, client_id, msg_id, data, length );
    }

    bool KFClusterProxyModule::ForwardToClient( const Route& route, uint32 msg_id, const google::protobuf::Message* message )
    {
        auto data = message->SerializeAsString();
        return ForwardToClient( route, msg_id, data.data(), data.length() );
    }

    bool KFClusterProxyModule::ForwardToClient( const Route& route, uint32 msg_id, const char* data, uint32 length )
    {
        auto client_id = __ROUTE_RECV_ID__;
        return _kf_tcp_server->SendNetMessage( client_id, msg_id, data, length );
    }
}