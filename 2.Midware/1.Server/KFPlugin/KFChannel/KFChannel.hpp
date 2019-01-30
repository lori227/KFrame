#ifndef __KF_CHANNEL_H__
#define __KF_CHANNEL_H__

#include "KFrame.h"
#include "KFChannelConfig.hpp"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFChannel
    {
    public:
        KFChannel() = default;
        virtual ~KFChannel() = default;

        // 登录请求
        virtual std::string RequestLogin( KFJson& json, const KFChannelSetting* kfsetting ) = 0;

        // 充值请求
        virtual std::string RequestPay( KFJson& json, const KFChannelSetting* kfsetting ) = 0;
    };
}


#endif
