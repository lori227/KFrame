#include "KFClusterClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFClusterClientModule::~KFClusterClientModule()
    {
        for ( auto keeper : _send_keeper )
        {
            __KF_DELETE__( SendKeeper, keeper );
        }
        _send_keeper.clear();
    }

    void KFClusterClientModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST__( &KFClusterClientModule::OnClientLostServer );
        __REGISTER_CLIENT_CONNECTION__( &KFClusterClientModule::OnClientConnectionServer );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK, &KFClusterClientModule::HandleClusterAuthToClientAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_TO_CLIENT_ACK, &KFClusterClientModule::HandleClusterVerifyToClientAck );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFClusterClientModule::BeforeShut()
    {
        __UN_CLIENT_LOST__();
        __UN_CLIENT_CONNECTION__();
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
        auto kfbind = _kf_connection_function.Create( name );
        kfbind->_function = function;
    }

    void KFClusterClientModule::RemoveConnectionFunction( const std::string& name )
    {
        _kf_connection_function.Remove( name );
    }

    void KFClusterClientModule::CallClusterConnectionFunction( uint64 serverid )
    {
        // 执行回调函数
        for ( auto& iter : _kf_connection_function._objects )
        {
            auto kfbind = iter.second;
            kfbind->_function( serverid );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFClusterClientModule::OnClientConnectionServer )
    {
        // cluster 只会连接和自己不同类型的服务
        if ( netdata->_name != _kf_ip_address->GetClusterName() )
        {
            return;
        }

        __LOG_DEBUG__( "connect route cluster server[{}:{}:{}]", netdata->_name, netdata->_type, netdata->_str_id );

        if ( netdata->_type == __STRING__( master ) )
        {
            // 保存信息
            _str_master_id = netdata->_str_id;
            _cluster_name = netdata->_name;
            _cluster_master_id = netdata->_id;

            // 开启认证定时器
            __LOOP_TIMER_1__( netdata->_id, 10000, 1000, &KFClusterClientModule::OnTimerSendClusterAuthMessage );
        }
        else if ( netdata->_type == __STRING__( proxy ) )
        {
            // 发送登录消息定时器
            __LOOP_TIMER_1__( netdata->_id, 3000, 1, &KFClusterClientModule::OnTimerSendClusterTokenMessage );
        }
    }

    __KF_TIMER_FUNCTION__( KFClusterClientModule::OnTimerSendClusterAuthMessage )
    {
        // 请求认证
        KFMsg::S2SClusterAuthToMasterReq req;
        req.set_clusterkey( _kf_ip_address->GetClusterKey() );
        req.set_clientid( KFGlobal::Instance()->_app_id->GetId() );
        auto ok = _kf_tcp_client->SendNetMessage( _cluster_master_id, KFMsg::S2S_CLUSTER_AUTH_TO_MASTER_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "send cluster[{}:{}] auth failed!", _cluster_name, _str_master_id );
        }
    }

    __KF_TIMER_FUNCTION__( KFClusterClientModule::OnTimerSendClusterTokenMessage )
    {
        // 请求认证
        KFMsg::S2SClusterVerifyToProxyReq req;
        req.set_token( _auth_token );
        req.set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        auto ok = _kf_tcp_client->SendNetMessage( objectid, KFMsg::S2S_CLUSTER_VERIFY_TO_PROXY_REQ, &req );
        if ( !ok )
        {
            // 删除定时器
            __UN_TIMER_1__( objectid );

            // 重新连接
            ReconnectClusterMaster();
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFClusterClientModule::OnClientLostServer )
    {
        // 不是转发集群
        if ( _cluster_name != netdata->_name || netdata->_type != __STRING__( proxy ) )
        {
            return;
        }

        // 重新启动连接
        ReconnectClusterMaster();
    }

    __KF_MESSAGE_FUNCTION__( KFClusterClientModule::HandleClusterAuthToClientAck )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterAuthToClientAck );
        auto listen = &kfmsg.listen();

        _auth_token = kfmsg.token();
        _cluster_proxy_id = listen->appid();

        // 停止定时器
        __UN_TIMER_1__( _cluster_master_id );

        // 删除master连接
        _kf_tcp_client->CloseClient( _cluster_master_id, __FUNC_LINE__ );

        // 连接新的服务器
        _kf_tcp_client->StartClient( listen->appname(), listen->apptype(), listen->appid(), listen->ip(), listen->port() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterClientModule::HandleClusterVerifyToClientAck )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterVerifyToClientAck );

        // 停止定时器
        __UN_TIMER_1__( _cluster_proxy_id );

        if ( kfmsg.serverid() == 0 )
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

            __LOG_INFO__( "[{}][{}] cluster services ok!", _cluster_name, KFAppId::ToString( _cluster_proxy_id ) );
        }
    }

    void KFClusterClientModule::ReconnectClusterMaster()
    {
        __LOG_ERROR__( "cluster=[{}] service reset!", _cluster_name );

        // 断开proxy服务器
        _kf_tcp_client->CloseClient( _cluster_proxy_id, __FUNC_LINE__ );

        _cluster_proxy_id = 0;
        _cluster_in_services = false;

        auto* kfaddress = _kf_ip_address->FindIpAddress( _cluster_name, __STRING__( master ), _str_master_id );
        if ( kfaddress == nullptr )
        {
            return __LOG_ERROR__( "can't [{}:{}:{}] address!", _cluster_name, __STRING__( master ), _str_master_id );
        }

        KFIpAddress ipaddress = *kfaddress;
        ipaddress._port = _kf_ip_address->CalcListenPort( kfaddress->_port_type, kfaddress->_port, kfaddress->_id );
        _kf_tcp_client->StartClient( &ipaddress );
    }
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    bool KFClusterClientModule::IsInService()
    {
        return _cluster_in_services;
    }

    bool KFClusterClientModule::SendToProxy( uint32 msgid, const char* data, uint32 length )
    {
        return SendToProxy( _invalid_int, msgid, data, length );
    }

    bool KFClusterClientModule::SendToProxy( uint32 msgid, google::protobuf::Message* message )
    {
        return SendToProxy( _invalid_int, msgid, message );
    }

    bool KFClusterClientModule::SendToProxy( uint64 shardid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return SendToProxy( shardid, msgid, strdata.data(), strdata.size() );
    }

    bool KFClusterClientModule::SendToProxy( uint64 shardid, uint32 msgid, const char* data, uint32 length )
    {
        if ( !_cluster_in_services )
        {
            __LOG_ERROR__( "cluster not in service, send msgid=[{}] shardid=[{}] failed!", msgid );
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _cluster_proxy_id, shardid, msgid, data, length );
    }

    bool KFClusterClientModule::RepeatToProxy( uint32 msgid, const char* data, uint32 length )
    {
        return RepeatToProxy( _invalid_int, msgid, data, length );
    }

    bool KFClusterClientModule::RepeatToProxy( uint32 msgid, google::protobuf::Message* message )
    {
        return RepeatToProxy( _invalid_int, msgid, message );
    }

    bool KFClusterClientModule::RepeatToProxy( uint64 shardid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return RepeatToProxy( shardid, msgid, strdata.data(), strdata.length() );
    }

    bool KFClusterClientModule::RepeatToProxy( uint64 shardid, uint32 msgid, const char* data, uint32 length )
    {
        if ( !_cluster_in_services )
        {
            // 不在服务中
            AddSendKeeper( shardid, msgid, data, length );
        }
        else
        {
            if ( _send_keeper.empty() )
            {
                // 发送队列为空
                auto ok = _kf_tcp_client->SendNetMessage( _cluster_proxy_id, shardid, msgid, data, length );
                if ( !ok )
                {
                    AddSendKeeper( shardid, msgid, data, length );
                }
            }
            else
            {
                AddSendKeeper( shardid, msgid, data, length );
            }
        }

        return true;
    }

    void KFClusterClientModule::AddSendKeeper( uint64 shardid, uint32 msgid, const char* data, uint32 length )
    {
        auto keeper = __KF_NEW__( SendKeeper, length );
        keeper->_shard_id = shardid;
        keeper->_msg_id = msgid;
        memcpy( keeper->_data, data, length );
        _send_keeper.push_back( keeper );
    }

    void KFClusterClientModule::RunSendKeeper()
    {
        while ( !_send_keeper.empty() )
        {
            // 把消息发送出去
            auto keeper = _send_keeper.front();
            auto ok = _kf_tcp_client->SendNetMessage( _cluster_proxy_id, keeper->_shard_id, keeper->_msg_id, keeper->_data, keeper->_length );
            if ( !ok )
            {
                // 发送失败, 有可能是消息队列满了, 下一帧继续发送
                break;
            }

            _send_keeper.pop_front();
            __KF_DELETE__( SendKeeper, keeper );
        }
    }
}