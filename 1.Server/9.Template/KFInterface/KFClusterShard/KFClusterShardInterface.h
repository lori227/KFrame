#ifndef __KF_CLUSTER_SHARD_INTERFACE_H__
#define __KF_CLUSTER_SHARD_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFClusterShardInterface : public KFModule
    {
    public:
        // 发送消息到proxy
        virtual void SendToProxy( uint32 msgid, google::protobuf::Message* message ) = 0;
        virtual bool SendToProxy( uint64 proxyid, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 发送消息到client
        virtual bool SendToClient( uint32 msgid, google::protobuf::Message* message ) = 0;
        virtual bool SendToClient( uint64 clientid, uint32 msgid, google::protobuf::Message* message ) = 0;
        virtual bool SendToClient( uint64 proxyid, uint64 clientid, uint32 msgid, google::protobuf::Message* message ) = 0;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_cluster_shard, KFClusterShardInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif