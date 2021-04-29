﻿#include "KFClusterClientModule.hpp"

namespace KFrame
{
    KFClusterClientModule::~KFClusterClientModule()
    {
        _send_keeper_list.clear();
    }

    void KFClusterClientModule::BeforeRun()
    {
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFClusterClientModule::OnClientConnectionClusterServer );
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFClusterClientModule::OnClientLostClusterProxy );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK, &KFClusterClientModule::HandleClusterAuthToClientAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_CLUSTER_VERIFY_TO_CLIENT_ACK, &KFClusterClientModule::HandleClusterVerifyToClientAck );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFClusterClientModule::BeforeShut()
    {
        __UN_TCP_CLIENT_CONNECTION__();
        __UN_TCP_CLIENT_SHUTDOWN__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK );
        __UN_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_TO_CLIENT_ACK );

    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterClientModule::Run()
    {
        if ( !_cluster_in_services )
        {
            return;
        }

        RunSendKeeper();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterClientModule::AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function )
    {
        auto function_data = _kf_connection_function.Create( name );
        function_data->SetFunction( function );
    }

    void KFClusterClientModule::RemoveConnectionFunction( const std::string& name )
    {
        _kf_connection_function.Remove( name );
    }

    void KFClusterClientModule::CallClusterConnectionFunction( uint64 server_id )
    {
        // 执行回调函数
        for ( auto& iter : _kf_connection_function._objects )
        {
            auto function = iter.second;
            function->Call( server_id );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFClusterClientModule::OnClientConnectionClusterServer )
    {
        // cluster 只会连接和自己不同类型的服务
        static auto _cluster_name_constant = KFGlobal::Instance()->FindConstant( __STRING__( clustername ) );
        if ( net_data->_name != _cluster_name_constant->_str_value )
        {
            return;
        }

        __LOG_DEBUG__( "connect cluster server[{}:{}:{}]", net_data->_name, net_data->_type, net_data->_str_id );

        if ( net_data->_type == __STRING__( master ) )
        {
            // 保存信息
            _cluster_name = net_data->_name;
            _cluster_type = net_data->_type;
            _cluster_master_id = net_data->_id;
            _cluster_master_ip = net_data->_ip;
            _cluster_master_port = net_data->_port;

            // 开启认证定时器
            __LOOP_TIMER_1__( net_data->_id, 10000, 1000, &KFClusterClientModule::OnTimerSendClusterAuthMessage );
        }
        else if ( net_data->_type == __STRING__( proxy ) )
        {
            // 发送登录消息定时器
            __LOOP_TIMER_1__( net_data->_id, 3000, 1, &KFClusterClientModule::OnTimerSendClusterTokenMessage );
        }
    }

    __KF_TIMER_FUNCTION__( KFClusterClientModule::OnTimerSendClusterAuthMessage )
    {
        static auto _cluster_key_constant = KFGlobal::Instance()->FindConstant( __STRING__( clusterkey ) );

        // 请求认证
        KFMsg::S2SClusterAuthToMasterReq req;
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );
        req.set_clusterkey( _cluster_key_constant->_str_value );
        auto ok = _kf_tcp_client->SendNetMessage( _cluster_master_id, KFMsg::S2S_CLUSTER_AUTH_TO_MASTER_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "send cluster[{}:{}] auth failed", _cluster_name, KFAppId::ToString( _cluster_master_id ) );
        }
    }

    __KF_TIMER_FUNCTION__( KFClusterClientModule::OnTimerSendClusterTokenMessage )
    {
        // 请求认证
        KFMsg::S2SClusterVerifyToProxyReq req;
        req.set_token( _auth_token );
        req.set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        auto ok = _kf_tcp_client->SendNetMessage( object_id, KFMsg::S2S_CLUSTER_VERIFY_TO_PROXY_REQ, &req );
        if ( !ok )
        {
            // 删除定时器
            __UN_TIMER_1__( object_id );

            // 重新连接
            ReconnectClusterMaster();
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterClientModule::OnClientLostClusterProxy )
    {
        // 不是转发集群
        if ( _cluster_name == net_data->_name && net_data->_type == __STRING__( proxy ) )
        {
            // 重新启动连接
            ReconnectClusterMaster();
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterClientModule::HandleClusterAuthToClientAck, KFMsg::S2SClusterAuthToClientAck )
    {
        auto listen = &kfmsg->listen();

        _auth_token = kfmsg->token();
        _cluster_proxy_id = listen->appid();

        // 停止定时器
        __UN_TIMER_1__( _cluster_master_id );

        // 删除master连接
        _kf_tcp_client->CloseClient( _cluster_master_id, __FUNC_LINE__ );

        // 连接新的服务器
        _kf_tcp_client->StartClient( listen->appname(), listen->apptype(), listen->appid(), listen->ip(), listen->port() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterClientModule::HandleClusterVerifyToClientAck, KFMsg::S2SClusterVerifyToClientAck )
    {
        // 停止定时器
        __UN_TIMER_1__( _cluster_proxy_id );

        if ( kfmsg->serverid() == 0 )
        {
            // 重新连接
            ReconnectClusterMaster();
        }
        else
        {
            // 设置可以服务
            _cluster_in_services = true;

            // 回调函数
            CallClusterConnectionFunction( _cluster_proxy_id );

            __LOG_INFO__( "[{}][{}] cluster services ok", _cluster_name, KFAppId::ToString( _cluster_proxy_id ) );
        }
    }

    void KFClusterClientModule::ReconnectClusterMaster()
    {
        __LOG_ERROR__( "cluster=[{}] service reconnect", _cluster_name );

        // 断开proxy服务器
        _kf_tcp_client->CloseClient( _cluster_proxy_id, __FUNC_LINE__ );

        _cluster_proxy_id = 0u;
        _cluster_in_services = false;
        _kf_tcp_client->StartClient( _cluster_name, _cluster_type, _cluster_master_id, _cluster_master_ip, _cluster_master_port );
    }
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    bool KFClusterClientModule::IsInService()
    {
        return _cluster_in_services;
    }

    bool KFClusterClientModule::SendToProxy( uint32 msg_id, const char* data, uint32 length )
    {
        return SendToProxy( _invalid_int, msg_id, data, length );
    }

    bool KFClusterClientModule::SendToProxy( uint32 msg_id, google::protobuf::Message* message )
    {
        return SendToProxy( _invalid_int, msg_id, message );
    }

    bool KFClusterClientModule::SendToProxy( uint64 shard_id, uint32 msg_id, google::protobuf::Message* message )
    {
        auto data = message->SerializeAsString();
        return SendToProxy( shard_id, msg_id, data.data(), data.size() );
    }

    bool KFClusterClientModule::SendToProxy( uint64 shard_id, uint32 msg_id, const char* data, uint32 length )
    {
        if ( !_cluster_in_services )
        {
            __LOG_ERROR__( "cluster not in service, send msg_id=[{}] failed", msg_id );
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _cluster_proxy_id, shard_id, msg_id, data, length );
    }

    bool KFClusterClientModule::RepeatToProxy( uint32 msg_id, const char* data, uint32 length )
    {
        return RepeatToProxy( _invalid_int, msg_id, data, length );
    }

    bool KFClusterClientModule::RepeatToProxy( uint32 msg_id, google::protobuf::Message* message )
    {
        return RepeatToProxy( _invalid_int, msg_id, message );
    }

    bool KFClusterClientModule::RepeatToProxy( uint64 shard_id, uint32 msg_id, google::protobuf::Message* message )
    {
        auto data = message->SerializeAsString();
        return RepeatToProxy( shard_id, msg_id, data.data(), data.length() );
    }

    bool KFClusterClientModule::RepeatToProxy( uint64 shard_id, uint32 msg_id, const char* data, uint32 length )
    {
        if ( !_cluster_in_services )
        {
            // 不在服务中
            AddSendKeeper( shard_id, msg_id, data, length );
        }
        else
        {
            if ( _send_keeper_list.empty() )
            {
                // 发送队列为空
                auto ok = _kf_tcp_client->SendNetMessage( _cluster_proxy_id, shard_id, msg_id, data, length );
                if ( !ok )
                {
                    AddSendKeeper( shard_id, msg_id, data, length );
                }
            }
            else
            {
                AddSendKeeper( shard_id, msg_id, data, length );
            }
        }

        return true;
    }

    void KFClusterClientModule::AddSendKeeper( uint64 shard_id, uint32 msg_id, const char* data, uint32 length )
    {
        auto keeper = __MAKE_SHARED__( SendKeeper, length );
        keeper->_shard_id = shard_id;
        keeper->_msg_id = msg_id;
        memcpy( keeper->_data, data, length );
        _send_keeper_list.push_back( keeper );
    }

    void KFClusterClientModule::RunSendKeeper()
    {
        while ( !_send_keeper_list.empty() )
        {
            // 把消息发送出去
            auto keeper = _send_keeper_list.front();
            auto ok = _kf_tcp_client->SendNetMessage( _cluster_proxy_id, keeper->_shard_id, keeper->_msg_id, keeper->_data, keeper->_length );
            if ( !ok )
            {
                // 发送失败, 有可能是消息队列满了, 下一帧继续发送
                break;
            }

            _send_keeper_list.pop_front();
        }
    }
}