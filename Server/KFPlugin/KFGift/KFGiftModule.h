#ifndef __KF_GIFT_MODULE_H__
#define __KF_GIFT__MODULE_H__

/************************************************************************
//    @Moudle			:    礼包系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-17
************************************************************************/

#include "KFrame.h"
#include "KFGiftInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFGiftModule : public KFGiftInterface
    {
    public:
        KFGiftModule();
        ~KFGiftModule();

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 领取礼包奖励
        __KF_MESSAGE_FUNCTION__( HandleReceiveGiftRewardReq );

    private:
        // 领取礼包回馈
        void HandleReceiveGiftResult( std::string& senddata, std::string& recvdata );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    };
}



#endif