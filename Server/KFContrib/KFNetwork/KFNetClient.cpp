#include "KFNetClient.h"
#include "KFNetEvent.h"
#include "KFNetClientEngine.h"
#include "KFNetClientServices.h"
#include "KFLogger/KFLogger.h"

namespace KFrame
{
    KFNetClient::KFNetClient()
    {
        _is_connected = false;
        _is_shutdown = false;
    }

    KFNetClient::~KFNetClient()
    {

    }

    void KFNetClient::StartClient( KFNetClientServices* netservices, const KFNetSetting& netsetting )
    {
        // 连接配置
        _net_setting = netsetting;
        _net_services = netservices;

        // 初始化客户端
        _uv_client.data = this;
        uv_tcp_nodelay( &_uv_client, 1 );
        //uv_tcp_keepalive( &_uv_client, 1, 20 );

        // 添加到服务
        uv_timer_init( netservices->_uv_loop, &_uv_connect_timer );

        _uv_connect.data = this;
        _uv_connect_timer.data = this;
        InitConnector( _net_setting._id, netservices );

        // 启动连接
        netservices->StartSession( this );
    }

    void KFNetClient::StartSession()
    {
        // 启动连接定时器
        StartConnectTimer( 1 );
    }

    void KFNetClient::StartConnectTimer( uint32 time )
    {
        _uv_connect_timer.data = this;
        uv_timer_start( &_uv_connect_timer, OnTimerConnectCallBack, time, 0 );

        KFLogger::LogNet( KFLogger::Debug, "[%s] client[%u] start connect timer[%u]!",
                          __FUNCTION__, _net_setting._id, time );
    }

    void KFNetClient::OnTimerConnectCallBack( uv_timer_t* handle )
    {
        auto netclient = reinterpret_cast< KFNetClient* >( handle->data );
        netclient->TryConnect();
    }

    void KFNetClient::TryConnect()
    {
        _uv_client.data = this;
        _uv_connect.data = this;

        uv_tcp_init( _net_services->_uv_loop, &_uv_client );

        sockaddr_in addr;
        uv_ip4_addr( _net_setting._ip.c_str(), _net_setting._port, &addr );
        uv_tcp_connect( &_uv_connect, &_uv_client, reinterpret_cast< const sockaddr* >( &addr ), OnConnectCallBack );
    }

    void KFNetClient::OnConnectCallBack( uv_connect_t* handle, int status )
    {
        auto netclient = reinterpret_cast< KFNetClient* >( handle->data );
        if ( status != 0 )
        {
            netclient->ConnectFailed( status );
        }
        else
        {
            netclient->ConnectSuccess( handle->handle );
        }
    }

    void KFNetClient::ConnectFailed( int32 status )
    {
        // 再次启动一个定时器
        StartConnectTimer( KFNetDefine::ConncectTime );

        _net_services->_net_event->AddEvent( KFNetDefine::FailedEvent, _net_setting._id );
    }

    void KFNetClient::ConnectSuccess( uv_stream_t* uvstream )
    {
        // 连接成功回调
        uvstream->data = this;
        OnConnect( uvstream );

        // 添加连接成功时间
        _net_services->_net_event->AddEvent( KFNetDefine::ConnectEvent, _net_setting._id );
    }

    void KFNetClient::OnDisconnect( const char* error, int32 code )
    {
        KFNetSession::OnDisconnect( error, code );
        _net_services->_net_event->AddEvent( KFNetDefine::DisconnectEvent, _id );

        if ( !_is_shutdown )
        {
            // 重新连接
            StartConnectTimer( KFNetDefine::ConncectTime );
        }
    }

    void KFNetClient::CloseClient()
    {
        if ( !_is_shutdown )
        {
            // 关闭连接
            _net_services->CloseSession( this );
        }
        else
        {
            KFLogger::LogNet( KFLogger::Error, "[%s:%u] [%s:%u] already shutdown!",
                              __FUNCTION_LINE__, _net_setting._name.c_str(), _net_setting._id );
        }
    }

    void KFNetClient::CloseSession()
    {
        KFNetSession::CloseSession();

        _uv_connect_timer.data = this;
        uv_timer_stop( &_uv_connect_timer );

        _uv_client.data = this;
        uv_close( reinterpret_cast< uv_handle_t* >( &_uv_client ), OnShutDownCallBack );
    }

    void KFNetClient::OnShutDownCallBack( uv_handle_t* handle )
    {
        auto* netclinet = reinterpret_cast< KFNetClient* >( handle->data );
        netclinet->_net_services->_net_event->AddEvent( KFNetDefine::ShutEvent, netclinet->_net_setting._id );
    }

}