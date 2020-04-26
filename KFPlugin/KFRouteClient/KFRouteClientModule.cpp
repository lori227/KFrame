#include "KFRouteClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRouteClientModule::BeforeRun()
    {
        _kf_cluster_client->RegisterConnectionFunction( this, &KFRouteClientModule::OnRouteConnectCluster );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_OK, &KFRouteClientModule::HandleRouteMessageOk );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_DISCOVER_TO_CLIENT_REQ, &KFRouteClientModule::HandleRouteDiscoverToClientReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK, &KFRouteClientModule::HandleRouteMessageToClientAck );
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
        __LOOP_TIMER_0__( 6000u, 1u, &KFRouteClientModule::OnTimerConnectionRouteMaster );
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
        auto kfaddress = _kf_ip_address->GetMasterIp( __STRING__( route ), 0u );
        if ( kfaddress == nullptr )
        {
            return;
        }

        __UN_TIMER_0__();
        _connect_route_master_failed_count = 0u;
        _kf_tcp_client->StartClient( kfaddress->_name, kfaddress->_type, kfaddress->_id, kfaddress->_ip, kfaddress->_port );
    }

    __KF_NET_EVENT_FUNCTION__( KFRouteClientModule::OnClientConnectRouteMasterFailed )
    {
        auto kfglobal = KFGlobal::Instance();
        if ( netdata->_name == __STRING__( route ) && netdata->_type == __STRING__( master ) )
        {
            ++_connect_route_master_failed_count;
            if ( _connect_route_master_failed_count > 5u )
            {
                // 超过设定次数, 重新连接
                _kf_tcp_client->CloseClient( netdata->_id, __FUNC_LINE__ );

                // 启动定时器
                __LOOP_TIMER_0__( 6000u, 1u, &KFRouteClientModule::OnTimerConnectionRouteMaster );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function )
    {
        auto kffunction = _kf_connection_function.Create( name );
        kffunction->_function = function;
    }

    void KFRouteClientModule::RemoveConnectionFunction( const std::string& name )
    {
        _kf_connection_function.Remove( name );
    }

    void KFRouteClientModule::SetTranspondFunction( KFTranspondFunction& function )
    {
        _kf_transpond_function = function;
    }

    void KFRouteClientModule::OnRouteConnectCluster( uint64 serverid )
    {
        RouteSyncObjectToProxy( _invalid_int );

        // 连接成功回调
        for ( auto& iter : _kf_connection_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( serverid );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteClientModule::SyncObject( RouteObjectList& objectlist )
    {
        _service_object_list.swap( objectlist );

        // 如果在服务中, 同步给转发集群
        if ( _kf_cluster_client->IsInService() )
        {
            RouteSyncObjectToProxy( _invalid_int );
        }
    }

    void KFRouteClientModule::RouteSyncObjectToProxy( uint64 shardid )
    {
        // 发送给shard
        KFMsg::S2SRouteSyncObjectToProxyReq req;
        req.set_name( _service_name );
        req.set_shardid( shardid );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );

        for ( auto objectid : _service_object_list )
        {
            req.add_objectid( objectid );
        }

        _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFRouteClientModule::AddObject( uint64 objectid )
    {
        _service_object_list.insert( objectid );

        KFMsg::S2SRouteAddObjectToProxyReq req;
        req.set_name( _service_name );
        req.set_objectid( objectid );
        req.set_objectcount( _service_object_list.size() );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_PROXY_REQ, &req );
    }

    void KFRouteClientModule::RemoveObject( uint64 objectid )
    {
        _service_object_list.erase( objectid );

        KFMsg::S2SRouteRemoveObjectToProxyReq req;
        req.set_name( _service_name );
        req.set_objectid( objectid );
        req.set_objectcount( _service_object_list.size() );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );
        _kf_cluster_client->SendToProxy( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_PROXY_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteDiscoverToClientReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteDiscoverToClientReq )

        // 把所有对象列表同步到shard
        RouteSyncObjectToProxy( kfmsg.shardid() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    const std::string& KFRouteClientModule::FindRouteProcess( const std::string& module )
    {
        auto iter = KFRouteConfig::Instance()->_route_list.find( module );
        if ( iter == KFRouteConfig::Instance()->_route_list.end() )
        {
            return module;
        }

        return iter->second;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToRoute( const Route& route, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto serverid = __ROUTE_SERVER_ID__;
        auto sendid = __ROUTE_RECV_ID__;
        auto recvid = __ROUTE_SEND_ID__;

        return SendToPlayer( sendid, serverid, recvid, msgid, message );
    }

    bool KFRouteClientModule::RepeatToRoute( const Route& route, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto serverid = __ROUTE_SERVER_ID__;
        auto sendid = __ROUTE_RECV_ID__;
        auto recvid = __ROUTE_SEND_ID__;

        return RepeatToPlayer( sendid, serverid, recvid, msgid, message );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToAll( const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        return SendToAll( 0u, name, msgid, message );
    }

    bool KFRouteClientModule::SendToAll( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameAllReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( process );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToAll( const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        return RepeatToAll( 0u, name, msgid, message );
    }

    bool KFRouteClientModule::RepeatToAll( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameAllReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pbroute->set_serial( ++_route_serial );

        req.set_name( process );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToRand( const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        return SendToRand( 0u, name, msgid, message );
    }

    bool KFRouteClientModule::SendToRand( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameRandReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( process );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToRand( const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        return RepeatToRand( 0u, name, msgid, message );
    }

    bool KFRouteClientModule::RepeatToRand( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameRandReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pbroute->set_serial( ++_route_serial );

        req.set_name( process );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToBalance( const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        return SendToBalance( 0u, name, msgid, message );
    }

    bool KFRouteClientModule::SendToBalance( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameBalanceReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( process );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToBalance( const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        return RepeatToBalance( 0u, name, msgid, message );
    }

    bool KFRouteClientModule::RepeatToBalance( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameBalanceReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pbroute->set_serial( ++_route_serial );

        req.set_name( process );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToObject( const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return SendToObject( 0u, name, objectid, msgid, message );
    }

    bool KFRouteClientModule::SendToObject( uint64 sendid, const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameObjectReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( objectid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_name( process );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToObject( const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return RepeatToObject( 0u, name, objectid, msgid, message );
    }

    bool KFRouteClientModule::RepeatToObject( uint64 sendid, const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto& process = FindRouteProcess( name );

        KFMsg::S2SRouteMessageToNameObjectReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( objectid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pbroute->set_serial( ++_route_serial );

        req.set_name( process );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToServer( uint64 serverid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return SendToServer( 0u, serverid, msgid, message );
    }

    bool KFRouteClientModule::SendToServer( uint64 sendid, uint64 serverid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToServerReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( serverid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_targetid( serverid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToServer( uint64 serverid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return RepeatToServer( 0u, serverid, msgid, message );
    }

    bool KFRouteClientModule::RepeatToServer( uint64 sendid, uint64 serverid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToServerReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( serverid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pbroute->set_serial( ++_route_serial );

        req.set_targetid( serverid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRouteClientModule::SendToPlayer( uint64 serverid, uint64 recvid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return SendToPlayer( recvid, serverid, recvid, msgid, message );
    }

    bool KFRouteClientModule::SendToPlayer( uint64 sendid, uint64 serverid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToPlayerReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( playerid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        req.set_targetid( serverid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return _kf_cluster_client->RepeatToProxy( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ, &req );
    }

    bool KFRouteClientModule::RepeatToPlayer( uint64 serverid, uint64 recvid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return RepeatToPlayer( recvid, serverid, recvid, msgid, message );
    }

    bool KFRouteClientModule::RepeatToPlayer( uint64 sendid, uint64 serverid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SRouteMessageToPlayerReq req;
        auto pbroute = req.mutable_pbroute();
        pbroute->set_sendid( sendid );
        pbroute->set_recvid( playerid );
        pbroute->set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        pbroute->set_serial( ++_route_serial );

        req.set_targetid( serverid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendRouteMessage( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ, &req );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteMessageToClientAck )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToClientAck );

        auto pbroute = &kfmsg.pbroute();
        {
            // 回复转发消息成功
            SendRouteMessageOk( pbroute->serverid(), pbroute->serial() );
        }

        auto msgdata = kfmsg.msgdata().data();
        auto msglength = static_cast< uint32 >( kfmsg.msgdata().length() );
        Route temproute( pbroute->serverid(), pbroute->sendid(), pbroute->recvid() );
        bool ok = __CALL_MESSAGE__( temproute, kfmsg.msgid(), msgdata, msglength );
        if ( ok )
        {
            return;
        }

        if ( _kf_transpond_function != nullptr )
        {
            auto ok = _kf_transpond_function( temproute, kfmsg.msgid(), msgdata, msglength );
            if ( !ok )
            {
                __LOG_ERROR__( "route msgid[{}] failed", kfmsg.msgid() );
            }
        }
        else
        {
            __LOG_ERROR__( "msgid[{}] can't find handle", kfmsg.msgid() );
        }
    }

    void KFRouteClientModule::SendRouteMessageOk( uint64 serverid, uint64 serial )
    {
        if ( serial == 0u )
        {
            return;
        }

        KFMsg::S2SRouteMessageOk req;
        req.set_serial( serial );
        SendToServer( serverid, KFMsg::S2S_ROUTE_MESSAGE_OK, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteClientModule::HandleRouteMessageOk )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageOk );

        _route_message_list.Remove( kfmsg.serial() );
    }

    bool KFRouteClientModule::SendRouteMessage( uint32 msgid, ::google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();

        // 添加到队列中
        if ( _route_message_list.Size() <= 20000u )
        {
            auto routemessage = __KF_NEW__( KFRouteMessage, strdata.size() );
            routemessage->_msg_id = msgid;
            memcpy( routemessage->_data, strdata.data(), strdata.size() );
            routemessage->SetRouteTime();
            _route_message_list.Insert( _route_serial, routemessage );
        }
        else
        {
            __LOG_ERROR__( "route message=[{}] list size=[{}] too many", msgid, _route_message_list.Size() );
        }

        // 发送消息
        return _kf_cluster_client->SendToProxy( msgid, strdata.data(), strdata.size() );
    }

    // 更新逻辑
    void KFRouteClientModule::RunSendRouteMessage()
    {
        for ( auto& iter : _route_message_list._objects )
        {
            auto kfroutemessage = iter.second;
            if ( !kfroutemessage->IsRouteTime() )
            {
                continue;
            }

            kfroutemessage->SetRouteTime();
            auto ok = _kf_cluster_client->SendToProxy( kfroutemessage->_msg_id, kfroutemessage->_data, kfroutemessage->_length );
            if ( !ok )
            {
                break;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
