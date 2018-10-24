#include "KFDeployClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFDeployClientModule::KFDeployClientModule()
    {
        _deploy_server_id = 0;
        _deploy_agent_id = 0;
    }

    void KFDeployClientModule::BeforeRun()
    {
        __REGISTER_LOOP_TIMER__( 0, 5000, &KFDeployClientModule::OnTimerGetAgentIpAddress );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFDeployClientModule::OnClientConnectDeployServer );
        ////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_ACK, &KFDeployClientModule::HandleGetAgentIpAddressAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_MASTER_REQ, &KFDeployClientModule::HandleDeployCommandToMasterReq );
    }

    void KFDeployClientModule::ShutDown()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        /////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_MASTER_REQ );
    }

    /////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFDeployClientModule::OnClientConnectDeployServer )
    {
        // 连接成功
        if ( servername == __KF_STRING__( deploy ) && servertype == __KF_STRING__( server ) )
        {
            KFAppID kfappid( serverid );
            kfappid._union._app_data._channel_id = KFGlobal::Instance()->_app_channel;
            _deploy_server_id = kfappid._union._app_id;
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
        __UNREGISTER_TIMER__();
        _kf_tcp_client->CloseClient( _deploy_server_id, __FUNC_LINE__ );
        _deploy_server_id = 0;

        // 连接Agent
        auto appid = KFAppID::ToUInt32( kfmsg.appid() );
        _kf_tcp_client->StartClient( kfmsg.appname(), kfmsg.apptype(), appid, kfmsg.ip(), kfmsg.port() );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployClientModule::HandleDeployCommandToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployCommandToMasterReq );

        auto pbdeploy = &kfmsg.deploycommand();

        // 关闭服务器
        _kf_command->DeployCommand( pbdeploy->command(), pbdeploy->value(), pbdeploy->appchannel(),
                                    pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );

        // 发送到客户端
        if ( _kf_tcp_server != nullptr )
        {
            KFMsg::S2SDeployCommandToServerReq req;
            req.mutable_deploycommand()->CopyFrom( *pbdeploy );
            _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_SERVER_REQ, &req );
        }
    }

}