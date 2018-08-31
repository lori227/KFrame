#ifndef __KF_GUILD_PROXY_MODULE_H__
#define __KF_GUILD_PROXY_MODULE_H__

/************************************************************************
//    @Module			:    帮派功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFrame.h"
#include "KFGuildProxyInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterProxy/KFClusterProxyInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFGuildProxyModule : public KFGuildProxyInterface
    {
    public:
        KFGuildProxyModule();
        ~KFGuildProxyModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        __KF_MESSAGE_FUNCTION__( HandleCreateGuildReq );
    };
}

#endif