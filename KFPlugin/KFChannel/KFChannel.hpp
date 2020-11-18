#ifndef __KF_CHANNEL_H__
#define __KF_CHANNEL_H__

#include "KFXmlReader/KFChannelConfig.hpp"
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
        virtual std::string RequestPay( const std::string& data, const KFChannelSetting* kfsetting ) = 0;

    protected:
        // 保存订单信息
        bool SavePayData( const std::string& order, StringMap& values );
    };
}


#endif
