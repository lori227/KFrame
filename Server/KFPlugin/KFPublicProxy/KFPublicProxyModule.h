#ifndef __KF_PUBLIC_PROXY_MODULE_H__
#define __KF_PUBLIC_PROXY_MODULE_H__

/************************************************************************
//    @Moudle			:    公共数据功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFrame.h"
#include "KFPublicProxyInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFPublicProxyModule : public KFPublicProxyInterface
    {
    public:
        KFPublicProxyModule();
        ~KFPublicProxyModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
    };
}

#endif