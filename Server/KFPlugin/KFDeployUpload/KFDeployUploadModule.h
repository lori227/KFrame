#ifndef __KF_DEPLOY_UPLOAD_MODULE_H__
#define __KF_DEPLOY_UPLOAD_MODULE_H__


/************************************************************************
//    @Module			:    部署上传
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-23
************************************************************************/

#include "KFDeployUploadInterface.h"
#include "KFFtp/KFFtpInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFDeployUploadModule : public KFDeployUploadInterface
    {
    public:
        KFDeployUploadModule();
        ~KFDeployUploadModule();

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void ShutDown();

    protected:
        // 上传文件
        __KF_HTTP_FUNCTION__( HandleUploadFile );
    };
}

#endif