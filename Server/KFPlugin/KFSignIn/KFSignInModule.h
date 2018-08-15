#ifndef __KF_SIGNIN_MODULE_H__
#define __KF_SIGNIN_MODULE_H__

/************************************************************************
//    @Moudle			:    签到模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-28
************************************************************************/

#include "KFrame.h"
#include "KFSignInInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"

namespace KFrame
{
    class KFSignInModule : public KFSignInInterface
    {
    public:
        KFSignInModule() = default;
        ~KFSignInModule() = default;

        // 加载配置
        virtual void InitModule();

        virtual void BeforeRun();

        virtual void ShutDown();

    protected:
        // 领取7天签到奖励
        __KF_MESSAGE_FUNCTION__( HandleReceiveSevenRewardReq );
    };
}



#endif