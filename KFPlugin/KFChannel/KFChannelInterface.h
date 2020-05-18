#ifndef __KF_CHANNEL_INTERFACE_H__
#define __KF_CHANNEL_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFChannelInterface : public KFModule
    {
    public:
        // 登陆验证
        virtual std::string AuthLogin( KFJson& request ) = 0;

        // 充值回调
        virtual std::string AuthPay( uint32 channel, const std::string& data ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_channel, KFChannelInterface );
    /////////////////////////////////////////////////////////////////////////////////////////
}


#endif