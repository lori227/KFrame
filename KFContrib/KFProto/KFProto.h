#ifndef __KF_PROTO_H__
#define __KF_PROTO_H__
#include "KFInclude.h"

////////////////////////////////////////////////////////////////////

#define __PROTO_PARSE__( msgtype ) \
    msgtype kfmsg;\
    if ( !KFProto::Parse( &kfmsg, data, length ) )\
    {\
        return;\
    }\

namespace KFrame
{
    class KFProto
    {
    public:
        // 解析消息
        static bool Parse( ::google::protobuf::Message* proto, const int8* data, uint32 length );

        ///////////////////////////////////////////////////////////////////////////////////////////
        // 序列化字串游戏中
        static std::string& Serialize( const ::google::protobuf::Message* proto, uint32 mask );

        // 解析
        static bool Parse( ::google::protobuf::Message* proto, const std::string& data, uint32 mask );

        ///////////////////////////////////////////////////////////////////////////////////////////
    };
}

#endif
