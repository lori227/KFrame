#ifndef __KF_MATCH_CLIENT_INTERFACE_H__
#define __KF_MATCH_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMatchClientInterface : public KFModule
    {
    public:
        // 发送消息到Match
        virtual bool SendMessageToMatch( uint32 msgid, ::google::protobuf::Message* message ) = 0;

        // 获得匹配最大人数
        virtual uint32 GetMatchMaxCount( uint32 matchid ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_match, KFMatchClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif