#include "KFClusterProxyModule.h"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    KFClusterProxyModule::KFClusterProxyModule()
    {

    }

    KFClusterProxyModule::~KFClusterProxyModule()
    {
    }

    void KFClusterProxyModule::InitModule()
    {
    }

    void KFClusterProxyModule::BeforeRun()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFClusterProxyModule::OnClientLostServer );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFClusterProxyModule::OnClientConnectionServer );
        __REGISTER_CLIENT_TRANSMIT_FUNCTION__( &KFClusterProxyModule::TransmitMessageToClient );

        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFClusterProxyModule::OnServerDiscoverClient );
        __REGISTER_SERVER_TRANSMIT_FUNCTION__( &KFClusterProxyModule::TransmitMessageToShard );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_REQ, &KFClusterProxyModule::HandleClusterTokenReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_REQ, &KFClusterProxyModule::HandleClusterVerifyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_OBJECT_TO_PROXY_REQ, &KFClusterProxyModule::HandleAddObjectToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_REMOVE_OBJECT_TO_PROXY_REQ, &KFClusterProxyModule::HandleRemoveObjectToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SEND_TO_CLUSTER_OBJECT_REQ, &KFClusterProxyModule::HandleSendToObjectReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ALLOC_SHARD_ACK, &KFClusterProxyModule::HandleAllocShardAck );

    }

    void KFClusterProxyModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();

        _kf_config->RemoveConfig( _kf_plugin->_plugin_name );

        _kf_tcp_client->UnRegisterLostFunction( this );
        _kf_tcp_client->UnRegisterConnectionFunction( this );
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_CLIENT_TRANSMIT_FUNCTION__();

        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        __UNREGISTER_SERVER_TRANSMIT_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_TOKEN_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_OBJECT_TO_PROXY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REMOVE_OBJECT_TO_PROXY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SEND_TO_CLUSTER_OBJECT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ALLOC_SHARD_ACK );
    }

    void KFClusterProxyModule::Run()
    {
        RunClusterTokenValidTime();
    }

    void KFClusterProxyModule::AfterRun()
    {
        RunRemoveClusterToken();
    }

    void KFClusterProxyModule::RunClusterTokenValidTime()
    {
        auto nowtime = KFGlobal::Instance()->_real_time;
        for ( auto& iter : _kf_token_list._objects )
        {
            auto kftoken = iter.second;
            if ( kftoken->_valid_time < nowtime )
            {
                _kf_remove_list.insert( kftoken->_token );
            }
        }
    }

    void KFClusterProxyModule::RunRemoveClusterToken()
    {
        if ( _kf_remove_list.empty() )
        {
            return;
        }

        for ( auto& token : _kf_remove_list )
        {
            _kf_token_list.Remove( token );
        }

        _kf_remove_list.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFClusterProxyModule::SelectClusterShard( uint64 objectid, bool cache /* = false */ )
    {
        return _kf_hash.FindHashNode( objectid, cache );
    }

    uint32 KFClusterProxyModule::SelectClusterShard( const char* data, bool cache /* = false */ )
    {
        return _kf_hash.FindHashNode( data, cache );
    }

    uint32 KFClusterProxyModule::SelectClusterShard( const char* data, uint32 objectid, bool cache /* = false */ )
    {
        return _kf_hash.FindHashNode( data, objectid, cache );
    }

    void KFClusterProxyModule::SendMessageToShard( uint32 msgid, const char* data, uint32 length )
    {
        _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), msgid, data, length );
    }

    void KFClusterProxyModule::SendMessageToShard( uint32 msgid, ::google::protobuf::Message* message )
    {
        _kf_tcp_client->SendMessageToType( __KF_STRING__( shard ), msgid, message );
    }

    bool KFClusterProxyModule::SendMessageToShard( uint32 shardid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_client->SendNetMessage( shardid, msgid, data, length );
    }

    bool KFClusterProxyModule::SendMessageToShard( uint32 shardid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_client->SendNetMessage( shardid, msgid, message );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterProxyModule::SendMessageToClient( uint32 msgid, ::google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msgid, message );
    }

    void KFClusterProxyModule::SendMessageToClient( uint32 msgid, const char* data, uint32 length )
    {
        _kf_tcp_server->SendNetMessage( msgid, data, length );
    }

    bool KFClusterProxyModule::SendMessageToClient( uint32 clientid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( clientid, msgid, message );
    }

    bool KFClusterProxyModule::SendMessageToClient( uint32 clientid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_server->SendNetMessage( clientid, msgid, data, length );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
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
            OnClientConnectionClusterServer( servername, serverid );
        }
        else if ( servertype == __KF_STRING__( shard ) )
        {
            OnClientConnectionClusterShard( servername, serverid );
        }
    }

    void KFClusterProxyModule::OnClientConnectionClusterServer( const std::string& servername, uint32 serverid )
    {
        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SClusterRegisterReq req;
        req.set_type( kfglobal->_app_type );
        req.set_id( kfglobal->_app_id );
        req.set_name( kfglobal->_app_name );
        req.set_ip( kfglobal->_interanet_ip );
        req.set_port( kfglobal->_listen_port );
        _kf_tcp_client->SendNetMessage( serverid, KFMsg::S2S_CLUSTER_REGISTER_REQ, &req );

        __REGISTER_LOOP_TIMER__( serverid, 5000, &KFClusterProxyModule::OnTimerSendClusterUpdateMessage );
    }

    void KFClusterProxyModule::OnClientConnectionClusterShard( const std::string& servername, uint32 serverid )
    {
        _kf_hash.AddHashNode( servername, serverid, 100 );

        // 自己所有的连接注册到Cluster中
        KFMsg::S2SClusterClientListReq req;

        // 所得所有的连接列表
        std::list< uint32 > clientlist;
        _kf_tcp_server->GetHandleList( clientlist );
        for ( auto clientid : clientlist )
        {
            req.add_clientid( clientid );
        }

        SendMessageToShard( serverid, KFMsg::S2S_CLUSTER_CLIENT_LIST_REQ, &req );
    }

    __KF_SERVER_DISCOVER_FUNCTION__( KFClusterProxyModule::OnServerDiscoverClient )
    {
        KFMsg::S2SClusterClientListReq req;
        req.add_clientid( handleid );
        SendMessageToShard( KFMsg::S2S_CLUSTER_CLIENT_LIST_REQ, &req );
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
            OnClientLostClusterServer( servername, serverid );
        }
        else if ( servertype == __KF_STRING__( shard ) )
        {
            OnClientLostClusterShard( servername, serverid );
        }
    }

    void KFClusterProxyModule::OnClientLostClusterServer( const std::string& servername, uint32 serverid )
    {
        __UNREGISTER_OBJECT_TIMER__( serverid );
    }

    void KFClusterProxyModule::OnClientLostClusterShard( const std::string& servername, uint32 serverid )
    {
        _kf_hash.RemoveHashNode( serverid );

        // 删除映射
        RemoveObjectShard( serverid );
        _kf_object_count.erase( serverid );
    }

    __KF_TIMER_FUNCTION__( KFClusterProxyModule::OnTimerSendClusterUpdateMessage )
    {
        KFMsg::S2SClusterUpdateReq req;

        req.set_gateid( KFGlobal::Instance()->_app_id );
        req.set_count( _kf_tcp_server->GetHandleCount() );
        _kf_tcp_client->SendNetMessage( static_cast< uint32 >( objectid ), KFMsg::S2S_CLUSTER_UPDATE_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleClusterTokenReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterTokenReq );

        auto kftoken = _kf_token_list.Create( kfmsg.token() );
        kftoken->_token = kfmsg.token();
        kftoken->_gate_id = kfmsg.gateid();
        kftoken->_valid_time = KFGlobal::Instance()->_real_time + 60;

        __LOG_DEBUG__( KFLogEnum::Logic, "update client[{}] token[{}]!",
                       kftoken->_gate_id, kftoken->_token );
    }

    uint32 KFClusterProxyModule::ClusterVerifyLogin( const std::string& token, uint32 serverid )
    {
        auto kftoken = _kf_token_list.Find( token );
        if ( kftoken == nullptr )
        {
            return _invalid_int;
        }

        auto gateid = kftoken->_gate_id;
        _kf_token_list.Remove( token );

        if ( kftoken->_gate_id != serverid )
        {
            return _invalid_int;
        }

        return serverid;
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleClusterVerifyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterVerifyReq );

        uint32 serverid = ClusterVerifyLogin( kfmsg.token(), kfmsg.serverid() );

        KFMsg::S2SClusterVerifyAck ack;
        ack.set_serverid( serverid );
        ack.set_clustertype( kfmsg.clustertype() );
        _kf_tcp_server->SendNetMessage( __KF_HEAD_ID__( kfguid ), KFMsg::S2S_CLUSTER_VERIFY_ACK, &ack );

        if ( serverid != 0 )
        {
            __LOG_DEBUG__( KFLogEnum::Logic, "[{}:{}] cluster verify ok!",
                           kfmsg.serverid(), kfmsg.token(), serverid );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::System, "[{}:{}] cluster verify failed!",
                           kfmsg.serverid(), kfmsg.token(), serverid );
        }
    }

    __KF_TRANSMIT_FUNCTION__( KFClusterProxyModule::TransmitMessageToShard )
    {
        auto handleid = __KF_HEAD_ID__( kfguid );
        auto shardserverid = __KF_DATA_ID__( kfguid );

        if ( shardserverid == _invalid_int )
        {
            shardserverid = SelectClusterShard( handleid, true );
            if ( shardserverid == _invalid_int )
            {
                __LOG_ERROR__( KFLogEnum::System, "handleid[{}] can not select shard!", handleid );
                return false;
            }
        }

        return _kf_tcp_client->SendNetMessage( shardserverid, handleid, msgid, data, length );
    }

    __KF_TRANSMIT_FUNCTION__( KFClusterProxyModule::TransmitMessageToClient )
    {
        auto handleid = __KF_DATA_ID__( kfguid );
        return _kf_tcp_server->SendNetMessage( handleid, msgid, data, length );
    }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleAddObjectToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddObjectToProxyReq );

        auto shardid = __KF_HEAD_ID__( kfguid );
        for ( auto i = 0; i < kfmsg.objectid_size(); ++i )
        {
            auto objectid = kfmsg.objectid( i );
            _kf_object_shard[ objectid ] = shardid;

            __LOG_DEBUG__( KFLogEnum::Logic, "add object[{}:{}] ok!", objectid, shardid );
        }

        // 添加数量
        AddObjectCount( shardid, kfmsg.objectid_size() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleAllocShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SAllocShardAck );

        _kf_object_shard.clear();
        for ( auto i = 0; i < kfmsg.objectid_size(); ++i )
        {
            auto objectid = kfmsg.objectid( i );
            auto shardid = kfmsg.shardid( i );
            _kf_object_shard[ objectid ] = shardid;

            __LOG_DEBUG__( KFLogEnum::Logic, "add object[{}:{}] ok!", objectid, shardid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleRemoveObjectToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRemoveObjectToProxyReq );

        for ( auto i = 0; i < kfmsg.objectid_size(); ++i )
        {
            auto objectid = kfmsg.objectid( i );
            _kf_object_shard.erase( objectid );

            __LOG_DEBUG__( KFLogEnum::Logic, "remove object[{}] ok!", objectid );
        }

        auto shardid = __KF_HEAD_ID__( kfguid );
        DecObjectCount( shardid, kfmsg.objectid_size() );
    }

    void KFClusterProxyModule::AddObjectShard( uint64 objectid, uint32 shardid )
    {
        AddObjectCount( shardid, 1 );
        _kf_object_shard[ objectid ] = shardid;
    }

    uint32 KFClusterProxyModule::FindObjectShard( uint64 objectid )
    {
        auto iter = _kf_object_shard.find( objectid );
        if ( iter == _kf_object_shard.end() )
        {
            return _invalid_int;
        }

        return iter->second;
    }

    void KFClusterProxyModule::RemoveObjectShard( uint32 shardid )
    {
        for ( auto iter = _kf_object_shard.begin(); iter != _kf_object_shard.end(); )
        {
            if ( iter->second != shardid )
            {
                ++iter;
            }
            else
            {
                iter = _kf_object_shard.erase( iter );
            }
        }
    }

    void KFClusterProxyModule::AddObjectCount( uint32 shardid, uint32 count )
    {
        _kf_object_count[ shardid ] += count;
    }

    void KFClusterProxyModule::DecObjectCount( uint32 shardid, uint32 count )
    {
        auto iter = _kf_object_count.find( shardid );
        if ( iter == _kf_object_count.end() )
        {
            return;
        }

        iter->second -= __MIN__( iter->second, count );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterProxyModule::HandleSendToObjectReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSendToClusterObjectReq );

        auto shardid = FindObjectShard( kfmsg.objectid() );
        if ( shardid == _invalid_int )
        {
            __LOG_ERROR__( KFLogEnum::System, "msgid[{}] objectid[{}] can't find shard!",
                           kfmsg.msgid(), kfmsg.objectid() );
            return;
        }

        auto msgdata = kfmsg.msgdata();
        SendMessageToShard( shardid, kfmsg.msgid(), msgdata.data(), msgdata.size() );
    }

    uint32 KFClusterProxyModule::FindMinObjectShard()
    {
        auto mincount = std::numeric_limits<uint32>::max();
        auto shardid = 0;

        for ( auto& iter : _kf_object_count )
        {
            if ( iter.second < mincount )
            {
                mincount = iter.second;
                shardid = iter.first;
            }
        }

        return shardid;
    }
}