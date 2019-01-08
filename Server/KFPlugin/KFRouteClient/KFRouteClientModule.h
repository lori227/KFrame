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

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 转发给所有服务器
        virtual bool SendToAll( uint32 msgid, ::google::protobuf::Message* message );

        // 转给给指定类型的所有
        virtual bool SendToAll( const std::string& name, uint32 msgid, ::google::protobuf::Message* message );

        // 转发给随机服务器
        virtual bool SendToRand( const std::string& name, uint32 msgid, ::google::protobuf::Message* message );

        // 转发到负载最小的服务器
        virtual bool SendToBalance( const std::string& name, uint32 msgid, ::google::protobuf::Message* message );

        // 发送到对象所在的服务器
        virtual bool SendToObject( const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message );

        // 转发给指定服务器
        virtual bool SendToServer( uint64 serverid, uint32 msgid, ::google::protobuf::Message* message );

        // 发送到指定玩家
        virtual bool SendToPlayer( uint64 serverid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 同步所有对象到Route Shard
        virtual void SyncObject( const std::string& name, std::unordered_set< uint64 >& objectlist );

        // 添加对象到Route Shard
        virtual void AddObject( const std::string& name, uint64 objectid );

        // 删除对象到Route Shard
        virtual void RemoveObject( const std::string& name, uint64 objectid );

    protected:
        virtual void AddRouteConnectionFunction( const std::string& name, KFClusterConnectionFunction& function );
        virtual void RemoveRouteConnectionFunction( const std::string& name );

        // 注册
        virtual void SetTransmitFunction( KFTransmitFunction& function );

    protected:
        // 转发消息
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToClientAck );

        // 通知有新的shard
        __KF_MESSAGE_FUNCTION__( HandleRouteDiscoverToClientReq );

    private:
        // 转发函数
        KFTransmitFunction _kf_transmit_function{ nullptr };

        // 对象列表
        std::map< std::string, std::unordered_set< uint64 > > _service_object_list;
    };
}



#endif