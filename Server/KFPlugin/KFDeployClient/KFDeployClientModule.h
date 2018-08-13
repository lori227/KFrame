#ifndef __KF_DEPLOY_CLIENT_MODULE_H__
#define __KF_DEPLOY_CLIENT_MODULE_H__


/************************************************************************
//    @Module			:    部署Client
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-2
************************************************************************/

#include "KFrame.h"
#include "KFDeployClientInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFDeployCommand/KFDeployCommandInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFDeployClientModule : public KFDeployClientInterface
    {
    public:
        KFDeployClientModule();
        ~KFDeployClientModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void ShutDown();

    protected:
        // 连接成功
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectDeployServer );

        // 启动服务器
        __KF_TIMER_FUNCTION__( OnTimerGetAgentIpAddress );

    protected:
        // 获得Agent地址
        __KF_MESSAGE_FUNCTION__( HandleGetAgentIpAddressAck );

        // 关闭服务器
        __KF_MESSAGE_FUNCTION__( HandleDeployCommandToMasterReq );

    private:

        // 部署serverid
        uint32 _deploy_server_id;

        // 部署agentid
        uint32 _deploy_agent_id;
    };
}

#endif