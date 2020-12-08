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

        __REGISTER_TCP_CLIENT_TRANSPOND__( &KFClusterProxyModule::TranspondToClient );
        __REGISTER_TCP_SERVER_TRANSPOND__( &KFClusterProxyModule::TranspondToShard );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFClusterProxyModule, KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ, KFMsg::S2SClusterTokenToProxyReq, HandleClusterTokenToProxyReq );
        __REGISTER_MESSAGE__( KFClusterProxyModule, KFMsg::S2S_CLUSTER_VERIFY_TO_PROXY_REQ, KFMsg::S2SClusterVerifyToProxyReq, HandleClusterVerifyToProxyReq );
    }

    void KFClusterProxyModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_TCP_CLIENT_SHUTDOWN__();
        __UN_TCP_CLIENT_CONNECTION__();
        __UN_TCP_CLIENT_TRANSPOND__();

        __UN_TCP_SERVER_DISCOVER__();
        __UN_TCP_SERVER_LOST__();
        __UN_TCP_SERVER_TRANSPOND__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_TO_PROXY_REQ );
    }

    void KFClusterProxyModule::Run()
    {
        // 删除过期的token信息
        StringSet removelist;
        auto nowtime = KFGlobal::Instance()->_game_time;
        for ( auto& iter : _kf_token_list._objects )
        {
            auto kftoken = iter.second;
            if ( kftoken->_valid_time < nowtime )
            {
                removelist.insert( kftoken->_token );
            }
        }

        if ( !removelist.empty() )
        {
            for ( auto& token : removelist )
            {
                _kf_token_list.Remove( token );
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFClusterProxyModule::OnClientConnectionServer )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_name != netdata->_name )
        {
            return;
        }

        if ( netdata->_type == __STRING__( master ) )
        {
            OnClientConnectionClusterMaster( netdata->_name, netdata->_id );
        }
        else if ( netdata->_type == __STRING__( shard ) )
        {
            OnClientConnectionClusterShard( netdata->_name, netdata->_id );
        }
    }

    void KFClusterProxyModule::OnClientConnectionClusterMaster( const std::string& servername, uint64 serverid )
    {
        // 注册定时器
        __LOOP_TIMER_1__( serverid, 5000, 1, &KFClusterProxyModule::OnTimerSendClusterRegisterMessage );
    }

    __KF_TIMER_FUNCTION__( KFClusterProxyModule::OnTimerSendClusterRegisterMessage )
    {
        if ( !_in_service )
        {
            return;
        }

        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SClusterRegisterToMasterReq req;
        auto listendata = req.mutable_listen();

        listendata->set_appname( kfglobal->_app_name );
        listendata->set_apptype( kfglobal->_app_type );
        listendata->set_appid( kfglobal->_app_id->GetId() );
        listendata->set_ip( kfglobal->_interanet_ip );
        listendata->set_port( kfglobal->_listen_port );
        auto ok = _kf_tcp_client->SendNetMessage( objectid, KFMsg::S2S_CLUSTER_REGISTER_TO_MASTER_REQ, &req );
        if ( ok )
        {
            // 取消定时器
            __UN_TIMER_1__( objectid );
            __LOG_INFO__( "cluster proxy register to master ok" );
        }
    }

    void KFClusterProxyModule::OnClientConnectionClusterShard( const std::string& servername, uint64 serverid )
    {
        _in_service = true;
        _kf_hash.AddHashNode( servername, serverid, 100 );

        // 自己所有的连接注册到Cluster中
        NetDataList clientlist;
        _kf_tcp_server->GetHandleList( clientlist );

        KFMsg::S2SClusterClientDiscoverToShardReq req;
        for ( auto ipaddress : clientlist )
        {
            req.add_clientid( ipaddress->_id );
        }
        _kf_tcp_client->SendNetMessage( serverid, KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterProxyModule::OnServerDiscoverClient )
    {
        // 判断是否有token验证
        for ( auto& iter : _kf_token_list._objects )
        {
            auto kftoken = iter.second;
            if ( kftoken->_client_id == netdata->_id )
            {
                // 启动定时器, 10秒钟内不验证, 关闭连接
                __LIMIT_TIMER_1__( netdata->_id, 10000, 1, &KFClusterProxyModule::OnTimerClusterAuthTimeOut );
                return;
            }
        }

        // 没有则认为是非法的连接, 直接关闭
        _kf_tcp_server->CloseNetHandle( netdata->_id, 1000, __FUNC_LINE__ );
    }

    __KF_TIMER_FUNCTION__( KFClusterProxyModule::OnTimerClusterAuthTimeOut )
    {
        // 认证超时, 关闭连接
        _kf_tcp_server->CloseNetHandle( objectid, 1000, __FUNC_LINE__ );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterProxyModule::OnServerLostClient )
    {
        KFMsg::S2SClusterClientLostToShardReq req;
        req.set_clientid( netdata->_id );
        _kf_tcp_client->SendMessageToType( __STRING__( shard ), KFMsg::S2S_CLUSTER_CLIENT_LOST_TO_SHARD_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterProxyModule::OnClientLostServer )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_name != netdata->_name )
        {
            return;
        }

        if ( netdata->_type == __STRING__( master ) )
        {
            OnClientLostClusterMaster( netdata->_name, netdata->_id );
        }
        else if ( netdata->_type == __STRING__( shard ) )
        {
            OnClientLostClusterShard( netdata->_name, netdata->_id );
        }
    }

    void KFClusterProxyModule::OnClientLostClusterMaster( const std::string& servername, uint64 serverid )
    {
        __UN_TIMER_1__( serverid );
    }

    void KFClusterProxyModule::OnClientLostClusterShard( const std::string& servername, uint64 serverid )
    {
        _kf_hash.RemoveHashNode( serverid );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleClusterTokenToProxyReq, KFMsg::S2SClusterTokenToProxyReq )
    {
        auto kftoken = _kf_token_list.Create( kfmsg->token() );
        kftoken->_token = kfmsg->token();
        kftoken->_client_id = kfmsg->clientid();
        kftoken->_valid_time = KFGlobal::Instance()->_game_time + 60000;

        KFMsg::S2SClusterTokenToProxyAck ack;
        ack.set_token( kfmsg->token() );
        ack.set_proxyid( kfmsg->proxyid() );
        ack.set_clientid( kfmsg->clientid() );
        ack.set_masterid( kfmsg->masterid() );
        _kf_tcp_client->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_ACK, &ack );

        __LOG_DEBUG__( "update client[{}] token[{}]", KFAppId::ToString( kftoken->_client_id ), kftoken->_token );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleClusterVerifyToProxyReq, KFMsg::S2SClusterVerifyToProxyReq )
    {
        auto clientid = __ROUTE_SERVER_ID__;
        auto serverid = ClusterVerifyLogin( kfmsg->token(), kfmsg->serverid() );

        KFMsg::S2SClusterVerifyToClientAck ack;
        ack.set_serverid( serverid );
        _kf_tcp_server->SendNetMessage( clientid, KFMsg::S2S_CLUSTER_VERIFY_TO_CLIENT_ACK, &ack );

        if ( serverid == _invalid_int )
        {
            return __LOG_ERROR__( "cluster client[{}] verify failed", KFAppId::ToString( kfmsg->serverid() ) );
        }

        // 删除定时器
        __UN_TIMER_1__( serverid );

        // 通知shard
        KFMsg::S2SClusterClientDiscoverToShardReq req;
        req.add_clientid( serverid );
        _kf_tcp_client->SendMessageToType( __STRING__( shard ), KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ, &req );
        __LOG_DEBUG__( "cluster client [{}] verify ok", KFAppId::ToString( kfmsg->serverid() ) );
    }

    uint64 KFClusterProxyModule::ClusterVerifyLogin( const std::string& token, uint64 serverid )
    {
        auto kftoken = _kf_token_list.Find( token );
        if ( kftoken == nullptr )
        {
            return _invalid_int;
        }

        auto clientid = kftoken->_client_id;
        _kf_token_list.Remove( token );

        if ( clientid != serverid )
        {
            return _invalid_int;
        }

        return serverid;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFClusterProxyModule::TranspondToShard( const Route& route, uint32 msgid, const google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return TranspondToShard( route, msgid, strdata.data(), strdata.length() );
    }

    bool KFClusterProxyModule::TranspondToShard( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        auto clientid = __ROUTE_SERVER_ID__;
        auto shardid = __ROUTE_RECV_ID__;
        if ( shardid == _invalid_int )
        {
            shardid = _kf_hash.FindHashNode( clientid, true );
            if ( shardid == _invalid_int )
            {
                __LOG_ERROR__( "msgid[{}] transpond failed", msgid );
                return false;
            }
        }

        return _kf_tcp_client->SendNetMessage( shardid, clientid, msgid, data, length );
    }

    bool KFClusterProxyModule::TranspondToClient( const Route& route, uint32 msgid, const google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return TranspondToClient( route, msgid, strdata.data(), strdata.length() );
    }

    bool KFClusterProxyModule::TranspondToClient( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        auto clientid = __ROUTE_RECV_ID__;
        return _kf_tcp_server->SendNetMessage( clientid, msgid, data, length );
    }
}