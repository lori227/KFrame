#ifndef __KF_ROUTE_CLIENT_MODULE_H__
#define __KF_ROUTE_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    转发的客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-12-21
************************************************************************/

#include "KFrame.h"
#include "KFRouteClientInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    class KFRouteClientModule : public KFRouteClientInterface
    {
    public:
        KFRouteClientModule() = default;
        ~KFRouteClientModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 转给给指定类型的所有
        virtual bool SendToAll( const std::string& name, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToAll( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message );

        // 转发给随机服务器
        virtual bool SendToRand( const std::string& name, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToRand( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message );

        // 转发到负载最小的服务器
        virtual bool SendToBalance( const std::string& name, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToBalance( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message );

        // 发送到对象所在的服务器
        virtual bool SendToObject( const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToObject( uint64 sendid, const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message );

        // 转发给指定服务器
        virtual bool SendToServer( uint64 serverid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToServer( uint64 sendid, uint64 serverid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送到指定玩家
        virtual bool SendToPlayer( uint64 serverid, uint64 recvid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendToPlayer( uint64 sendid, uint64 serverid, uint64 recvid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送到路由
        virtual bool SendToRoute( const Route& route, uint32 msgid, ::google::protobuf::Message* message );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 同步所有对象到Route Shard
        virtual void SyncObject( RouteObjectList& objectlist );

        // 添加对象到Route Shard
        virtual void AddObject( uint64 objectid );

        // 删除对象到Route Shard
        virtual void RemoveObject( uint64 objectid );

    protected:
        // 添加连接回调
        virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function );
        virtual void RemoveConnectionFunction( const std::string& name );

        // 注册
        virtual void SetTranspondFunction( KFTranspondFunction& function );

        // 注册服务
        void RegisterService();

    protected:
        // 连接成功
        void OnRouteConnectCluster( uint64 serverid );

        // 发送同步对象消息
        void RouteSyncObjectToProxy( uint64 shardid );

        // 转发消息
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToClientAck );

        // 通知有新的shard
        __KF_MESSAGE_FUNCTION__( HandleRouteDiscoverToClientReq );

    private:
        // 转发函数
        KFTranspondFunction _kf_transpond_function = nullptr;

        // 服务器名字
        std::string _service_name;

        // 对象列表, 一个进程只挂载一种服务
        RouteObjectList _service_object_list;

        // 集群认证成功的回调函数
        KFBind< std::string, const std::string&, KFClusterConnectionFunction >_kf_connection_function;
    };
}



#endif