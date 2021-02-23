#ifndef __KF_STEAM__H__
#define __KF_STEAM__H__

#include "KFChannel.hpp"

namespace KFrame
{
    class KFSteam : public KFChannel
    {
    public:
        KFSteam() = default;
        virtual ~KFSteam() = default;

        // 登录请求
        std::string RequestLogin( KFJson& json, const KFChannelSetting* setting );

        // 充值请求
        std::string RequestPay( const std::string& data, const KFChannelSetting* setting );
    };
}


#endif
