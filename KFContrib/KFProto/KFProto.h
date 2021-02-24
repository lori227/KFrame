#ifndef __KF_PROTO_H__
#define __KF_PROTO_H__
#include "KFDefine.h"

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
        static bool Parse( google::protobuf::Message* message, const int8* data, uint32 length );
        static bool Parse( std::shared_ptr<google::protobuf::Message> message, const int8* data, uint32 length );

        ///////////////////////////////////////////////////////////////////////////////////////////
        // 序列化字串游戏中
        static std::string& Serialize( const google::protobuf::Message* message, uint32 compress_type, uint32 compress_level, bool convert );
        static std::string& Serialize( std::shared_ptr<const google::protobuf::Message> message, uint32 compress_type, uint32 compress_level, bool convert );

        // 解析
        static bool Parse( google::protobuf::Message* message, const std::string& data, uint32 compress_type, bool convert );
        static bool Parse( std::shared_ptr<google::protobuf::Message> message, const std::string& data, uint32 compress_type, bool convert );

        ///////////////////////////////////////////////////////////////////////////////////////////
    };
}

#endif
