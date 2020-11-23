#ifndef __KF_SIGN_MODULE_H__
#define __KF_SIGN_MODULE_H__

/************************************************************************
//    @Module			:    签到模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-28
************************************************************************/

#include "KFSignInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFReset/KFResetInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFConfig/KFSignConfig.hpp"

namespace KFrame
{
    class KFSignModule : public KFSignInterface
    {
    public:
        KFSignModule() = default;
        ~KFSignModule() = default;

        virtual void BeforeRun();

        virtual void ShutDown();

    protected:
        // 领取7天签到奖励
        __KF_MESSAGE_FUNCTION__( HandleReceiveSevenRewardReq );

    protected:
        // 签到逻辑
        __KF_RESET_FUNCTION__( OnResetSigninData );

        // 计算签到天数
        void CalcSignDay( KFEntity* player );

        // 计算连续签到
        void CalcContinuousSign( KFEntity* player, uint64 lastresettime, uint64 nowresettime );
    };
}



#endif