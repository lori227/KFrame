#include "KFClusterMasterModule.hpp"

namespace KFrame
{
    void KFClusterMasterModule::BeforeRun()
    {
        __REGISTER_TCP_SERVER_LOST__( &KFClusterMasterModule::OnServerLostClusterProxy );
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFClusterMasterModule::OnClientConnectClusterMaster );
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFClusterMasterModule::OnClientLostClusterMaster );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_REGISTER_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterRegisterToMasterReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_SYNC_PROXY_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterSyncProxyToMasterReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_AUTH_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterAuthToMasterReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_LOST_PROXY_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterLostProxyToMasterReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterTokenToMasterReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_ACK, &KFClusterMasterModule::HandleClusterTokenToProxyAck  );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_ACK, &KFClusterMasterModule::HandleClusterTokenToMasterAck );
    }

    void KFClusterMasterModule::BeforeShut()
    {
        __UN_TCP_SERVER_LOST__();
        __UN_TCP_CLIENT_CONNECTION__();
        __UN_TCP_CLIENT_SHUTDOWN__();
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_REGISTER_TO_MASTER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_SYNC_PROXY_TO_MASTER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_TO_MASTER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_LOST_PROXY_TO_MASTER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_ACK );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFClusterMasterModule::OnClientConnectClusterMaster )
    {
        if ( net_data->_type != __STRING__( master ) )
        {
            return;
        }

        KFMsg::S2SClusterSyncProxyToMasterReq req;
        for ( auto& iter : _kf_proxy_list._objects )
        {
            auto proxy_data = iter.second;
            if ( proxy_data->_master_id != KFGlobal::Instance()->_app_id->GetId() )
            {
                continue;
            }

            auto listen = req.add_listen();
            listen->set_appname( proxy_data->_name );
            listen->set_apptype( proxy_data->_type );
            listen->set_appid( proxy_data->_id );
            listen->set_ip( proxy_data->_ip );
            listen->set_port( proxy_data->_port );
        }
        _kf_tcp_client->SendNetMessage( net_data->_id, KFMsg::S2S_CLUSTER_SYNC_PROXY_TO_MASTER_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterMasterModule::OnClientLostClusterMaster )
    {
        if ( net_data->_type != __STRING__( master ) )
        {
            return;
        }

        UInt64Set remove_list;
        for ( auto& iter : _kf_proxy_list._objects )
        {
            auto proxy_data = iter.second;
            if ( proxy_data->_master_id == net_data->_id )
            {
                remove_list.insert( proxy_data->_id );
            }
        }

        for ( auto proxy_id : remove_list )
        {
            _kf_proxy_list.Remove( proxy_id );
            _proxy_hash.RemoveHashNode( proxy_id );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterMasterModule::OnServerLostClusterProxy )
    {
        if ( net_data->_type != __STRING__( proxy ) )
        {
            return;
        }

        _kf_proxy_list.Remove( net_data->_id );
        _proxy_hash.RemoveHashNode( net_data->_id );

        // 发送给其他master服务器
        KFMsg::S2SClusterLostProxyToMasterReq req;
        req.set_proxyid( net_data->_id );
        _kf_tcp_client->SendMessageToType( KFGlobal::Instance()->_app_type, KFMsg::S2S_CLUSTER_LOST_PROXY_TO_MASTER_REQ, &req );

        __LOG_ERROR__( "[{}|{}:{}] lost", net_data->_name, net_data->_type, net_data->_str_id );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterLostProxyToMasterReq, KFMsg::S2SClusterLostProxyToMasterReq )
    {
        _kf_proxy_list.Remove( kfmsg->proxyid() );
        _proxy_hash.RemoveHashNode( kfmsg->proxyid() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterRegisterToMasterReq, KFMsg::S2SClusterRegisterToMasterReq )
    {
        auto listen = &kfmsg->listen();

        auto proxy_data = _kf_proxy_list.Create( listen->appid() );
        proxy_data->_name = listen->appname();
        proxy_data->_type = listen->apptype();
        proxy_data->_id = listen->appid();
        proxy_data->_ip = listen->ip();
        proxy_data->_port = listen->port();
        proxy_data->_master_id = KFGlobal::Instance()->_app_id->GetId();

        _proxy_hash.AddHashNode( listen->appname(), listen->appid(), 100 );

        // 发送给其他master服务器
        KFMsg::S2SClusterSyncProxyToMasterReq req;
        req.add_listen()->CopyFrom( *listen );
        _kf_tcp_client->SendMessageToType( KFGlobal::Instance()->_app_type, KFMsg::S2S_CLUSTER_SYNC_PROXY_TO_MASTER_REQ, &req );

        __LOG_INFO__( "add cluster proxy[{}|{}:{}]", KFAppId::ToString( listen->appid() ), listen->ip(), listen->port() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterSyncProxyToMasterReq, KFMsg::S2SClusterSyncProxyToMasterReq )
    {
        for ( auto i = 0; i < kfmsg->listen_size(); ++i )
        {
            auto listen = &kfmsg->listen( i );

            auto proxy_data = _kf_proxy_list.Create( listen->appid() );
            proxy_data->_name = listen->appname();
            proxy_data->_type = listen->apptype();
            proxy_data->_id = listen->appid();
            proxy_data->_ip = listen->ip();
            proxy_data->_port = listen->port();
            proxy_data->_master_id = __ROUTE_SERVER_ID__;
            _proxy_hash.AddHashNode( listen->appname(), listen->appid(), 100 );

            __LOG_INFO__( "add cluster proxy[{}|{}:{}]", KFAppId::ToString( listen->appid() ), listen->ip(), listen->port() );
        }
    }

    std::shared_ptr<KFProxyData> KFClusterMasterModule::SelectProxyServer( uint64 client_id )
    {
        auto proxy_id = _proxy_hash.FindHashNode( client_id );
        if ( proxy_id == _invalid_int )
        {
            return nullptr;
        }

        return _kf_proxy_list.Find( proxy_id );
    }

    std::string KFClusterMasterModule::CreateToken( uint64 client_id )
    {
        auto uuid = KFGlobal::Instance()->STMakeUuid();
        std::string md5source = __FORMAT__( "{}:{}", uuid, client_id );

        auto token = KFCrypto::Md5Encode( md5source );
        _token_list[ client_id ] = token;

        return token;
    }

    std::string KFClusterMasterModule::FindToken( uint64 client_id )
    {
        auto iter = _token_list.find( client_id );
        if ( iter == _token_list.end() )
        {
            return _invalid_string;
        }

        auto token = iter->second;
        _token_list.erase( iter );
        return token;
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterAuthToMasterReq, KFMsg::S2SClusterAuthToMasterReq )
    {
        auto str_client_id = KFAppId::ToString( kfmsg->clientid() );
        __LOG_DEBUG__( "cluster client[{}] auth req", str_client_id );

        static auto _cluster_key_constant = KFGlobal::Instance()->FindConstant( __STRING__( clusterkey ) );
        if ( kfmsg->clusterkey() != _cluster_key_constant->_str_value )
        {
            return __LOG_ERROR__( "cluster client[{}] key[{}!={}] error", str_client_id, kfmsg->clusterkey(), _cluster_key_constant->_str_value );
        }

        auto proxy_data = SelectProxyServer( kfmsg->clientid() ) ;
        if ( proxy_data == nullptr )
        {
            return __LOG_ERROR__( "cluster client[{}] no proxy error", str_client_id );
        }

        __LOG_DEBUG__( "cluster client[{}] auth proxy[{}] ok", str_client_id, KFAppId::ToString( proxy_data->_id ) );

        // 创建token
        auto token = CreateToken( kfmsg->clientid() );
        if ( proxy_data->_master_id == KFGlobal::Instance()->_app_id->GetId() )
        {
            // Token发送给ProxyServer
            KFMsg::S2SClusterTokenToProxyReq req;
            req.set_token( token );
            req.set_proxyid( proxy_data->_id );
            req.set_clientid( kfmsg->clientid() );
            req.set_masterid( KFGlobal::Instance()->_app_id->GetId() );
            _kf_tcp_server->SendNetMessage( proxy_data->_id, KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ, &req );
        }
        else
        {
            // 转发到其他master
            KFMsg::S2SClusterTokenToMasterReq req;
            req.set_token( token );
            req.set_clientid( kfmsg->clientid() );
            req.set_proxyid( proxy_data->_id );
            req.set_masterid( KFGlobal::Instance()->_app_id->GetId() );
            _kf_tcp_server->SendNetMessage( proxy_data->_master_id, KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterTokenToMasterReq, KFMsg::S2SClusterTokenToMasterReq )
    {
        // Token发送给ProxyServer
        KFMsg::S2SClusterTokenToProxyReq req;
        req.set_token( kfmsg->token() );
        req.set_proxyid( kfmsg->proxyid() );
        req.set_clientid( kfmsg->clientid() );
        req.set_masterid( kfmsg->masterid() );
        _kf_tcp_server->SendNetMessage( kfmsg->proxyid(), KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterTokenToProxyAck, KFMsg::S2SClusterTokenToProxyAck )
    {
        if ( kfmsg->masterid() == KFGlobal::Instance()->_app_id->GetId() )
        {
            auto token = FindToken( kfmsg->clientid() );
            if ( token != kfmsg->token() )
            {
                return __LOG_ERROR__( "client[{}] token error", KFAppId::ToString( kfmsg->clientid() ) );
            }

            auto proxy_data = _kf_proxy_list.Find( kfmsg->proxyid() );
            if ( proxy_data == nullptr )
            {
                return __LOG_ERROR__( "client[{}] can't find proxy[{}]!", KFAppId::ToString( kfmsg->clientid() ), KFAppId::ToString( kfmsg->proxyid() ) );
            }

            // 发送给client
            KFMsg::S2SClusterAuthToClientAck ack;
            ack.set_token( token );

            auto listen = ack.mutable_listen();
            listen->set_appname( proxy_data->_name );
            listen->set_apptype( proxy_data->_type );
            listen->set_appid( proxy_data->_id );
            listen->set_ip( proxy_data->_ip );
            listen->set_port( proxy_data->_port );
            _kf_tcp_server->SendNetMessage( kfmsg->clientid(), KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK, &ack );
            _kf_tcp_server->CloseNetHandle( kfmsg->clientid(), 2000, __FUNC_LINE__ );
        }
        else
        {
            KFMsg::S2SClusterTokenToMasterAck ack;
            ack.set_token( kfmsg->token() );
            ack.set_clientid( kfmsg->clientid() );
            ack.set_proxyid( kfmsg->proxyid() );
            _kf_tcp_client->SendNetMessage( kfmsg->masterid(), KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_ACK, &ack );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterTokenToMasterAck, KFMsg::S2SClusterTokenToMasterAck )
    {
        auto token = FindToken( kfmsg->clientid() );
        if ( token != kfmsg->token() )
        {
            return __LOG_ERROR__( "client[{}] token error", KFAppId::ToString( kfmsg->clientid() ) );
        }

        auto proxy_data = _kf_proxy_list.Find( kfmsg->proxyid() );
        if ( proxy_data == nullptr )
        {
            return __LOG_ERROR__( "client[{}] can't find proxy[{}]!", KFAppId::ToString( kfmsg->clientid() ), KFAppId::ToString( kfmsg->proxyid() ) );
        }

        // 发送给client
        KFMsg::S2SClusterAuthToClientAck ack;
        ack.set_token( token );

        auto listen = ack.mutable_listen();
        listen->set_appname( proxy_data->_name );
        listen->set_apptype( proxy_data->_type );
        listen->set_appid( proxy_data->_id );
        listen->set_ip( proxy_data->_ip );
        listen->set_port( proxy_data->_port );
        _kf_tcp_server->SendNetMessage( kfmsg->clientid(), KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK, &ack );
        _kf_tcp_server->CloseNetHandle( kfmsg->clientid(), 2000, __FUNC_LINE__ );
    }
}