#include "KFDeployClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFDeployClientModule::KFDeployClientModule()
    {
        _deploy_server_id = 0;
        _deploy_agent_id = 0;
    }

    KFDeployClientModule::~KFDeployClientModule()
    {

    }

    void KFDeployClientModule::InitModule()
    {
    }

    void KFDeployClientModule::BeforeRun()
    {
        _kf_timer->RegisterLoopTimer( 0, 5000, this, &KFDeployClientModule::OnTimerGetAgentIpAddress );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFDeployClientModule::OnClientConnectDeployServer );
        ////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_ACK, &KFDeployClientModule::HandleGetAgentIpAddressAck );
    }

    void KFDeployClientModule::ShutDown()
    {
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        _kf_timer->UnRegisterTimer( this );
        /////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_ACK );
    }

    /////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFDeployClientModule::OnClientConnectDeployServer )
    {
        // 连接成功
        if ( servername == KFField::_deploy && servertype == KFField::_server )
        {
            _deploy_server_id = serverid;
        }
    }

    __KF_TIMER_FUNCTION__( KFDeployClientModule::OnTimerGetAgentIpAddress )
    {
        // 还没有连接上server
        if ( _deploy_server_id == 0 )
        {
            return;
        }

        // 发消息取本机的agent地址
        KFMsg::S2SGetAgentIpAddressReq req;
        req.set_localip( _kf_ip_address->GetLocalIp() );
        _kf_tcp_client->SendNetMessage( _deploy_server_id, KFMsg::S2S_GET_AGENT_IP_ADDRESS_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployClientModule::HandleGetAgentIpAddressAck )
    {
        __PROTO_PARSE__( KFMsg::S2SGetAgentIpAddressAck );

        // 先关闭当前连接
        _kf_tcp_client->CloseClient( _deploy_server_id );
        _kf_timer->UnRegisterTimer( this, 0 );
        _deploy_server_id = 0;

        // 连接Agent
        _kf_tcp_client->StartClient( kfmsg.appname(), kfmsg.apptype(), kfmsg.appid(), kfmsg.ip(), kfmsg.port() );
    }
}