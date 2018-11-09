#ifndef __KF_CLUSTER_CLIENT_MODULE_H__
#define __KF_CLUSTER_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    集群客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-17
************************************************************************/

#include "KFrame.h"
#include "KFClusterClient.h"
#include "KFBus/KFBusInterface.h"
#include "KFClusterClientInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
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

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 调用注册函数
        void CallClusterConnectionFunction( const std::string& name, uint32 serverid );

        // 发送消息
        virtual bool SendToShard( const std::string& name, uint32 msgid, google::protobuf::Message* message );
        virtual bool SendToShard( const std::string& name, uint32 shardid, uint32 msgid, google::protobuf::Message* message );

        // 发送到静态对象所在的分片服务器
        virtual bool SendToStaticObject( const std::string& name, uint32 objectid, uint32 msgid, google::protobuf::Message* message );

        // 发送到动态对象所在的分片服务器
        virtual bool SendToDynamicObject( const std::string& name, uint64 objectid, uint32 msgid, google::protobuf::Message* message );

    protected:
        // 认证回馈
        __KF_MESSAGE_FUNCTION__( HandleClusterAuthAck );

        // 验证token回馈
        __KF_MESSAGE_FUNCTION__( HandleClusterVerifyAck );

    protected:
        // 断开服务器连接
        __KF_CLIENT_LOST_FUNCTION__( OnClientLostServer );

        // 连接事件
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectionServer );
    private:

        // 注册回调函数
        virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function );
        virtual void RemoveConnectionFunction( const std::string& name );

        // 发送消息
        bool SendNetMessage( const std::string& name, uint32 msgid, google::protobuf::Message* message );
        bool SendNetMessage( const std::string& name, uint32 shardid, uint32 msgid, google::protobuf::Message* message );
    private:
        // 集群客户端列表
        KFMap< std::string, const std::string&, KFClusterClient > _kf_cluster_client;

        // 集群认证成功的回调函数
        KFBind< std::string, const std::string&, KFClusterConnectionFunction >_kf_connection_function;
    };
}



#endif