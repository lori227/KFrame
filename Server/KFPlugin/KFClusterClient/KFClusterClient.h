#ifndef __KF_CLUSTER_CLIENT_H__
#define __KF_CLUSTER_CLIENT_H__

#include "KFrame.h"
#include "KFTimer/KFTimerInterface.h"

namespace KFrame
{
    class KFClusterClientModule;
    class KFClusterClient
    {
    public:
        KFClusterClient() = default;
        ~KFClusterClient() = default;

        // 开启集群客户端
        void StartClusterMasterClient();

        // 连接事件
        void OnConnectionClusterServer( const std::string& servername, const std::string& servertype, uint64 serverid );

        // 断线事件
        void OnLostClusterServer( const std::string& servertype, uint64 serverid );

        // 认证成功
        void ProcessClusterAuth( const std::string& name, const std::string& type, uint64 id, const std::string& ip, uint32 port, const std::string& token );

        // 验证token成功
        void ProcessClusterVerify( uint64 serverid );

        // 发送消息
        bool SendNetMessage( uint32 msgid, google::protobuf::Message* message );
        bool SendNetMessage( uint64 objectid, uint32 msgid, google::protobuf::Message* message );

        bool SendNetMessage( uint32 msgid, const char* data, uint32 length );
        bool SendNetMessage( uint64 objectid, uint32 msgid, const char* data, uint32 length );

    protected:
        // 发送认证消息
        __KF_TIMER_FUNCTION__( OnTimerSendClusterAuthMessage );

        // 发送Token消息
        __KF_TIMER_FUNCTION__( OnTimerSendClusterTokenMessage );

    public:
        // 模块
        KFClusterClientModule* _cluster_client_module;

        // 集群设置
        std::string _cluster_master_name;
        uint64 _cluster_master_id{ 0 };
        std::string _str_master_id;

    private:
        // 是否可以服务
        bool _cluster_in_services{ false };

        // token
        std::string _auth_token;

        // proxy id
        uint64 _cluster_proxy_id{ 0 };
    };
}



#endif