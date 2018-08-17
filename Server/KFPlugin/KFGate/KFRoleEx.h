#ifndef __KF_GATE_ROLEEX_H__
#define __KF_GATE_ROLEEX_H__

#include "KFRole.h"

namespace KFrame
{
    class KFRoleEx : public KFRole
    {
    public:
        KFRoleEx() = default;
        virtual ~KFRoleEx() = default;

        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////

        // 发送消息到Client
        virtual bool SendMessageToClient( uint32 msgid, const char* data, uint32 length );
        virtual bool SendMessageToClient( uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到Game
        virtual bool SendMessageToGame( uint32 msgid, const char* data, uint32 length );
        virtual bool SendMessageToGame( uint32 msgid, ::google::protobuf::Message* message );
    };
}



#endif