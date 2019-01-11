#include "KFClusterProxyModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFClusterProxyModule::BeforeRun()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFClusterProxyModule::OnClientLostServer );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFClusterProxyModule::OnClientConnectionServer );
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFClusterProxyModule::OnServerDiscoverClient );
        __REGISTER_SERVER_LOST_FUNCTION__( &KFClusterProxyModule::OnServerLostClient );
        __REGISTER_CLIENT_TRANSMIT_FUNCTION__( &KFClusterProxyModule::TransmitMessageToClient );
        __REGISTER_SERVER_TRANSMIT_FUNCTION__( &KFClusterProxyModule::TransmitMessageToShard );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ, &KFClusterProxyModule::HandleClusterTokenToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_TO_PROXY_REQ, &KFClusterProxyModule::HandleClusterVerifyToProxyReq );
    }

    void KFClusterProxyModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_CLIENT_TRANSMIT_FUNCTION__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_SERVER_TRANSMIT_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_TO_PROXY_REQ );
    }

    void KFClusterProxyModule::Run()
    {
        // 删除过期的token信息
        std::set< std::string > removelist;
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
    __KF_CLIENT_CONNECT_FUNCTION__( KFClusterProxyModule::OnClientConnectionServer )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_name != servername )
        {
            return;
        }

        if ( servertype == __KF_STRING__( master ) )
        {
            OnClientConnectionClusterMaster( servername, serverid );
        }
        else if ( servertype == __KF_STRING__( shard ) )
        {
            OnClientConnectionClusterShard( servername, serverid );
        }
    }

    void KFClusterProxyModule::OnClientConnectionClusterMaster( const std::string& servername, uint64 serverid )
    {
        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SClusterRegisterToMasterReq req;
        req.set_name( kfglobal->_app_name );
        req.set_type( kfglobal->_app_type );
        req.set_id( kfglobal->_app_id._union._id );
        req.set_ip( kfglobal->_interanet_ip );
        req.set_port( kfglobal->_listen_port );
        _kf_tcp_client->SendNetMessage( serverid, KFMsg::S2S_CLUSTER_REGISTER_TO_MASTER_REQ, &req );

        // 注册定时器
        __REGISTER_LOOP_TIMER__( serverid, 5000, &KFClusterProxyModule::OnTimerSendClusterUpdateMessage );
    }

    __KF_TIMER_FUNCTION__( KFClusterProxyModule::OnTimerSendClusterUpdateMessage )
    {
        if ( !_in_service )
        {
            return;
        }

        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SClusterUpdateToMasterReq req;
        req.set_name( kfglobal->_app_name );
        req.set_type( kfglobal->_app_type );
        req.set_id( kfglobal->_app_id._union._id );
        req.set_ip( kfglobal->_interanet_ip );
        req.set_port( kfglobal->_listen_port );
        req.set_count( _kf_tcp_server->GetHandleCount() );
        _kf_tcp_client->SendNetMessage( objectid, KFMsg::S2S_CLUSTER_UPDATE_TO_MASTER_REQ, &req );
    }

    void KFClusterProxyModule::OnClientConnectionClusterShard( const std::string& servername, uint64 serverid )
    {
        _in_service = true;
        _kf_hash.AddHashNode( servername, serverid, 100 );

        // 自己所有的连接注册到Cluster中
        std::list< uint64 > clientlist;
        _kf_tcp_server->GetHandleList( clientlist );

        KFMsg::S2SClusterClientDiscoverToShardReq req;
        for ( auto clientid : clientlist )
        {
            req.add_clientid( clientid );
        }
        _kf_tcp_client->SendNetMessage( serverid, KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ, &req );
    }

    __KF_SERVER_DISCOVER_FUNCTION__( KFClusterProxyModule::OnServerDiscoverClient )
    {
        // 判断是否有token验证
        for ( auto& iter : _kf_token_list._objects )
        {
            auto kftoken = iter.second;
            if ( kftoken->_client_id == handleid )
            {
                // 启动定时器, 10秒钟内不验证, 关闭连接
                __REGISTER_LIMIT_TIMER__( handleid, 10000, 1, &KFClusterProxyModule::OnTimerClusterAuthTimeOut );
                return;
            }
        }

        // 没有则认为是非法的连接, 直接关闭
        _kf_tcp_client->CloseClient( handleid, __FUNC_LINE__ );
    }

    __KF_TIMER_FUNCTION__( KFClusterProxyModule::OnTimerClusterAuthTimeOut )
    {
        // 认证超时, 关闭连接
        _kf_tcp_client->CloseClient( objectid, __FUNC_LINE__ );
    }

    __KF_SERVER_LOST_FUNCTION__( KFClusterProxyModule::OnServerLostClient )
    {
        KFMsg::S2SClusterClientLostToShardReq req;
        req.set_clientid( handleid );
        _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), KFMsg::S2S_CLUSTER_CLIENT_LOST_TO_SHARD_REQ, &req );
    }

    __KF_CLIENT_LOST_FUNCTION__( KFClusterProxyModule::OnClientLostServer )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( kfglobal->_app_name != servername )
        {
            return;
        }

        if ( servertype == __KF_STRING__( master ) )
        {
            OnClientLostClusterMaster( servername, serverid );
        }
        else if ( servertype == __KF_STRING__( shard ) )
        {
            OnClientLostClusterShard( servername, serverid );
        }
    }

    void KFClusterProxyModule::OnClientLostClusterMaster( const std::string& servername, uint64 serverid )
    {
        __UNREGISTER_OBJECT_TIMER__( serverid );
    }

    void KFClusterProxyModule::OnClientLostClusterShard( const std::string& servername, uint64 serverid )
    {
        _kf_hash.RemoveHashNode( serverid );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleClusterTokenToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterTokenToProxyReq );

        auto kftoken = _kf_token_list.Create( kfmsg.token() );
        kftoken->_token = kfmsg.token();
        kftoken->_client_id = kfmsg.clientid();
        kftoken->_valid_time = KFGlobal::Instance()->_game_time + 60000;

        __LOG_DEBUG__( "update client[{}] token[{}]!", KFAppID::ToString( kftoken->_client_id ), kftoken->_token );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleClusterVerifyToProxyReq )
    {
        auto clientid = __ROUTE_SERVER_ID__;
        __PROTO_PARSE__( KFMsg::S2SClusterVerifyToProxyReq );

        auto serverid = ClusterVerifyLogin( kfmsg.token(), kfmsg.serverid() );

        KFMsg::S2SClusterVerifyToClientAck ack;
        ack.set_serverid( serverid );
        _kf_tcp_server->SendNetMessage( clientid, KFMsg::S2S_CLUSTER_VERIFY_TO_CLIENT_ACK, &ack );

        if ( serverid == _invalid_int )
        {
            return __LOG_ERROR__( "cluster client[{}] verify failed!", KFAppID::ToString( kfmsg.serverid() ) );
        }

        // 删除定时器
        __UNREGISTER_OBJECT_TIMER__( serverid );

        // 通知shard
        KFMsg::S2SClusterClientDiscoverToShardReq req;
        req.add_clientid( serverid );
        _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), KFMsg::S2S_CLUSTER_CLIENT_DISCOVER_TO_SHARD_REQ, &req );
        __LOG_DEBUG__( "cluster client [{}] verify ok!", KFAppID::ToString( kfmsg.serverid() ) );
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
    __KF_TRANSMIT_MESSAGE_FUNCTION__( KFClusterProxyModule::TransmitMessageToShard )
    {
        auto clientid = __ROUTE_SERVER_ID__;
        auto shardid = __ROUTE_RECV_ID__;
        if ( shardid == _invalid_int )
        {
            shardid = _kf_hash.FindHashNode( clientid, true );
            if ( shardid == _invalid_int )
            {
                __LOG_ERROR__( "msgid[{}] transmit failed!", msgid );
                return false;
            }
        }

        return _kf_tcp_client->SendNetMessage( shardid, clientid, msgid, data, length );
    }

    __KF_TRANSMIT_MESSAGE_FUNCTION__( KFClusterProxyModule::TransmitMessageToClient )
    {
        auto clientid = __ROUTE_RECV_ID__;
        return _kf_tcp_server->SendNetMessage( clientid, msgid, data, length );
    }
}