#ifndef __KF_CLUSTER_NODE_MODULE_H__
#define __KF_CLUSTER_NODE_MODULE_H__

/************************************************************************
//    @Module			:    集群节点模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-18
************************************************************************/

#include "KFrame.h"
#include "KFClusterProxyInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    class KFClusterToken
    {
    public:
        KFClusterToken()
        {
            _gate_id = 0;
            _valid_time = 0;
        }

    public:

        std::string _token;
        uint32 _gate_id;
        uint64 _valid_time;
    };

    ///////////////////////////////////////////////////////////////////////////////////

    class KFClusterProxyModule : public KFClusterProxyInterface
    {
    public:
        KFClusterProxyModule() = default;
        ~KFClusterProxyModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 逻辑
        virtual void Run();
        virtual void AfterRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        virtual uint32 SelectClusterShard( const char* data, bool cache = false );
        virtual uint32 SelectClusterShard( uint64 objectid, bool cache = false );
        virtual uint32 SelectClusterShard( const char* data, uint32 objectid, bool cache = false );

        // 发送消息到Shard
        virtual void SendToShard( uint32 msgid, ::google::protobuf::Message* message );
        virtual void SendToShard( uint32 msgid, const char* data, uint32 length );

        virtual bool SendToShard( uint32 shardid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToShard( uint32 shardid, uint32 msgid, const char* data, uint32 length );

        virtual bool SendToShard( uint32 shardid, uint32 clientid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToShard( uint32 shardid, uint32 clientid, uint32 msgid, const char* data, uint32 length );

        // 发送消息到客户端
        virtual void SendToClient( uint32 msgid, ::google::protobuf::Message* message );
        virtual void SendToClient( uint32 msgid, const char* data, uint32 length );

        virtual bool SendToClient( uint32 clientid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToClient( uint32 clientid, uint32 msgid, const char* data, uint32 length );

        ////////////////////////////////////////////////////////////////////////////////
        // 添加映射
        virtual void AddDynamicShard( uint64 objectid, uint32 shardid );

        // 查找映射
        virtual uint32 FindDynamicShard( uint64 objectid );

        // 查找负载最小的逻辑分片id
        virtual uint32 FindMinObjectShard();

        // 查找映射
        virtual uint32 FindStaticShard( uint32 objectid );

    protected:
        // 处理更新token信息
        __KF_MESSAGE_FUNCTION__( HandleClusterTokenReq );

        // 验证toekn
        __KF_MESSAGE_FUNCTION__( HandleClusterVerifyReq );

        // 添加对象映射
        __KF_MESSAGE_FUNCTION__( HandleAddObjectToProxyReq );

        // 删除对象映射
        __KF_MESSAGE_FUNCTION__( HandleRemoveObjectToProxyReq );

        // 分配shard
        __KF_MESSAGE_FUNCTION__( HandleAllocObjectToProxyAck );

        // 发送到动态对象
        __KF_MESSAGE_FUNCTION__( HandleSendToStaticObjectReq );

        // 发送到动态对象
        __KF_MESSAGE_FUNCTION__( HandleSendToDynamicObjectReq );

    protected:
        // 转发消息到Shard
        __KF_TRANSMIT_FUNCTION__( TransmitMessageToShard );

        // 转发消息到Client
        __KF_TRANSMIT_FUNCTION__( TransmitMessageToClient );

    protected:
        // 更新
        __KF_TIMER_FUNCTION__( OnTimerSendClusterUpdateMessage );

    protected:
        // 连接成功
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectionServer );

        // 连接断开
        __KF_CLIENT_LOST_FUNCTION__( OnClientLostServer );

        // 发现连接
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDiscoverClient );

    private:
        // 连接Cluster Server 成功
        void OnClientConnectionClusterMaster( const std::string& servername, uint32 serverid );

        // 连接Cluster Server 成功
        void OnClientConnectionClusterShard( const std::string& servername, uint32 serverid );

        // 断开连接
        void OnClientLostClusterMaster( const std::string& servername, uint32 serverid );
        void OnClientLostClusterShard( const std::string& servername, uint32 serverid );

        // 检查token有效时间
        void RunClusterTokenValidTime();

        // 删除token
        void RunRemoveClusterToken();

        // 验证集群客户端登录
        uint32 ClusterVerifyLogin( const std::string& token, uint32 serverid );

        // 删除对象映射
        virtual void RemoveObjectShard( uint32 shardid );

        // 添加数量
        void AddObjectCount( uint32 shardid, uint32 count );

        // 减少数量
        void DecObjectCount( uint32 shardid, uint32 count );

    private:
        // masterid
        uint32 _master_server_id{ 0 };

        // 是否在服务中
        bool _in_service{ false };

        // 认证的token列表
        KFMap< std::string, const std::string&, KFClusterToken > _kf_token_list;

        // 需要删除的token列表
        std::set< std::string > _kf_remove_list;

        // hash一致性列表
        KFConHash _kf_hash;

        // 对象映射列表
        std::map< uint64, uint32 > _kf_dynamic_shard;

        // 对象数量
        std::map< uint64, uint32 > _kf_object_count;

        // 分配列表
        std::map< uint32, uint32 > _kf_static_shard;
    };
}
#endif