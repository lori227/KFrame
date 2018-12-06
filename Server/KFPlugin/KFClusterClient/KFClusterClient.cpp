#include "KFClusterClient.h"
#include "KFProtocol/KFProtocol.h"
#include "KFClusterClientModule.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterClient::OnConnectionClusterServer( const std::string& servername, const std::string& servertype, uint64 serverid )
    {
        auto strid = KFAppID::ToString( serverid );
        __LOG_DEBUG__( "connect cluster server[{}:{}:{}]", servername, servertype, strid );

        if ( servertype == __KF_STRING__( master ) )
        {
            _str_master_id = strid;
            _cluster_master_id = serverid;

            // 开启认证定时器
            __REGISTER_LOOP_TIMER__( serverid, 5000, &KFClusterClient::OnTimerSendClusterAuthMessage );
        }
        else if ( servertype == __KF_STRING__( proxy ) )
        {
            // 发送登录消息定时器
            __REGISTER_LOOP_TIMER__( serverid, 3000, &KFClusterClient::OnTimerSendClusterTokenMessage );
        }
    }

    void KFClusterClient::OnLostClusterServer( const std::string& servertype, uint64 serverid )
    {
        if ( servertype == __KF_STRING__( proxy ) )
        {
            // 重新启动连接
            StartClusterMasterClient();
        }
    }

    void KFClusterClient::StartClusterMasterClient()
    {
        // 断开了proxy服务器
        _kf_tcp_client->CloseClient( _cluster_proxy_id, __FUNC_LINE__ );

        _cluster_proxy_id = 0;
        _cluster_in_services = false;

        auto* kfaddress = _kf_ip_address->FindIpAddress( _cluster_master_name, __KF_STRING__( master ), _str_master_id );
        if ( kfaddress == nullptr )
        {
            return __LOG_ERROR__( "can't find cluster[{}:{}:{}:{}] address!", _cluster_master_name, __KF_STRING__( master ), _cluster_master_id, _str_master_id );
        }

        _kf_tcp_client->StartClient( kfaddress->_app_name, kfaddress->_app_type, kfaddress->_app_id, kfaddress->_ip, kfaddress->_port );
    }


    __KF_TIMER_FUNCTION__( KFClusterClient::OnTimerSendClusterAuthMessage )
    {
        if ( _cluster_master_name.empty() )
        {
            return;
        }

        auto _cluster_key = _kf_option->GetString( __KF_STRING__( clusterkey ), _cluster_master_name );
        if ( _cluster_key.empty() )
        {
            return __LOG_ERROR__( "[{}] can't find clusterkey!", _cluster_master_name );
        }

        // 请求认证
        KFMsg::S2SClusterAuthReq req;
        req.set_clusterkey( _cluster_key );
        req.set_clustertype( _cluster_master_name );
        auto ok = _kf_tcp_client->SendNetMessage( _cluster_master_id, KFMsg::S2S_CLUSTER_AUTH_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "send cluster[{}:{}] auth failed!", _cluster_master_id, _str_master_id );
        }
    }

    void KFClusterClient::ProcessClusterAuth( const std::string& name, const std::string& type, uint64 id, const std::string& ip, uint32 port, const std::string& token )
    {
        // 停止定时器
        __UNREGISTER_OBJECT_TIMER__( _cluster_master_id );

        // 删除master连接
        _kf_tcp_client->CloseClient( _cluster_master_id, __FUNC_LINE__ );

        _auth_token = token;
        _cluster_proxy_id = id;

        // 连接新的服务器
        _kf_tcp_client->StartClient( name, type, id, ip, port );
    }

    __KF_TIMER_FUNCTION__( KFClusterClient::OnTimerSendClusterTokenMessage )
    {
        // 请求认证
        KFMsg::S2SClusterVerifyReq req;
        req.set_token( _auth_token );
        req.set_clustertype( _cluster_master_name );
        req.set_serverid( KFGlobal::Instance()->_app_id._union._id );
        auto ok = _kf_tcp_client->SendNetMessage( objectid, KFMsg::S2S_CLUSTER_VERIFY_REQ, &req );
        if ( !ok )
        {
            StartClusterMasterClient();
        }
    }

    void KFClusterClient::ProcessClusterVerify( uint64 serverid )
    {
        // 停止定时器
        __UNREGISTER_OBJECT_TIMER__( _cluster_proxy_id );

        if ( serverid != 0 )
        {
            __LOG_INFO__( "[{}] cluster services ok!", _cluster_master_name );

            // 设置可以服务
            _cluster_in_services = true;

            // 回调函数
            _cluster_client_module->CallClusterConnectionFunction( _cluster_master_name, serverid );
        }
        else
        {
            // 重新连接
            StartClusterMasterClient();
        }
    }

    bool KFClusterClient::SendNetMessage( uint32 msgid, google::protobuf::Message* message )
    {
        auto data = message->SerializeAsString();
        return SendNetMessage( msgid, data.data(), data.size() );
    }

    bool KFClusterClient::SendNetMessage( uint64 objectid, uint32 msgid, google::protobuf::Message* message )
    {
        auto data = message->SerializeAsString();
        return SendNetMessage( objectid, msgid, data.data(), data.size() );
    }

    bool KFClusterClient::SendNetMessage( uint32 msgid, const char* data, uint32 length )
    {
        if ( !_cluster_in_services )
        {
            __LOG_ERROR__( "cluster[{}] not in services! msgid[{}]", _cluster_master_name, msgid );
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _cluster_proxy_id, msgid, data, length );
    }

    bool KFClusterClient::SendNetMessage( uint64 objectid, uint32 msgid, const char* data, uint32 length )
    {
        if ( !_cluster_in_services )
        {
            __LOG_ERROR__( "cluster[{}] not in services! objectid[{}] msgid[{}]", _cluster_master_name, objectid, msgid );
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _cluster_proxy_id, objectid, msgid, data, length );
    }
}