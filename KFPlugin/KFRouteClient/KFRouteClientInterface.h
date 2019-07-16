#ifndef __KF_ROUTE_CLIENT_INTERFACE_H__
#define __KF_ROUTE_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::unordered_set< uint64 > RouteObjectList;
    class KFRouteClientInterface : public KFModule
    {
    public:
        // 转给到指定类型的所有
        virtual bool SendToAll( const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;
        virtual bool SendToAll( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;

        // 转发到随机服务器
        virtual bool SendToRand( const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;
        virtual bool SendToRand( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;

        // 转发到负载最小的服务器
        virtual bool SendToBalance( const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;
        virtual bool SendToBalance( uint64 sendid, const std::string& name, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;

        // 发送到对象所在的服务器
        virtual bool SendToObject( const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;
        virtual bool SendToObject( uint64 sendid, const std::string& name, uint64 objectid, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;

        // 转发给指定服务器
        virtual bool SendToServer( uint64 serverid, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;
        virtual bool SendToServer( uint64 sendid, uint64 serverid, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;

        // 发送到指定玩家
        virtual bool SendToPlayer( uint64 serverid, uint64 recvid, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;
        virtual bool SendToPlayer( uint64 sendid, uint64 serverid, uint64 recvid, uint32 msgid, ::google::protobuf::Message* message, bool resend ) = 0;

        // 发送到路由
        virtual bool SendToRoute( const Route& route, uint32 msgid, ::google::protobuf::Message* message ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步所有对象到Route Shard
        virtual void SyncObject( RouteObjectList& objectlist ) = 0;

        // 添加对象到Route Shard
        virtual void AddObject( uint64 objectid ) = 0;

        // 删除对象到Route Shard
        virtual void RemoveObject( uint64 objectid ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册连接回调
        template< class T >
        void RegisterConnectionFunction( T* object, void ( T::*handle )( uint64 ) )
        {
            KFClusterConnectionFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddConnectionFunction( typeid( T ).name(), function );
        }

        // 取消注册
        template< class T >
        void UnRegisterConnectionFunction( T* object )
        {
            RemoveConnectionFunction( typeid( T ).name() );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册转发消息
        template< class T >
        void RegisterTranspondFunction( T* object, bool ( T::*handle )( const Route& route, uint32 msgid, const char* data, uint32 length ) )
        {
            KFTranspondFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            SetTranspondFunction( function );
        }

        // 卸载
        template< class T >
        void UnRegisterTranspondFunction( T* object )
        {
            KFTranspondFunction function = nullptr;
            SetTranspondFunction( function );
        }

    protected:
        virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function ) = 0;
        virtual void RemoveConnectionFunction( const std::string& name ) = 0;

        virtual void SetTranspondFunction( KFTranspondFunction& function ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_route, KFRouteClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __REGISTER_ROUTE_MESSAGE_FUNCTION__( function )\
    _kf_route->RegisterTranspondFunction( this, function )

#define __UN_ROUTE_MESSAGE_FUNCTION__()\
    _kf_route->UnRegisterTranspondFunction( this )
}



#endif