#ifndef __KF_RANK_PROXY_MODULE_H__
#define __KF_RANK_PROXY_MODULE_H__

/************************************************************************
//    @Module			:    帮派功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/


#include "KFrame.h"
#include "KFRankProxyInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFClusterProxy/KFClusterProxyInterface.h"

namespace KFrame
{
    class KFRankProxyModule : public KFRankProxyInterface
    {
    public:
        KFRankProxyModule();
        ~KFRankProxyModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    };
}

#endif