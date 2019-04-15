#ifndef __KF_DEPLOY_SERVICE_H__
#define __KF_DEPLOY_SERVICE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDeployService : public KFSingleton< KFDeployService >
    {
    public:
        // 开始服务
        void InitServices();

        // 关闭服务
        void ShutDown();

        // run
        void Run();

    };

    //////////////////////////////////////////////////////////////////////////////
    static auto _kf_deploy_service = KFDeployService::Instance();
    //////////////////////////////////////////////////////////////////////////////
}
#endif
