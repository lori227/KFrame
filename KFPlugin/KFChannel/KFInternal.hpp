#ifndef __KF_INTERNAL_H__
#define __KF_INTERNAL_H__

#include "KFChannel.hpp"

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
        std::string RequestPay( const std::string& data, const KFChannelSetting* kfsetting );
    };
}


#endif
