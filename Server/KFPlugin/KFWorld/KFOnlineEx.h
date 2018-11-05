#ifndef __KF_ONLINE_EX_H__
#define __KF_ONLINE_EX_H__

#include "KFOnline.h"

namespace KFrame
{
    class KFOnlineEx : public KFOnline
    {
    public:
        KFOnlineEx() = default;
        virtual ~KFOnlineEx() = default;

        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息到Game
        virtual void SendToOnline( uint32 msgid, const char* data, uint32 length );
        virtual void SendToOnline( uint32 msgid, ::google::protobuf::Message* message );
    };
}


#endif