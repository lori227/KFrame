#ifndef __KF_CLUSTER_PROXY_INTERFACE_H__
#define __KF_CLUSTER_PROXY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFClusterProxyInterface : public KFModule
    {
    public:
        // 转发消息到shard
        virtual bool ForwardToShard( const Route& route, uint32 msg_id, const char* data, uint32 length ) = 0;
        virtual bool ForwardToShard( const Route& route, uint32 msg_id, const google::protobuf::Message* message ) = 0;

        // 转发消息到client
        virtual bool ForwardToClient( const Route& route, uint32 msg_id, const char* data, uint32 length ) = 0;
        virtual bool ForwardToClient( const Route& route, uint32 msg_id, const google::protobuf::Message* message ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_cluster_proxy, KFClusterProxyInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif