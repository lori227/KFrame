#ifndef __KF_CLUSTER_CLIENT_MODULE_H__
#define __KF_CLUSTER_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    集群客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-17
************************************************************************/

#include "KFClusterClientInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    class KFClusterClientModule : public KFClusterClientInterface
    {
    public:
        KFClusterClientModule() = default;
        ~KFClusterClientModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 是否在服务中
        virtual bool IsInService();

        // 发送消息
        virtual bool SendToProxy( uint32 msgid, google::protobuf::Message* message );
        virtual bool SendToProxy( uint64 shardid, uint32 msgid, google::protobuf::Message* message );

        // 注册回调函数
        virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function );
        virtual void RemoveConnectionFunction( const std::string& name );

    protected:
        // 认证回馈
        __KF_MESSAGE_FUNCTION__( HandleClusterAuthToClientAck );

        // 验证token回馈
        __KF_MESSAGE_FUNCTION__( HandleClusterVerifyToClientAck );

    protected:
        // 断开服务器连接
        __KF_NET_EVENT_FUNCTION__( OnClientLostServer );

        // 连接事件
        __KF_NET_EVENT_FUNCTION__( OnClientConnectionServer );

        // 发送认证消息
        __KF_TIMER_FUNCTION__( OnTimerSendClusterAuthMessage );

        // 发送Token消息
        __KF_TIMER_FUNCTION__( OnTimerSendClusterTokenMessage );
    private:



        // 调用注册函数
        void CallClusterConnectionFunction( uint64 serverid );

        // 连接master
        void ReconnectClusterMaster();

    private:
        // cluster name
        std::string _cluster_name;

        // cluster master id
        uint64 _cluster_master_id{ 0 };
        std::string _str_master_id;

        // token
        std::string _auth_token;

        // proxy id
        uint64 _cluster_proxy_id{ 0 };

        // 是否可以服务
        bool _cluster_in_services{ false };

        // 集群认证成功的回调函数
        KFBind< std::string, const std::string&, KFClusterConnectionFunction >_kf_connection_function;
    };
}



#endif