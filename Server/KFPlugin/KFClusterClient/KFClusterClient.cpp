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
        _cluster_node_id = 0;
        _cluster_in_services = false;
    }

    KFClusterClient::~KFClusterClient()
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterClient::StartClusterClient( KFClusterClientModule* module, const KFConnection* setting )
    {
        _cluster_client_module = module;
        _cluster_setting = *setting;

        _kf_tcp_client->StartClient( setting->_type, setting->_id, setting->_name, setting->_ip, setting->_port );
    }

    void KFClusterClient::OnConnectionClusterServer( const std::string& servertype, uint32 serverid )
    {
        KFLogger::LogLogic( KFLogger::Info, "[%s] connect cluster server[%s:%u]",
                            __FUNCTION__, servertype.c_str(), serverid );

        if ( servertype == _cluster_setting._type )
        {
            // 开启认证定时器
            if ( serverid == _cluster_setting._id )
            {
                _kf_timer->RegisterLoopTimer( _cluster_setting._id, 5000, this, &KFClusterClient::OnTimerSendClusterAuthMessage );
            }
        }
        else if ( _cluster_node_type == servertype )
        {
            // 发送登录消息定时器
            if ( serverid == _cluster_node_id )
            {
                _kf_timer->RegisterLimitTimer( _cluster_node_id, 1000, 1, this, &KFClusterClient::OnTimerSendClusterTokenMessage );
            }
        }
    }

    void KFClusterClient::OnLostClusterServer( const std::string& servertype, uint32 serverid )
    {
        if ( serverid != _cluster_node_id )
        {
            return;
        }

        _cluster_in_services = false;

        _kf_tcp_client->CloseClient( _cluster_node_id );
        StartClusterClient( _cluster_client_module, &_cluster_setting );
    }

    __KF_TIMER_FUNCTION__( KFClusterClient::OnTimerSendClusterAuthMessage )
    {
        // 请求认证
        KFMsg::S2SClusterAuthReq req;
        req.set_clusterkey( _cluster_setting._key );
        req.set_clustertype( _cluster_setting._name );
        _kf_tcp_client->SendNetMessage( objectid, KFMsg::S2S_CLUSTER_AUTH_REQ, &req );
    }

    void KFClusterClient::ProcessClusterAuth( const std::string& servertype, uint32 serverid, const std::string& name, const std::string& ip, uint32 port, const std::string& token )
    {
        // 停止定时器
        _kf_timer->UnRegisterTimer( this, _cluster_setting._id );

        // 删除连接
        _kf_tcp_client->CloseClient( _cluster_setting._id );

        _auth_token = token;
        _cluster_node_type = servertype;
        _cluster_node_id = serverid;

        // 连接新的服务器
        _kf_tcp_client->StartClient( servertype, serverid, name, ip, port );
    }

    __KF_TIMER_FUNCTION__( KFClusterClient::OnTimerSendClusterTokenMessage )
    {
        // 请求认证
        KFMsg::S2SClusterVerifyReq req;
        req.set_token( _auth_token );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        req.set_clustertype( _cluster_setting._name );
        _kf_tcp_client->SendNetMessage( objectid, KFMsg::S2S_CLUSTER_VERIFY_REQ, &req );
    }

    void KFClusterClient::ProcessClusterVerify( uint32 serverid )
    {
        // 停止定时器
        _kf_timer->UnRegisterTimer( this, _cluster_node_id );

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
            StartClusterClient( _cluster_client_module, &_cluster_setting );
            _kf_tcp_client->CloseClient( _cluster_node_id );
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
            KFLogger::LogSystem( KFLogger::Error, "cluster[%s] is not in services! msgid[%u]",
                                 _cluster_setting._name.c_str(), msgid );
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _cluster_node_id, msgid, data, length );
    }

    bool KFClusterClient::SendNetMessage( uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        if ( !_cluster_in_services )
        {
            KFLogger::LogSystem( KFLogger::Error, "cluster[%s] is not in services! objectid[%u] msgid[%u]",
                                 _cluster_setting._name.c_str(), objectid, msgid );
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _cluster_node_id, objectid, msgid, data, length );
    }
}