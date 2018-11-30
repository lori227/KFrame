#ifndef __KF_CLUSTER_PROXY_INTERFACE_H__
#define __KF_CLUSTER_PROXY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFClusterProxyInterface : public KFModule
    {
    public:

        // 选择Shard
        virtual uint64 SelectClusterShard( const char* data, bool cache = false ) = 0;
        virtual uint64 SelectClusterShard( uint64 objectid, bool cache = false ) = 0;
        virtual uint64 SelectClusterShard( const char* data, uint64 objectid, bool cache = false ) = 0;

        // 发送消息到Shard
        virtual void SendToShard( uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual void SendToShard( uint32 msgid, const char* data, uint32 length ) = 0;

        virtual bool SendToShard( uint64 shardid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToShard( uint64 shardid, uint32 msgid, const char* data, uint32 length ) = 0;

        virtual bool SendToShard( uint64 shardid, uint64 clientid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToShard( uint64 shardid, uint64 clientid, uint32 msgid, const char* data, uint32 length ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息到客户端
        virtual void SendToClient( uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual void SendToClient( uint32 msgid, const char* data, uint32 length ) = 0;

        virtual bool SendToClient( uint64 clientid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToClient( uint64 clientid, uint32 msgid, const char* data, uint32 length ) = 0;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加映射
        virtual void AddDynamicShard( uint64 objectid, uint64 shardid ) = 0;

        // 查找动态对象映射
        virtual uint64 FindDynamicShard( uint64 objectid ) = 0;

        // 查找负载最小的逻辑分片id
        virtual uint64 FindMinObjectShard() = 0;

        // 查找静态对象映射
        virtual uint64 FindStaticShard( uint64 objectid ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_cluster_proxy, KFClusterProxyInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif