#ifndef __KF_INTERNAL_H__
#define __KF_INTERNAL_H__

#include "KFChannel.h"

namespace KFrame
{
    class KFInternal : public KFChannel
    {
    public:
        KFInternal() = default;
        virtual ~KFInternal() = default;

        // 登录请求
        std::string RequestLogin( KFJson& json, const KFChannelSetting* kfsetting );

        // 充值请求
        std::string RequestPay( KFJson& json, const KFChannelSetting* kfsetting );
    };
}


#endif
