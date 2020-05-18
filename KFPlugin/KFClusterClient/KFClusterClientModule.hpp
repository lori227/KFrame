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
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFZConfig/KFRouteConfig.hpp"

namespace KFrame
{
    class SendKeeper
    {
    public:
        SendKeeper( uint32 length )
        {
            if ( length != 0u )
            {
                _length = length;
                _data = __KF_MALLOC__( char, length );
            }
        }

        ~SendKeeper()
        {
            if ( _data != nullptr )
            {
                __KF_FREE__( char, _data );
            }
        }

    public:
        uint64 _shard_id = 0u;
        uint32 _msg_id = 0u;
        uint32 _length = 0u;
        char* _data = nullptr;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////

    class KFClusterClientModule : public KFClusterClientInterface
    {
    public:
        KFClusterClientModule() = default;
        ~KFClusterClientModule();

        // 初始化
        virtual void BeforeRun();
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 是否在服务中
        virtual bool IsInService();

        // 发送消息( 只发送一次, 不管成功还是失败 )
        virtual bool SendToProxy( uint32 msgid, const char* data, uint32 length );
        virtual bool SendToProxy( uint32 msgid, google::protobuf::Message* message );

        virtual bool SendToProxy( uint64 shardid, uint32 msgid, const char* data, uint32 length );
        virtual bool SendToProxy( uint64 shardid, uint32 msgid, google::protobuf::Message* message );

        // 发送消息( 失败重复发送, 一直到发送成功 )
        virtual bool RepeatToProxy( uint32 msgid, const char* data, uint32 length );
        virtual bool RepeatToProxy( uint32 msgid, google::protobuf::Message* message );

        virtual bool RepeatToProxy( uint64 shardid, uint32 msgid, const char* data, uint32 length );
        virtual bool RepeatToProxy( uint64 shardid, uint32 msgid, google::protobuf::Message* message );

    protected:
        // 认证回馈
        __KF_MESSAGE_FUNCTION__( HandleClusterAuthToClientAck );

        // 验证token回馈
        __KF_MESSAGE_FUNCTION__( HandleClusterVerifyToClientAck );

    protected:
        // 连接事件
        __KF_NET_EVENT_FUNCTION__( OnClientConnectionClusterServer );

        // 断开服务器连接
        __KF_NET_EVENT_FUNCTION__( OnClientLostClusterProxy );

        // 发送认证消息
        __KF_TIMER_FUNCTION__( OnTimerSendClusterAuthMessage );

        // 发送Token消息
        __KF_TIMER_FUNCTION__( OnTimerSendClusterTokenMessage );
    protected:
        // 注册回调函数
        virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function );
        virtual void RemoveConnectionFunction( const std::string& name );

        // 调用注册函数
        void CallClusterConnectionFunction( uint64 serverid );

        // 连接master
        void ReconnectClusterMaster();

        // 添加重发消息
        void AddSendKeeper( uint64 shardid, uint32 msgid, const char* data, uint32 length );

        // 发送重发消息
        void RunSendKeeper();

    private:
        // cluster name
        std::string _cluster_name;
        std::string _cluster_type;

        // cluster master id
        uint64 _cluster_master_id = 0u;
        std::string _cluster_master_ip;
        uint32 _cluster_master_port = 0u;

        // 发送cluster master认证次数
        uint32 _cluster_auth_count = 0u;

        // token
        std::string _auth_token;

        // proxy id
        uint64 _cluster_proxy_id = 0u;

        // 是否可以服务
        bool _cluster_in_services = false;

        // 集群认证成功的回调函数
        KFBind< std::string, const std::string&, KFClusterConnectionFunction >_kf_connection_function;

        // 需要重新发送的消息
        std::list < SendKeeper* > _send_keeper;
    };
}



#endif