#ifndef __KF_CHANNEL_H__
#define __KF_CHANNEL_H__

#include "KFrame.h"
#include "KFChannelConfig.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFChannel
    {
    public:
        KFChannel( uint32 channel );
        virtual ~KFChannel();

    public:
        // 渠道编号
        uint32 _channel;
    };
}


#endif
