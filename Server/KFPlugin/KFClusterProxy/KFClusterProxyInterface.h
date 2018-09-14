#ifndef __KF_CLUSTER_PROXY_INTERFACE_H__
#define __KF_CLUSTER_PROXY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFClusterProxyInterface : public KFModule
    {
    public:

        // 选择Shard
        virtual uint32 SelectClusterShard( const char* data, bool cache = false ) = 0;
        virtual uint32 SelectClusterShard( uint64 objectid, bool cache = false ) = 0;
        virtual uint32 SelectClusterShard( const char* data, uint32 objectid, bool cache = false ) = 0;

        // 发送消息到Shard
        virtual void SendMessageToShard( uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual void SendMessageToShard( uint32 msgid, const char* data, uint32 length ) = 0;

        virtual bool SendMessageToShard( uint32 shardid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual bool SendMessageToShard( uint32 shardid, uint32 msgid, const char* data, uint32 length ) = 0;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息到客户端
        virtual void SendMessageToClient( uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual void SendMessageToClient( uint32 msgid, const char* data, uint32 length ) = 0;

        virtual bool SendMessageToClient( uint32 clientid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual bool SendMessageToClient( uint32 clientid, uint32 msgid, const char* data, uint32 length ) = 0;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加映射
        virtual void AddDynamicShard( uint64 objectid, uint32 shardid ) = 0;

        // 查找动态对象映射
        virtual uint32 FindDynamicShard( uint64 objectid ) = 0;

        // 查找负载最小的逻辑分片id
        virtual uint32 FindMinObjectShard() = 0;

        // 查找静态对象映射
        virtual uint32 FindStaticShard( uint32 objectid ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_cluster_proxy, KFClusterProxyInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif