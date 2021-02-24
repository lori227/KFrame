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
        std::string RequestLogin( KFJson& json, std::shared_ptr<const KFChannelSetting> setting );

        // 充值请求
        std::string RequestPay( const std::string& data, std::shared_ptr<const KFChannelSetting> setting );
    };
}


#endif
