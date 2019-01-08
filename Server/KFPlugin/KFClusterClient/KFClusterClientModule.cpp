#include "KFClusterClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{

    void KFClusterClientModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFClusterClientModule::OnClientLostServer );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFClusterClientModule::OnClientConnectionServer );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK, &KFClusterClientModule::HandleClusterAuthToClientAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_TO_CLIENT_ACK, &KFClusterClientModule::HandleClusterVerifyToClientAck );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFClusterClientModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_TO_CLIENT_ACK );

    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    __KF_CLIENT_CONNECT_FUNCTION__( KFClusterClientModule::OnClientConnectionServer )
    {
        // 转发集群的名字
        static auto _option = _kf_option->FindOption( __KF_STRING__( clustername ) );
        if ( _option->_str_value != servername )
        {
            return;
        }

        auto strid = KFAppID::ToString( serverid );
        __LOG_DEBUG__( "connect route cluster server[{}:{}:{}]", servername, servertype, strid );

        if ( servertype == __KF_STRING__( master ) )
        {
            // 保存信息
            _str_master_id = strid;
            _cluster_name = servername;
            _cluster_master_id = serverid;

            // 开启认证定时器
            __REGISTER_LOOP_TIMER__( serverid, 5000, &KFClusterClientModule::OnTimerSendClusterAuthMessage );
        }
        else if ( servertype == __KF_STRING__( proxy ) )
        {
            // 发送登录消息定时器
            __REGISTER_LOOP_TIMER__( serverid, 3000, &KFClusterClientModule::OnTimerSendClusterTokenMessage );
        }
    }

    __KF_TIMER_FUNCTION__( KFClusterClientModule::OnTimerSendClusterAuthMessage )
    {
        static auto _option = _kf_option->FindOption( __KF_STRING__( clusterkey ) );

        // 请求认证
        KFMsg::S2SClusterAuthToMasterReq req;
        req.set_clusterkey( _option->_str_value );
        req.set_clientid( KFGlobal::Instance()->_app_id._union._id );
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
        req.set_serverid( KFGlobal::Instance()->_app_id._union._id );
        auto ok = _kf_tcp_client->SendNetMessage( objectid, KFMsg::S2S_CLUSTER_VERIFY_TO_PROXY_REQ, &req );
        if ( !ok )
        {
            // 删除定时器
            __UNREGISTER_OBJECT_TIMER__( objectid );

            // 重新连接
            ReconnectClusterMaster();
        }
    }

    __KF_CLIENT_LOST_FUNCTION__( KFClusterClientModule::OnClientLostServer )
    {
        // 不是转发集群
        if ( _cluster_name != servername || servertype != __KF_STRING__( proxy ) )
        {
            return;
        }

        // 重新启动连接
        ReconnectClusterMaster();
    }

    __KF_MESSAGE_FUNCTION__( KFClusterClientModule::HandleClusterAuthToClientAck )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterAuthToClientAck );

        _auth_token = kfmsg.token();
        _cluster_proxy_id = kfmsg.id();

        // 停止定时器
        __UNREGISTER_OBJECT_TIMER__( _cluster_master_id );

        // 删除master连接
        _kf_tcp_client->CloseClient( _cluster_master_id, __FUNC_LINE__ );

        // 连接新的服务器
        _kf_tcp_client->StartClient( kfmsg.name(), kfmsg.type(), kfmsg.id(), kfmsg.ip(), kfmsg.port() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterClientModule::HandleClusterVerifyToClientAck )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterVerifyToClientAck );

        // 停止定时器
        __UNREGISTER_OBJECT_TIMER__( _cluster_proxy_id );

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

            __LOG_INFO__( "[{}][{}] cluster services ok!", _cluster_name, KFAppID::ToString( _cluster_proxy_id ) );
        }
    }

    void KFClusterClientModule::ReconnectClusterMaster()
    {
        // 断开proxy服务器
        _kf_tcp_client->CloseClient( _cluster_proxy_id, __FUNC_LINE__ );

        _cluster_proxy_id = 0;
        _cluster_in_services = false;

        auto* kfaddress = _kf_ip_address->FindIpAddress( _cluster_name, __KF_STRING__( master ), _str_master_id );
        if ( kfaddress == nullptr )
        {
            return __LOG_ERROR__( "can't [{}:{}:{}] address!", _cluster_name, __KF_STRING__( master ), _str_master_id );
        }

        _kf_tcp_client->StartClient( kfaddress->_app_name, kfaddress->_app_type, kfaddress->_app_id, kfaddress->_ip, kfaddress->_port );
    }
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    bool KFClusterClientModule::SendToProxy( uint32 msgid, google::protobuf::Message* message )
    {
        return SendToProxy( _invalid_int, msgid, message );
    }

    bool KFClusterClientModule::SendToProxy( uint64 shardid, uint32 msgid, google::protobuf::Message* message )
    {
        if ( !_cluster_in_services )
        {
            __LOG_ERROR__( "cluster not in service, send msgid[{}] failed!", msgid );
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _cluster_proxy_id, shardid, msgid, message );
    }
}