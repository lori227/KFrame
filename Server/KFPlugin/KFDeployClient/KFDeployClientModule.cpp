#include "KFDeployClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFDeployClientModule::BeforeRun()
    {
        ////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_MASTER_REQ, &KFDeployClientModule::HandleDeployCommandToMasterReq );
    }

    void KFDeployClientModule::ShutDown()
    {
        /////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_MASTER_REQ );
    }

    void KFDeployClientModule::OnceRun()
    {
        auto agentdata = KFGlobal::Instance()->_startup_params[ __KF_STRING__( agent ) ];
        if ( agentdata.empty() )
        {
            return;
        }

        auto agentid = KFUtility::SplitString( agentdata, "|" );
        auto ip = KFUtility::SplitString( agentdata, "|" );
        auto port = KFUtility::SplitValue< uint32 >( agentdata, "port" );

        auto appid = KFAppID::ToUInt64( agentid );
        _kf_tcp_client->StartClient( __KF_STRING__( deploy ), __KF_STRING__( agent ), appid, ip, port );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployClientModule::HandleDeployCommandToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployCommandToMasterReq );

        auto pbdeploy = &kfmsg.deploycommand();

        // 关闭服务器
        _kf_command->DeployCommand( pbdeploy->command(), pbdeploy->value(), pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );

        // 发送到客户端
        if ( _kf_tcp_server != nullptr )
        {
            KFMsg::S2SDeployCommandToServerReq req;
            req.mutable_deploycommand()->CopyFrom( *pbdeploy );
            _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_SERVER_REQ, &req );
        }
    }

}