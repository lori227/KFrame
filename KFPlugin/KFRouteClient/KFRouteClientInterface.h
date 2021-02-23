#ifndef __KF_ROUTE_CLIENT_INTERFACE_H__
#define __KF_ROUTE_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRouteClientInterface : public KFModule
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步所有对象到Route Shard
        virtual void SyncObject( UInt64Set& object_list ) = 0;

        // 添加对象到Route Shard
        virtual void AddObject( uint64 object_id ) = 0;

        // 删除对象到Route Shard
        virtual void RemoveObject( uint64 object_id ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送到路由( 只发送一次 )
        virtual bool SendToRoute( const Route& route, uint32 msg_id, ::google::protobuf::Message* message ) = 0;

        // 发送到路由( 失败重复发送 )
        virtual bool RepeatToRoute( const Route& route, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 转给到指定类型的所有( 只发送一次 )
        virtual bool SendToAll( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToAll( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;

        // 转给到指定类型的所有( 失败重复发送 )
        virtual bool RepeatToAll( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool RepeatToAll( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 转发到随机服务器( 只发送一次 )
        virtual bool SendToRand( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToRand( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;

        // 转发到随机服务器( 失败重复发送 )
        virtual bool RepeatToRand( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool RepeatToRand( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 转发到负载最小的服务器( 只发送一次 )
        virtual bool SendToBalance( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToBalance( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;

        // 转发到负载最小的服务器( 失败重复发送 )
        virtual bool RepeatToBalance( const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool RepeatToBalance( uint64 send_id, const std::string& name, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送到对象所在的服务器( 只发送一次 )
        virtual bool SendToObject( const std::string& name, uint64 object_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToObject( uint64 send_id, const std::string& name, uint64 object_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;

        // 发送到对象所在的服务器( 失败重复发送 )
        virtual bool RepeatToObject( const std::string& name, uint64 object_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool RepeatToObject( uint64 send_id, const std::string& name, uint64 object_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 转发给指定服务器( 只发送一次 )
        virtual bool SendToServer( uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToServer( uint64 send_id, uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;

        // 转发给指定服务器( 失败重复发送 )
        virtual bool RepeatToServer( uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool RepeatToServer( uint64 send_id, uint64 server_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送到指定实体对象, 包括玩家, 工会, 队伍等( 只发送一次 )
        virtual bool SendToEntity( uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToEntity( uint64 send_id, uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;

        // 发送到指定实体对象, 包括玩家, 工会, 队伍等( 失败重复发送 )
        virtual bool RepeatToEntity( uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        virtual bool RepeatToEntity( uint64 send_id, uint64 server_id, uint64 recv_id, uint32 msg_id, ::google::protobuf::Message* message ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册连接回调
        template<class T>
        void RegisterConnectionFunction( T* module, void ( T::*handle )( uint64 ) )
        {
            KFClusterConnectionFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddConnectionFunction( module, function );
        }

        // 取消注册
        template<class T>
        void UnRegisterConnectionFunction( T* module )
        {
            RemoveConnectionFunction( module );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册转发消息
        template<class T>
        void RegisterForwardFunction( T* module, bool ( T::*handle )( const Route& route, uint32, const char*, uint32 ) )
        {
            KFForwardFunction function = std::bind( handle, module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            SetForwardFunction( module, function );
        }

        // 卸载
        template<class T>
        void UnRegisterForwardFunction( T* module )
        {
            KFForwardFunction function = nullptr;
            SetForwardFunction( module, function );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual void AddConnectionFunction( KFModule* module, KFClusterConnectionFunction& function ) = 0;
        virtual void RemoveConnectionFunction( KFModule* module ) = 0;
        virtual void SetForwardFunction( KFModule* module, KFForwardFunction& function ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_route, KFRouteClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __REGISTER_ROUTE_MESSAGE_FUNCTION__( function )\
    _kf_route->RegisterForwardFunction( this, function )

#define __UN_ROUTE_MESSAGE_FUNCTION__()\
    _kf_route->UnRegisterForwardFunction( this )
}



#endif