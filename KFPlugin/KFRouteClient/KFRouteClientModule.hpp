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
#include "KFTimer/KFTimerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"
#include "KFRouteConfig.hpp"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    class KFRouteMessage
    {
    public:
        KFRouteMessage( uint32 length )
        {
            if ( length != 0u )
            {
                _length = length;
                _data = __KF_MALLOC__( char, length );
            }
        }

        ~KFRouteMessage()
        {
            if ( _data != nullptr )
            {
                __KF_FREE__( char, _data );
            }
        }

        void SetRouteTime()
        {
            _route_time = KFGlobal::Instance()->_game_time + 6000u;
        }

        bool IsRouteTime()
        {
            return KFGlobal::Instance()->_game_time >= _route_time;
        }

    public:
        uint32 _msg_id = 0u;
        uint32 _length = 0u;
        char* _data = nullptr;

        // 超时时间
        uint64 _route_time = 0u;
    };
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    class KFRouteClientModule : public KFRouteClientInterface
    {
    public:
        KFRouteClientModule() = default;
        ~KFRouteClientModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void PrepareRun();

        // Run
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步所有对象到Route Shard
        virtual void SyncObject( RouteObjectList& objectlist );

        // 添加对象到Route Shard
        virtual void AddObject( uint64 objectid );

        // 删除对象到Route Shard
        virtual void RemoveObject( uint64 objectid );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送到路由( 只发送一次 )
        virtual bool SendToRoute( const Route& route, uint32 msg_id, ::google::protobuf::Message* message );

        // 发送到路由( 失败重复发送 )
        virtual bool RepeatToRoute( const Route& route, uint32 msg_id, ::google::protobuf::Message* message );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 转给到指定类型的所有( 只发送一次 )
        virtual bool SendToAll( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool SendToAll( uint64 sendid, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );

        // 转给到指定类型的所有( 失败重复发送 )
        virtual bool RepeatToAll( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool RepeatToAll( uint64 sendid, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 转发到随机服务器( 只发送一次 )
        virtual bool SendToRand( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool SendToRand( uint64 sendid, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );

        // 转发到随机服务器( 失败重复发送 )
        virtual bool RepeatToRand( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool RepeatToRand( uint64 sendid, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 转发到负载最小的服务器( 只发送一次 )
        virtual bool SendToBalance( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool SendToBalance( uint64 sendid, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );

        // 转发到负载最小的服务器( 失败重复发送 )
        virtual bool RepeatToBalance( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool RepeatToBalance( uint64 sendid, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送到对象所在的服务器( 只发送一次 )
        virtual bool SendToObject( const std::string& name, uint64 objectid, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool SendToObject( uint64 sendid, const std::string& name, uint64 objectid, uint32 msg_id, ::google::protobuf::Message* message );

        // 发送到对象所在的服务器( 失败重复发送 )
        virtual bool RepeatToObject( const std::string& name, uint64 objectid, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool RepeatToObject( uint64 sendid, const std::string& name, uint64 objectid, uint32 msg_id, ::google::protobuf::Message* message );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 转发给指定服务器( 只发送一次 )
        virtual bool SendToServer( uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message ) ;
        virtual bool SendToServer( uint64 sendid, uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message );

        // 转发给指定服务器( 失败重复发送 )
        virtual bool RepeatToServer( uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool RepeatToServer( uint64 sendid, uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送到指定玩家( 只发送一次 )
        virtual bool SendToEntity( uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool SendToEntity( uint64 sendid, uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message );

        // 发送到指定玩家( 失败重复发送 )
        virtual bool RepeatToEntity( uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message );
        virtual bool RepeatToEntity( uint64 sendid, uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加连接回调
        virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function );
        virtual void RemoveConnectionFunction( const std::string& name );

        // 注册
        virtual void SetTranspondFunction( KFForwardFunction& function );

        // 转发规则
        const std::string& FindRouteProcess( const std::string& module );

        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 发送转发消息
        bool SendRouteMessage( uint32 msg_id, ::google::protobuf::Message* message );

        // 发送转发消息
        void RunSendRouteMessage();

        // 转发消息成功
        void SendRouteMessageOk( uint64 server_id, uint64 serial );

    protected:
        // 连接成功
        void OnRouteConnectCluster( uint64 server_id );

        // 发送同步对象消息
        void RouteSyncObjectToProxy( uint64 shardid );

        // 转发消息成功
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageOk, KFMsg::S2SRouteMessageOk );

        // 转发消息
        __KF_MESSAGE_FUNCTION__( HandleRouteMessageToClientAck, KFMsg::S2SRouteMessageToClientAck );

        // 通知有新的shard
        __KF_MESSAGE_FUNCTION__( HandleRouteDiscoverToClientReq, KFMsg::S2SRouteDiscoverToClientReq );

        // 连接route master
        __KF_TIMER_FUNCTION__( OnTimerConnectionRouteMaster );

        // 连接route master 失败
        __KF_NET_EVENT_FUNCTION__( OnClientConnectRouteMasterFailed );
    private:
        // 转发函数
        KFForwardFunction _kf_forward_function = nullptr;

        // 服务器名字
        std::string _service_name;

        // 对象列表, 一个进程只挂载一种服务
        RouteObjectList _service_object_list;

        // 集群认证成功的回调函数
        KFMapFunction< std::string, KFClusterConnectionFunction >_kf_connection_function;

        // 转发消息
        uint64 _route_serial = 0u;
        KFMap<uint64, KFRouteMessage > _route_message_list;

        // 连接route master失败次数
        uint32 _connect_route_master_failed_count = 0u;
    };
}



#endif