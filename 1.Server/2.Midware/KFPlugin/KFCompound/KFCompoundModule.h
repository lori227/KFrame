#ifndef __KF_COMPOUND_MODULE_H__
#define __KF_COMPOUND_MODULE_H__

/************************************************************************
//    @Module			:    合成模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-6-28
************************************************************************/

#include "KFrame.h"
#include "KFCompoundInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"

namespace KFrame
{
    class KFCompoundModule : public KFCompoundInterface
    {
    public:
        KFCompoundModule() = default;
        ~KFCompoundModule() = default;

        // 加载配置
        virtual void InitModule();

        virtual void BeforeRun();

        virtual void ShutDown();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 请求合成
        __KF_MESSAGE_FUNCTION__( HandleCompoundDataReq );
    };
}

#endif