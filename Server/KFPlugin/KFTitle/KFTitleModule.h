#ifndef __KF_TITLE_MODULE_H__
#define __KF_TITLE_MODULE_H__

/************************************************************************
//    @Module			:    称号系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-12-27
************************************************************************/

#include "KFTitleInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFTitleModule : public KFTitleInterface
    {
    public:
        KFTitleModule() = default;
        ~KFTitleModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 更换称号
        __KF_MESSAGE_FUNCTION__( HandleTitleChangeReq );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    };
}



#endif