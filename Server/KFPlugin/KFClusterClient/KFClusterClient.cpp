#include "KFClusterClient.h"
#include "KFProtocol/KFProtocol.h"
#include "KFClusterClientConfig.h"
#include "KFClusterClientModule.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    KFClusterClient::KFClusterClient()
    {
        _cluster_proxy_id = 0;
        _cluster_in_services = false;
    }

    KFClusterClient::~KFClusterClient()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterClient::OnConnectionClusterServer( const std::string& servername, const std::string& servertype, uint32 serverid )
    {
        KFLogger::LogLogic( KFLogger::Info, "[%s] connect cluster server[%s:%s:%u]",
                            __FUNCTION__, servername.c_str(), servertype.c_str(), serverid );

        if ( servertype == _cluster_setting._type )
        {
            _cluster_setting._id = serverid;

            // 开启认证定时器
            __REGISTER_LOOP_TIMER__( serverid, 5000, &KFClusterClient::OnTimerSendClusterAuthMessage );
        }
        else if ( _cluster_proxy_type == servertype )
        {
            // 发送登录消息定时器
            __REGISTER_LOOP_TIMER__( serverid, 3000, &KFClusterClient::OnTimerSendClusterTokenMessage );
        }
    }

    void KFClusterClient::OnLostClusterServer( const std::string& servertype, uint32 serverid )
    {
        if ( servertype != _cluster_proxy_type )
        {
            return;
        }

        // 重新启动连接
        StartClusterMasterClient();
    }

    void KFClusterClient::StartClusterMasterClient()
    {
        // 断开了proxy服务器
        _kf_tcp_client->CloseClient( _cluster_proxy_id, __FUNCTION_LINE__ );
        _cluster_proxy_id = 0;
        _cluster_proxy_type.clear();
        _cluster_in_services = false;

        auto* kfaddress = _kf_ip_address->FindIpAddress( _cluster_setting._name, _cluster_setting._type, _cluster_setting._id );
        if ( kfaddress == nullptr )
        {
            return KFLogger::LogSystem( KFLogger::Error, "[%s] can't find cluster[%s:%s:%u] address!", __FUNCTION__,
                                        _cluster_setting._name.c_str(), _cluster_setting._type.c_str(), _cluster_setting._id );
        }

        _kf_tcp_client->StartClient( kfaddress->_app_name, kfaddress->_app_type, kfaddress->_app_id, kfaddress->_ip, kfaddress->_port );
    }


    __KF_TIMER_FUNCTION__( KFClusterClient::OnTimerSendClusterAuthMessage )
    {
        // 请求认证
        KFMsg::S2SClusterAuthReq req;
        req.set_clusterkey( _cluster_setting._key );
        req.set_clustertype( _cluster_setting._name );
        auto ok = _kf_tcp_client->SendNetMessage( _cluster_setting._id, KFMsg::S2S_CLUSTER_AUTH_REQ, &req );
        if ( !ok )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] send cluster[%u] auth failed!",
                                __FUNCTION__, _cluster_setting._id );
        }
    }

    void KFClusterClient::ProcessClusterAuth( const std::string& name, const std::string& type, uint32 id, const std::string& ip, uint32 port, const std::string& token )
    {
        // 停止定时器
        __UNREGISTER_TIMER__();

        // 删除master连接
        _kf_tcp_client->CloseClient( _cluster_setting._id, __FUNCTION_LINE__ );

        _auth_token = token;
        _cluster_proxy_id = id;
        _cluster_proxy_type = type;

        // 连接新的服务器
        _kf_tcp_client->StartClient( name, type, id, ip, port );
    }

    __KF_TIMER_FUNCTION__( KFClusterClient::OnTimerSendClusterTokenMessage )
    {
        // 请求认证
        KFMsg::S2SClusterVerifyReq req;
        req.set_token( _auth_token );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        req.set_clustertype( _cluster_setting._name );
        auto ok = _kf_tcp_client->SendNetMessage( objectid, KFMsg::S2S_CLUSTER_VERIFY_REQ, &req );
        if ( !ok )
        {
            StartClusterMasterClient();
        }
    }

    void KFClusterClient::ProcessClusterVerify( uint32 serverid )
    {
        // 停止定时器
        __UNREGISTER_TIMER__();

        if ( serverid != 0 )
        {
            KFLogger::LogSystem( KFLogger::Info, "[%s] cluster services ok!", _cluster_setting._name.c_str() );

            // 设置可以服务
            _cluster_in_services = true;

            // 回调函数
            _cluster_client_module->CallClusterConnectionFunction( _cluster_setting._name, serverid );
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

    bool KFClusterClient::SendNetMessage( uint32 objectid, uint32 msgid, google::protobuf::Message* message )
    {
        auto data = message->SerializeAsString();
        return SendNetMessage( objectid, msgid, data.data(), data.size() );
    }

    bool KFClusterClient::SendNetMessage( uint32 msgid, const char* data, uint32 length )
    {
        if ( !_cluster_in_services )
        {
            KFLogger::LogLogic( KFLogger::Error, "cluster[%s] is not in services! msgid[%u]",
                                _cluster_setting._name.c_str(), msgid );
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _cluster_proxy_id, msgid, data, length );
    }

    bool KFClusterClient::SendNetMessage( uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        if ( !_cluster_in_services )
        {
            KFLogger::LogLogic( KFLogger::Error, "cluster[%s] is not in services! objectid[%u] msgid[%u]",
                                _cluster_setting._name.c_str(), objectid, msgid );
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _cluster_proxy_id, objectid, msgid, data, length );
    }
}