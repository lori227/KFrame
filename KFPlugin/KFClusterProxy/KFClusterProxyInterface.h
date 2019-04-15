#ifndef __KF_CLUSTER_PROXY_INTERFACE_H__
#define __KF_CLUSTER_PROXY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFClusterProxyInterface : public KFModule
    {
    public:
        // 转发消息到shard
        virtual bool TranspondToShard( const Route& route, uint32 msgid, const char* data, uint32 length ) = 0;

        // 转发消息到client
        virtual bool TranspondToClient( const Route& route, uint32 msgid, const char* data, uint32 length ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_cluster_proxy, KFClusterProxyInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif