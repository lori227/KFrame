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
        __KF_MESSAGE_FUNCTION__( HandleClusterTokenToProxyReq );

        // 验证toekn
        __KF_MESSAGE_FUNCTION__( HandleClusterVerifyToProxyReq );

    protected:
        // 转发消息到Shard
        __KF_TRANSMIT_FUNCTION__( TransmitMessageToShard );

        // 转发消息到Client
        __KF_TRANSMIT_FUNCTION__( TransmitMessageToClient );

    protected:
        // 更新
        __KF_TIMER_FUNCTION__( OnTimerSendClusterUpdateMessage );

        // 认证超时
        __KF_TIMER_FUNCTION__( OnTimerClusterAuthTimeOut );

    protected:
        // 连接成功
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectionServer );

        // 连接断开
        __KF_CLIENT_LOST_FUNCTION__( OnClientLostServer );

        // 发现连接
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDiscoverClient );

        // 丢失连接
        __KF_SERVER_LOST_FUNCTION__( OnServerLostClient );

    private:
        // 连接Cluster Server 成功
        void OnClientConnectionClusterMaster( const std::string& servername, uint64 serverid );

        // 连接Cluster Server 成功
        void OnClientConnectionClusterShard( const std::string& servername, uint64 serverid );

        // 断开连接
        void OnClientLostClusterMaster( const std::string& servername, uint64 serverid );
        void OnClientLostClusterShard( const std::string& servername, uint64 serverid );

        // 验证集群客户端登录
        uint64 ClusterVerifyLogin( const std::string& token, uint64 serverid );

    private:
        // masterid
        uint64 _master_server_id{ 0 };

        // 是否在服务中
        bool _in_service{ false };

        // 认证的token列表
        KFMap< std::string, const std::string&, KFClusterToken > _kf_token_list;

        // hash一致性列表
        KFConHash _kf_hash;

        // 对象映射列表
        std::map< uint64, uint64 > _kf_dynamic_shard;

        // 对象数量
        std::map< uint64, uint64 > _kf_object_count;

        // 分配列表
        std::map< uint64, uint64 > _kf_static_shard;
    };
}
#endif