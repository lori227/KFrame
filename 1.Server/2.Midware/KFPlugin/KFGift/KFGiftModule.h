#ifndef __KF_GIFT_MODULE_H__
#define __KF_GIFT__MODULE_H__

/************************************************************************
//    @Module			:    礼包系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-17
************************************************************************/

#include "KFrame.h"
#include "KFGiftInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFPlatform/KFPlatformInterface.h"

namespace KFrame
{
    class KFGiftModule : public KFGiftInterface
    {
    public:
        KFGiftModule() = default;
        ~KFGiftModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 领取礼包奖励
        __KF_MESSAGE_FUNCTION__( HandleReceiveGiftRewardReq );
        __KF_HTTP_CALL_BACK_FUNCTION__( HandleReceiveGiftResult );

        // 绑定支付宝
        __KF_MESSAGE_FUNCTION__( HandleBindAlipayReq );
        __KF_HTTP_CALL_BACK_FUNCTION__( OnBindAlipayCallBack );

        // 查询邀请积分
        __KF_MESSAGE_FUNCTION__( HandleQueryInviteScoreReq );
        __KF_HTTP_CALL_BACK_FUNCTION__( OnQueryInviteScoreCallBack );

        // 兑换邀请积分
        __KF_MESSAGE_FUNCTION__( HandleExchangeInviteScoreReq );
        __KF_HTTP_CALL_BACK_FUNCTION__( OnExchangeInviteScoreCallBack );

        // 查询兑换记录
        __KF_MESSAGE_FUNCTION__( HandleExChangeInviteRecordReq );
        __KF_HTTP_CALL_BACK_FUNCTION__( OnExChangeInviteRecordCallBack );

    };
}



#endif