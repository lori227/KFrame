#include "KFClusterMasterModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFClusterMasterModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFClusterMasterModule::OnServerLostClusterProxy );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFClusterMasterModule::OnClientConnectClusterMaster );
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFClusterMasterModule::OnClientLostClusterMaster );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_REGISTER_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterRegisterToMasterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_SYNC_PROXY_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterSyncProxyToMasterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterAuthToMasterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_LOST_PROXY_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterLostProxyToMasterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterTokenToMasterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_ACK, &KFClusterMasterModule::HandleClusterTokenToProxyAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_ACK, &KFClusterMasterModule::HandleClusterTokenToMasterAck );
    }

    void KFClusterMasterModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_REGISTER_TO_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_SYNC_PROXY_TO_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_TO_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_LOST_PROXY_TO_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFClusterMasterModule::OnClientConnectClusterMaster )
    {
        if ( netdata->_type != __KF_STRING__( master ) )
        {
            return;
        }

        KFMsg::S2SClusterSyncProxyToMasterReq req;
        for ( auto& iter : _kf_proxy_list._objects )
        {
            auto kfproxy = iter.second;
            if ( kfproxy->_master_id != KFGlobal::Instance()->_app_id->GetId() )
            {
                continue;
            }

            auto listen = req.add_listen();
            listen->set_appname( kfproxy->_name );
            listen->set_apptype( kfproxy->_type );
            listen->set_appid( kfproxy->_id );
            listen->set_ip( kfproxy->_ip );
            listen->set_port( kfproxy->_port );
        }
        _kf_tcp_client->SendNetMessage( netdata->_id, KFMsg::S2S_CLUSTER_SYNC_PROXY_TO_MASTER_REQ, &req );
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterMasterModule::OnClientLostClusterMaster )
    {
        if ( netdata->_type != __KF_STRING__( master ) )
        {
            return;
        }

        std::set< uint64 > removes;
        for ( auto& iter : _kf_proxy_list._objects )
        {
            auto kfproxy = iter.second;
            if ( kfproxy->_master_id == netdata->_id )
            {
                removes.insert( kfproxy->_id );
            }
        }

        for ( auto proxyid : removes )
        {
            _kf_proxy_list.Remove( proxyid );
            _proxy_hash.RemoveHashNode( proxyid );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterMasterModule::OnServerLostClusterProxy )
    {
        if ( netdata->_type != __KF_STRING__( proxy ) )
        {
            return;
        }

        _kf_proxy_list.Remove( netdata->_id );
        _proxy_hash.RemoveHashNode( netdata->_id );

        // 发送给其他master服务器
        KFMsg::S2SClusterLostProxyToMasterReq req;
        req.set_proxyid( netdata->_id );
        _kf_tcp_client->SendMessageToType( KFGlobal::Instance()->_app_type, KFMsg::S2S_CLUSTER_LOST_PROXY_TO_MASTER_REQ, &req );

        __LOG_ERROR__( "[{}|{}:{}] lost!", netdata->_name, netdata->_type, netdata->_str_id );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterLostProxyToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterLostProxyToMasterReq );

        _kf_proxy_list.Remove( kfmsg.proxyid() );
        _proxy_hash.RemoveHashNode( kfmsg.proxyid() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterRegisterToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterRegisterToMasterReq );

        auto listen = &kfmsg.listen();

        auto kfproxy = _kf_proxy_list.Create( listen->appid() );
        kfproxy->_name = listen->appname();
        kfproxy->_type = listen->apptype();
        kfproxy->_id = listen->appid();
        kfproxy->_ip = listen->ip();
        kfproxy->_port = listen->port();
        kfproxy->_master_id = KFGlobal::Instance()->_app_id->GetId();

        _proxy_hash.AddHashNode( listen->appname(), listen->appid(), 100 );

        // 发送给其他master服务器
        KFMsg::S2SClusterSyncProxyToMasterReq req;
        req.add_listen()->CopyFrom( *listen );
        _kf_tcp_client->SendMessageToType( KFGlobal::Instance()->_app_type, KFMsg::S2S_CLUSTER_SYNC_PROXY_TO_MASTER_REQ, &req );

        __LOG_INFO__( "add cluster proxy[{}|{}:{}]", KFAppId::ToString( listen->appid() ), listen->ip(), listen->port() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterSyncProxyToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterSyncProxyToMasterReq );

        for ( auto i = 0; i < kfmsg.listen_size(); ++i )
        {
            auto listen = &kfmsg.listen( i );

            auto kfproxy = _kf_proxy_list.Create( listen->appid() );
            kfproxy->_name = listen->appname();
            kfproxy->_type = listen->apptype();
            kfproxy->_id = listen->appid();
            kfproxy->_ip = listen->ip();
            kfproxy->_port = listen->port();
            kfproxy->_master_id = __ROUTE_SERVER_ID__;
            _proxy_hash.AddHashNode( listen->appname(), listen->appid(), 100 );

            __LOG_INFO__( "add cluster proxy[{}|{}:{}]", KFAppId::ToString( listen->appid() ), listen->ip(), listen->port() );
        }
    }

    KFProxyData* KFClusterMasterModule::SelectProxyServer( uint64 clientid )
    {
        auto proxyid = _proxy_hash.FindHashNode( clientid );
        if ( proxyid == _invalid_int )
        {
            return nullptr;
        }

        return _kf_proxy_list.Find( proxyid );
    }

    std::string KFClusterMasterModule::CreateToken( uint64 clientid )
    {
        auto id = KFGlobal::Instance()->MakeUUID();
        std::string md5source = __FORMAT__( "{}:{}", id, clientid );

        auto token = KFCrypto::Md5Encode( md5source );
        _token_list[ clientid ] = token;

        return token;
    }

    std::string KFClusterMasterModule::FindToken( uint64 clientid )
    {
        auto iter = _token_list.find( clientid );
        if ( iter == _token_list.end() )
        {
            return _invalid_str;
        }

        auto token = iter->second;
        _token_list.erase( iter );
        return token;
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterAuthToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterAuthToMasterReq );

        auto strclientid = KFAppId::ToString( kfmsg.clientid() );
        __LOG_DEBUG__( "cluster client[{}] auth req!", strclientid );

        static auto* _option = _kf_option->FindOption( __KF_STRING__( clusterkey ) );
        if ( kfmsg.clusterkey() != _option->_str_value )
        {
            return __LOG_ERROR__( "cluster client[{}] key[{}!={}] error!", strclientid, kfmsg.clusterkey(), _option->_str_value );
        }

        auto kfproxy = SelectProxyServer( kfmsg.clientid() ) ;
        if ( kfproxy == nullptr )
        {
            return __LOG_ERROR__( "cluster client[{}] no proxy error!", strclientid );
        }

        __LOG_DEBUG__( "cluster client[{}] auth proxy[{}] ok!", strclientid, KFAppId::ToString( kfproxy->_id ) );

        // 创建token
        auto token = CreateToken( kfmsg.clientid() );
        if ( kfproxy->_master_id == KFGlobal::Instance()->_app_id->GetId() )
        {
            // Token发送给ProxyServer
            KFMsg::S2SClusterTokenToProxyReq req;
            req.set_token( token );
            req.set_proxyid( kfproxy->_id );
            req.set_clientid( kfmsg.clientid() );
            req.set_masterid( KFGlobal::Instance()->_app_id->GetId() );
            _kf_tcp_server->SendNetMessage( kfproxy->_id, KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ, &req );
        }
        else
        {
            // 转发到其他master
            KFMsg::S2SClusterTokenToMasterReq req;
            req.set_token( token );
            req.set_clientid( kfmsg.clientid() );
            req.set_proxyid( kfproxy->_id );
            req.set_masterid( KFGlobal::Instance()->_app_id->GetId() );
            _kf_tcp_server->SendNetMessage( kfproxy->_master_id, KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterTokenToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterTokenToMasterReq );

        // Token发送给ProxyServer
        KFMsg::S2SClusterTokenToProxyReq req;
        req.set_token( kfmsg.token() );
        req.set_proxyid( kfmsg.proxyid() );
        req.set_clientid( kfmsg.clientid() );
        req.set_masterid( kfmsg.masterid() );
        _kf_tcp_server->SendNetMessage( kfmsg.proxyid(), KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterTokenToProxyAck )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterTokenToProxyAck );

        if ( kfmsg.masterid() == KFGlobal::Instance()->_app_id->GetId() )
        {
            auto token = FindToken( kfmsg.clientid() );
            if ( token != kfmsg.token() )
            {
                return __LOG_ERROR__( "client[{}] token error!", KFAppId::ToString( kfmsg.clientid() ) );
            }

            auto kfproxy = _kf_proxy_list.Find( kfmsg.proxyid() );
            if ( kfproxy == nullptr )
            {
                return __LOG_ERROR__( "client[{}] can't find proxy[{}]!!", KFAppId::ToString( kfmsg.clientid() ), KFAppId::ToString( kfmsg.proxyid() ) );
            }

            // 发送给client
            KFMsg::S2SClusterAuthToClientAck ack;
            ack.set_token( token );

            auto listen = ack.mutable_listen();
            listen->set_appname( kfproxy->_name );
            listen->set_apptype( kfproxy->_type );
            listen->set_appid( kfproxy->_id );
            listen->set_ip( kfproxy->_ip );
            listen->set_port( kfproxy->_port );
            _kf_tcp_server->SendNetMessage( kfmsg.clientid(), KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK, &ack );
            _kf_tcp_server->CloseNetHandle( kfmsg.clientid(), 2000, __FUNC_LINE__ );
        }
        else
        {
            KFMsg::S2SClusterTokenToMasterAck ack;
            ack.set_token( kfmsg.token() );
            ack.set_clientid( kfmsg.clientid() );
            ack.set_proxyid( kfmsg.proxyid() );
            _kf_tcp_client->SendNetMessage( kfmsg.masterid(), KFMsg::S2S_CLUSTER_TOKEN_TO_MASTER_ACK, &ack );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterTokenToMasterAck )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterTokenToMasterAck );

        auto token = FindToken( kfmsg.clientid() );
        if ( token != kfmsg.token() )
        {
            return __LOG_ERROR__( "client[{}] token error!", KFAppId::ToString( kfmsg.clientid() ) );
        }

        auto kfproxy = _kf_proxy_list.Find( kfmsg.proxyid() );
        if ( kfproxy == nullptr )
        {
            return __LOG_ERROR__( "client[{}] can't find proxy[{}]!!", KFAppId::ToString( kfmsg.clientid() ), KFAppId::ToString( kfmsg.proxyid() ) );
        }

        // 发送给client
        KFMsg::S2SClusterAuthToClientAck ack;
        ack.set_token( token );

        auto listen = ack.mutable_listen();
        listen->set_appname( kfproxy->_name );
        listen->set_apptype( kfproxy->_type );
        listen->set_appid( kfproxy->_id );
        listen->set_ip( kfproxy->_ip );
        listen->set_port( kfproxy->_port );
        _kf_tcp_server->SendNetMessage( kfmsg.clientid(), KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK, &ack );
        _kf_tcp_server->CloseNetHandle( kfmsg.clientid(), 2000, __FUNC_LINE__ );
    }
}