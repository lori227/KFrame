#ifndef __KF_GROUP_PROXY_MODULE_H__
#define __KF_GROUP_PROXY_MODULE_H__

/************************************************************************
//    @Module			:	 匹配小组逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-4-4
************************************************************************/

#include "KFrame.h"
#include "KFGroupProxyInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterProxy/KFClusterProxyInterface.h"

namespace KFrame
{
    class KFGroupProxyModule : public KFGroupProxyInterface
    {
    public:
        KFGroupProxyModule();
        ~KFGroupProxyModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 创建的队伍
        __KF_MESSAGE_FUNCTION__( HandleCreateMatchGroupReq );
    };
}



#endif