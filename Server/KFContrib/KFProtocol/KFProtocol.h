#ifndef __KF_PROTOCOL_H__
#define __KF_PROTOCOL_H__
#include "KFInclude.h"

#include "KFCommonMessage.pb.h"
#include "KFClusterMessage.pb.h"
#include "KFRouteMessage.pb.h"
#include "KFFrameMessage.pb.h"
#include "KFDataMessage.pb.h"
#include "KFClientMessage.pb.h"
#include "KFServerMessage.pb.h"
#include "KFBattleMessage.pb.h"
#include "KFMatchMessage.pb.h"

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
    };

}

#endif
