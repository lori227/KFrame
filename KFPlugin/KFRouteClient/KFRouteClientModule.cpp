#include "KFRouteClientModule.hpp"

namespace KFrame
{
    void KFRouteClientModule::BeforeRun()
    {
        _kf_cluster_client->RegisterConnectionFunction( this, &KFRouteClientModule::OnRouteConnectCluster );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_MESSAGE_OK, &KFRouteClientModule::HandleRouteMessageOk );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_DISCOVER_TO_CLIENT_REQ, &KFRouteClientModule::HandleRouteDiscoverToClientReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK, &KFRouteClientModule::HandleRouteMessageToClientAck );
    }

    void KFRouteClientModule::BeforeShut()
    {
        _route_message_list.Clear();
        _kf_cluster_client->UnRegisterConnectionFunction( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_OK );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_DISCOVER_TO_CLIENT_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK );
    }

    void KFRouteClientModule::PrepareRun()
    {
        // 注册服务
        _service_object_list.clear();
        _service_name = KFGlobal::Instance()->_app_name;

        // 启动连接定时器
        __LOOP_TIMER_0__( 6000u, 0u, &KFRouteClientModule::OnTimerConnectionRouteMaster );
    }

    void KFRouteClientModule::Run()
    {
        // 发送转发消息
        RunSendRouteMessage();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFRouteClientModule::OnTimerConnectionRouteMaster )
    {
        // 找到route 地址自动连接, 不需要bus对每个节点都配置一条连接信息
        auto ip_address = _kf_ip_address->GetMasterIp( __STRING__( route ), 0u );
        if ( ip_address == nullptr )
        {
            return;
        }

        __UN_TIMER_0__();
        _connect_route_master_failed_count = 0u;
        _kf_tcp_client->StartClient( ip_address->_name, ip_address->_type, ip_address->_id, ip_address->_ip, ip_address->_port );
    }

    __KF_NET_EVENT_FUNCTION__( KFRouteClientModule::OnClientConnectRouteMasterFailed )
    {
        auto global = KFGlobal::Instance();
        if ( net_data->_name == __STRING__( route ) && net_data->_type == __STRING__( master ) )
        {
            ++_connect_route_master_failed_count;
            if ( _connect_route_master_failed_count > 5u )
            {
                // 超过设定次数, 重新连接
                _kf_tcp_client->CloseClient( net_data->_id, __FUNC_LINE__ );

                // 启动定时器
                __LOOP_TIMER_0__( 6000u, 1u, &KFRouteClientModule::OnTimerConnectionRouteMaster );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::AddConnectionFunction( KFModule* module, KFClusterConnectionFunction& function )
    {
        auto function_data = _kf_connection_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFRouteClientModule::RemoveConnectionFunction( KFModule* module )
    {
        _kf_connection_function.Remove( module );
    }

    void KFRouteClientModule::SetForwardFunction( KFModule* module, KFForwardFunction& function )
    {
        if ( function == nullptr )
        {
            _kf_forward_function.Reset();
        }
        else
        {
            _kf_forward_function.SetFunction( module, function );
        }
    }

    void KFRouteClientModule::OnRouteConnectCluster( uint64 server_id )
    {
        RouteSyncObjectToProxy( _invalid_int );

        // 连接成功回调
        for ( auto& iter : _kf_connection_function._objects )
        {
            auto function = iter.second;
            function->Call( server_id );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::SyncObject( UInt64Set& object_list )
    {
        _service_object_list.swap( object_list );

        // 如果在服务中, 同步给转发集群
        if ( _kf_cluster_client->IsInService() )
        {
            RouteSyncObjectToProxy( _invalid_int );
        }
    }

    void KFRouteClientModule::RouteSyncObjectToProxy( uint64 shard_id )
    {
        // 发送给shard
        KFMsg::S2SRouteSyncObjectToProxyReq req;
        req.set_name( _service_name );
        req.set_shardid( shard_id );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );

        for ( auto object_id : _service_object_list )
        {
            req.add_objectid( object_id );
        }

        _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFRouteClientModule::AddObject( uint64 object_id )
    {
        _service_object_list.insert( object_id );

        KFMsg::S2SRouteAddObjectToProxyReq req;
        req.set_name( _service_name );
        req.set_objectid( object_id );
        req.set_objectcount( _service_object_list.size() );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFRouteClientModule::RemoveObject( uint64 object_id )
    {
        _service_object_list.erase( object_id );

        KFMsg::S2SRouteRemoveObjectToProxyReq req;
        req.set_name( _service_name );
        req.set_objectid( object_id );
        req.set_objectcount( _service_object_list.size() );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_PROXY_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteDiscoverToClientReq, KFMsg::S2SRouteDiscoverToClientReq )
    {
        // 把所有对象列表同步到shard
        RouteSyncObjectToProxy( kfmsg->shardid() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    const std::string& KFRouteClientModule::FindRouteProcess( const std::string& module )
    {
        auto setting = KFRouteConfig::Instance()->FindSetting( module );
        if ( setting == nullptr )
        {
            return module;
        }

        return setting->_target;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToRoute( const Route& route, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto server_id = __ROUTE_SERVER_ID__;
        auto send_id = __ROUTE_RECV_ID__;
        auto recv_id = __ROUTE_SEND_ID__;

        return SendToEntity( send_id, server_id, recv_id, msg_id, message );
    }

    bool KFRouteClientModule::RepeatToRoute( const Route& route, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto server_id = __ROUTE_SERVER_ID__;
        auto send_id = __ROUTE_RECV_ID__;
        auto recv_id = __ROUTE_SEND_ID__;

        return RepeatToEntity( send_id, server_id, recv_id, msg_id, message );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToAll( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return SendToAll( 0u, name, msg_id, message );
    }

    bool KFRouteClientModule::SendToAll( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameAllReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( process );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToAll( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return RepeatToAll( 0u, name, msg_id, message );
    }

    bool KFRouteClientModule::RepeatToAll( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameAllReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pb_route->set_serial( ++_route_serial );

        req.set_name( process );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToRand( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return SendToRand( 0u, name, msg_id, message );
    }

    bool KFRouteClientModule::SendToRand( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameRandReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( process );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToRand( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return RepeatToRand( 0u, name, msg_id, message );
    }

    bool KFRouteClientModule::RepeatToRand( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameRandReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pb_route->set_serial( ++_route_serial );

        req.set_name( process );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToBalance( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return SendToBalance( 0u, name, msg_id, message );
    }

    bool KFRouteClientModule::SendToBalance( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameBalanceReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( process );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToBalance( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return RepeatToBalance( 0u, name, msg_id, message );
    }

    bool KFRouteClientModule::RepeatToBalance( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameBalanceReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pb_route->set_serial( ++_route_serial );

        req.set_name( process );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToObject( const std::string& name, uint64 objectid, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return SendToObject( 0u, name, objectid, msg_id, message );
    }

    bool KFRouteClientModule::SendToObject( uint64 send_id, const std::string& name, uint64 objectid, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameObjectReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_recvid( objectid );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( process );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToObject( const std::string& name, uint64 objectid, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return RepeatToObject( 0u, name, objectid, msg_id, message );
    }

    bool KFRouteClientModule::RepeatToObject( uint64 send_id, const std::string& name, uint64 objectid, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameObjectReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_recvid( objectid );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pb_route->set_serial( ++_route_serial );

        req.set_name( process );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToServer( uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return SendToServer( 0u, server_id, msg_id, message );
    }

    bool KFRouteClientModule::SendToServer( uint64 send_id, uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToServerReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_recvid( server_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_targetid( server_id );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToServer( uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return RepeatToServer( 0u, server_id, msg_id, message );
    }

    bool KFRouteClientModule::RepeatToServer( uint64 send_id, uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToServerReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_recvid( server_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pb_route->set_serial( ++_route_serial );

        req.set_targetid( server_id );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToEntity( uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return SendToEntity( recv_id, server_id, recv_id, msg_id, message );
    }

    bool KFRouteClientModule::SendToEntity( uint64 send_id, uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToEntityReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_recvid( recv_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_targetid( server_id );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_ENTITY_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToEntity( uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return RepeatToEntity( recv_id, server_id, recv_id, msg_id, message );
    }

    bool KFRouteClientModule::RepeatToEntity( uint64 send_id, uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToEntityReq req;
        auto pb_route = req.mutable_pbroute();
        pb_route->set_sendid( send_id );
        pb_route->set_recvid( recv_id );
        pb_route->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pb_route->set_serial( ++_route_serial );

        req.set_targetid( server_id );
        req.set_msgid( msg_id );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_ENTITY_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteMessageToClientAck, KFMsg::S2SRouteMessageToClientAck )
    {
        auto pb_route = &kfmsg->pbroute();
        {
            // 回复转发消息成功
            SendRouteMessageOk( pb_route->serverid(), pb_route->serial() );
        }

        auto msg_data = kfmsg->msgdata().data();
        auto msg_length = static_cast<uint32>( kfmsg->msgdata().length() );
        Route temp_route( pb_route->serverid(), pb_route->sendid(), pb_route->recvid() );
        bool ok = __HANDLE_MESSAGE__( temp_route, kfmsg->msgid(), msg_data, msg_length );
        if ( !ok )
        {
            ok = _kf_forward_function.CallEx<bool>( temp_route, kfmsg->msgid(), msg_data, msg_length );
            if ( !ok )
            {
                __LOG_ERROR__( "route msg_id=[{}] failed", kfmsg->msgid() );
            }
        }
    }

    void KFRouteClientModule::SendRouteMessageOk( uint64 server_id, uint64 serial )
    {
        if ( serial == 0u )
        {
            return;
        }

        KFMsg::S2SRouteMessageOk req;
        req.set_serial( serial );
        SendToServer( server_id, KFMsg::S2S_ROUTE_MESSAGE_OK, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteMessageOk, KFMsg::S2SRouteMessageOk )
    {
        _route_message_list.Remove( kfmsg->serial() );
    }

    bool KFRouteClientModule::SendRouteMessage( uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto data = message->SerializeAsString();

        // 添加到队列中
        if ( _route_message_list.Size() <= 20000u )
        {
            auto route_message = __MAKE_SHARED__( KFRouteMessage, data.size() );
            route_message->_msg_id = msg_id;
            memcpy( route_message->_data, data.data(), data.size() );
            route_message->SetRouteTime();
            _route_message_list.Insert( _route_serial, route_message );
        }
        else
        {
            __LOG_ERROR__( "route message=[{}] list size=[{}] too many", msg_id, _route_message_list.Size() );
        }

        // 发送消息
        return _kf_cluster_client->SendToProxy( msg_id, data.data(), data.size() );
    }

    // 更新逻辑
    void KFRouteClientModule::RunSendRouteMessage()
    {
        for ( auto& iter : _route_message_list._objects )
        {
            auto route_message = iter.second;
            if ( !route_message->IsRouteTime() )
            {
                continue;
            }

            route_message->SetRouteTime();
            auto ok = _kf_cluster_client->SendToProxy( route_message->_msg_id, route_message->_data, route_message->_length );
            if ( !ok )
            {
                break;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
