#ifndef __KF_TCP_SERVER_INTERFACE_H__
#define __KF_TCP_SERVER_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFTcpServerInterface : public KFModule
    {
    public:
        //////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册到连接器
        virtual bool RegisteNetHandle( uint64 sessionid, uint64 handleid, uint64 objectid ) = 0;

        // 关闭连接器
        virtual bool CloseNetHandle( uint64 handleid, uint32 delaytime, const char* function, uint32 line ) = 0;

        // 连接数量
        virtual uint32 GetHandleCount() = 0;

        // 连接列表
        virtual void GetHandleList( std::list< uint64 >& handlelist ) = 0;

        // 获得连接ip
        virtual const std::string& GetHandleIp( uint64 handleid ) = 0;

        // 设置id
        virtual bool BindObjectId( uint64 handleid, uint64 objectid ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////

        // 给全部客户端发送消息
        virtual void SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 excludeid = 0 ) = 0;
        virtual void SendNetMessage( uint32 msgid, google::protobuf::Message* message, uint32 excludeid = 0 ) = 0;

        // 给指定客户端发送消息
        virtual bool SendNetMessage( uint64 handleid, uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendNetMessage( uint64 handleid, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 给指定对象发送消息
        virtual bool SendNetMessage( uint64 handleid, uint64 objectid, uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendNetMessage( uint64 handleid, uint64 objectid, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 给指定对象发送消息
        virtual bool SendNetMessage( const KFId& kfid, uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendNetMessage( const KFId& kfid, uint32 msgid, google::protobuf::Message* message ) = 0;

        // 给指定类型发送消息
        virtual void SendMessageToName( const std::string& name, uint32 msgid, google::protobuf::Message* message ) = 0;

        virtual void SendMessageToType( const std::string& type, uint32 msgid, google::protobuf::Message* message ) = 0;
        virtual void SendMessageToType( const std::string& type, uint32 msgid, const char* data, uint32 length ) = 0;

        // 给某一类型客户端发送消息
        //////////////////////////////////////////////////////////////////////////////////////////
        // 注册连接成功函数
        template< class T >
        void RegisterDiscoverFunction( T* object, void ( T::*handle )( uint64 handleid, const std::string& servername, const std::string& servertype, const std::string& ip, uint32 port ) )
        {
            KFServerDiscoverFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5 );
            AddDiscoverFunction( typeid( T ).name(), function );
        }

        // 卸载
        template< class T >
        void UnRegisterDiscoverFunction( T* object )
        {
            RemoveDiscoverFunction( typeid( T ).name() );
        }
        //////////////////////////////////////////////////////////////////////////////////////////

        // 注册断线回调函数
        template< class T >
        void RegisterLostFunction( T* object, void ( T::*handle )( uint64 id, const std::string& servername, const std::string& servertype  ) )
        {
            KFServerLostFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            AddLostFunction( typeid( T ).name(), function );
        }

        // 卸载
        template< class T >
        void UnRegisterLostFunction( T* object )
        {
            RemoveLostFunction( typeid( T ).name() );
        }
        /////////////////////////////////////////////////////////////////////////////
        // 注册转发
        template< class T >
        void RegisterTransmitFunction( T* object, bool ( T::*handle )( const KFId& kfid, uint32 msgid, const char* data, uint32 length ) )
        {
            KFTransmitFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            AddTransmitFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterTransmitFunction( T* object )
        {
            RemoveTransmitFunction( typeid( T ).name() );
        }

    private:
        virtual void AddDiscoverFunction( const char* module, KFServerDiscoverFunction& function ) = 0;
        virtual void RemoveDiscoverFunction( const char* module ) = 0;

        virtual void AddLostFunction( const char* module, KFServerLostFunction& function ) = 0;
        virtual void RemoveLostFunction( const char* module ) = 0;

        virtual void AddTransmitFunction( const char* module, KFTransmitFunction& function ) = 0;
        virtual void RemoveTransmitFunction( const char* module ) = 0;

    };

    //////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_tcp_server, KFTcpServerInterface );
    //////////////////////////////////////////////////////////////////////////////////////////

#define __KF_SERVER_LOST_FUNCTION__( function ) \
    void function( uint64 handleid, const std::string& handlename, const std::string& handletype )

#define __KF_SERVER_DISCOVER_FUNCTION__( function ) \
    void function( uint64 handleid, const std::string& handlename , const std::string& handletype, const std::string& ip, uint32 port )

#define __REGISTER_SERVER_DISCOVER_FUNCTION__( function ) \
    _kf_tcp_server->RegisterDiscoverFunction( this, function )

#define __UNREGISTER_SERVER_DISCOVER_FUNCTION__() \
    _kf_tcp_server->UnRegisterDiscoverFunction( this )

#define __REGISTER_SERVER_LOST_FUNCTION__( function ) \
    _kf_tcp_server->RegisterLostFunction( this, function )

#define __UNREGISTER_SERVER_LOST_FUNCTION__() \
    _kf_tcp_server->UnRegisterLostFunction( this )

#define __REGISTER_SERVER_TRANSMIT_FUNCTION__( function ) \
    _kf_tcp_server->RegisterTransmitFunction( this, function )

#define __UNREGISTER_SERVER_TRANSMIT_FUNCTION__() \
    _kf_tcp_server->UnRegisterTransmitFunction( this )

}



#endif