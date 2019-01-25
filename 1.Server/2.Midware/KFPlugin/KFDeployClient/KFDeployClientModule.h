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

namespace KFrame
{
    class KFDeployClientModule : public KFDeployClientInterface
    {
    public:
        KFDeployClientModule() = default;
        ~KFDeployClientModule() = default;

        // 逻辑
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void ShutDown();

    protected:
        // 关闭服务器
        __KF_MESSAGE_FUNCTION__( HandleDeployCommandToMasterReq );
    };
}

#endif