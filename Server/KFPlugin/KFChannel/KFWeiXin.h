#ifndef __KF_WEI_XIN__H__
#define __KF_WEI_XIN__H__

#include "KFChannel.h"

namespace KFrame
{
    class KFWeiXin : public KFChannel
    {
    public:
        KFWeiXin( uint32 channel );
        virtual ~KFWeiXin();

        // 登录请求
        std::string RequestLogin( KFJson& json, const KFChannelSetting* kfchannelsetting );

        // 充值请求
        std::string RequestPay( KFJson& json, const KFChannelSetting* kfchannelsetting );
    };
}


#endif
