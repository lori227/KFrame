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
            _client_id = 0;
            _valid_time = 0;
        }

    public:

        std::string _token;
        uint64 _client_id;
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

        // 关闭
        virtual void BeforeShut();

    protected:
        // 处理更新token信息
        __KF_MESSAGE_FUNCTION__( HandleClusterTokenToProxyReq, KFMsg::S2SClusterTokenToProxyReq );

        // 验证toekn
        __KF_MESSAGE_FUNCTION__( HandleClusterVerifyToProxyReq, KFMsg::S2SClusterVerifyToProxyReq );

    protected:
        // 转发消息到shard
        virtual bool TranspondToShard( const Route& route, uint32 msg_id, const char* data, uint32 length );
        virtual bool TranspondToShard( const Route& route, uint32 msg_id, const google::protobuf::Message* message );

        // 转发消息到client
        virtual bool TranspondToClient( const Route& route, uint32 msg_id, const char* data, uint32 length );
        virtual bool TranspondToClient( const Route& route, uint32 msg_id, const google::protobuf::Message* message );

    protected:
        // 更新
        __KF_TIMER_FUNCTION__( OnTimerSendClusterRegisterMessage );

        // 认证超时
        __KF_TIMER_FUNCTION__( OnTimerClusterAuthTimeOut );

    protected:
        // 连接成功
        __KF_NET_EVENT_FUNCTION__( OnClientConnectionServer );

        // 连接断开
        __KF_NET_EVENT_FUNCTION__( OnClientLostServer );

        // 发现连接
        __KF_NET_EVENT_FUNCTION__( OnServerDiscoverClient );

        // 丢失连接
        __KF_NET_EVENT_FUNCTION__( OnServerLostClient );

    private:
        // 连接Cluster Server 成功
        void OnClientConnectionClusterMaster( const std::string& servername, uint64 server_id );

        // 连接Cluster Server 成功
        void OnClientConnectionClusterShard( const std::string& servername, uint64 server_id );

        // 断开连接
        void OnClientLostClusterMaster( const std::string& servername, uint64 server_id );
        void OnClientLostClusterShard( const std::string& servername, uint64 server_id );

        // 验证集群客户端登录
        uint64 ClusterVerifyLogin( const std::string& token, uint64 server_id );

    private:
        // 是否在服务中
        bool _in_service{ false };

        // 认证的token列表
        KFHashMap< std::string, KFClusterToken > _kf_token_list;

        // hash一致性列表
        KFConHash _kf_hash;
    };
}
#endif