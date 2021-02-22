#include "KFNetClient.hpp"
#include "KFNetEvent.hpp"
#include "KFNetClientService.hpp"
#include "uv.h"

namespace KFrame
{
    KFNetClient::KFNetClient()
    {
        _uv_client = __NEW_OBJECT__( uv_tcp_t );
        _uv_connect = __NEW_OBJECT__( uv_connect_t );
        _uv_connect_timer = __NEW_OBJECT__( uv_timer_t );
    }

    KFNetClient::~KFNetClient()
    {
        __DELETE_OBJECT__( _uv_client );
        __DELETE_OBJECT__( _uv_connect );
        __DELETE_OBJECT__( _uv_connect_timer );
    }

    void KFNetClient::StartClient( KFNetClientService* net_service, const KFNetData& net_data )
    {
        // 连接配置
        _net_data = net_data;
        _object_id = _net_data._id;
        _session_id = _net_data._id;
        _net_service = net_service;

        // 初始化客户端
        _uv_client->data = this;
        uv_tcp_nodelay( _uv_client, 1 );
        uv_tcp_keepalive( _uv_client, 1, 20 );

        // 添加到服务
        uv_timer_init( net_service->_uv_loop, _uv_connect_timer );

        _uv_connect->data = this;
        _uv_connect_timer->data = this;
        InitConnector( _net_data._id, net_service, &_net_compress_encrypt );

        // 启动连接
        SendServiceEvent( KFNetDefine::ConnectEvent );
    }

    void KFNetClient::StartSession()
    {
        // 启动连接定时器
        StartConnectTimer( 1 );
    }

    void KFNetClient::StartConnectTimer( uint32 time )
    {
        _uv_connect_timer->data = this;
        uv_timer_start( _uv_connect_timer, OnTimerConnectCallBack, time, 0 );
    }

    void KFNetClient::OnTimerConnectCallBack( uv_timer_t* handle )
    {
        auto client = reinterpret_cast< KFNetClient* >( handle->data );
        if ( !client->_is_shutdown )
        {
            client->TryConnect();
        }
    }

    void KFNetClient::TryConnect()
    {
        _uv_client->data = this;
        _uv_connect->data = this;

        uv_tcp_init( _net_service->_uv_loop, _uv_client );

        sockaddr_in addr;
        uv_ip4_addr( _net_data._ip.c_str(), _net_data._port, &addr );
        uv_tcp_connect( _uv_connect, _uv_client, reinterpret_cast< const sockaddr* >( &addr ), OnConnectCallBack );
    }

    void KFNetClient::OnConnectCallBack( uv_connect_t* handle, int status )
    {
        auto client = reinterpret_cast< KFNetClient* >( handle->data );
        if ( status != 0 )
        {
            client->ConnectFailed( status );
        }
        else
        {
            client->ConnectSuccess( handle->handle );
        }
    }

    void KFNetClient::ConnectFailed( int32 status )
    {
        _uv_client->data = this;
        if ( !uv_is_closing( reinterpret_cast< uv_handle_t* >( _uv_client ) ) )
        {
            uv_close( reinterpret_cast< uv_handle_t* >( _uv_client ), nullptr );
        }

        // 再次启动一个定时器
        StartConnectTimer( KFNetDefine::ConnectTime );
        _net_service->_net_event->AddEvent( KFNetDefine::FailedEvent, _object_id );
    }

    void KFNetClient::ConnectSuccess( uv_stream_t* uv_stream )
    {
        // 连接成功回调
        uv_stream->data = this;
        OnConnect( uv_stream );

        // 添加连接成功时间
        _net_service->_net_event->AddEvent( KFNetDefine::ConnectEvent, _object_id );
    }

    void KFNetClient::OnDisconnect( int32 code, const char* function, uint32 line )
    {
        KFNetSession::OnDisconnect( code, function, line );
        _net_service->_net_event->AddEvent( KFNetDefine::DisconnectEvent, _object_id );

        // 关闭连接
        TryShutDown();
    }

    void KFNetClient::InitCompressEncrypt( uint32 compress_type, uint32 compress_level, uint32 compress_length, const std::string& encrypt_key, bool open_encrypt )
    {
        _net_compress_encrypt._compress_type = compress_type;
        _net_compress_encrypt._compress_level = compress_level;
        if ( compress_length != 0u )
        {
            _net_compress_encrypt._compress_length = compress_length;
        }

        _net_compress_encrypt._encrypt_key = encrypt_key;
        _net_compress_encrypt._open_encrypt = open_encrypt;
    }

    void KFNetClient::CloseClient()
    {
        // 关闭连接
        if ( !_is_shutdown )
        {
            SendServiceEvent( KFNetDefine::CloseEvent );
        }
    }

    void KFNetClient::CloseSession()
    {
        KFNetSession::CloseSession();
        TryShutDown();
    }

    void KFNetClient::TryShutDown()
    {
        if ( _is_shutdown )
        {
            return;
        }

        _is_shutdown = true;
        _uv_connect_timer->data = this;
        uv_timer_stop( _uv_connect_timer );

        _uv_client->data = this;
        if ( uv_is_closing( reinterpret_cast< uv_handle_t* >( _uv_client ) ) )
        {
            _net_service->_net_event->AddEvent( KFNetDefine::ShutEvent, _object_id );
        }
        else
        {
            uv_close( reinterpret_cast< uv_handle_t* >( _uv_client ), OnShutDownCallBack );
        }
    }

    void KFNetClient::OnShutDownCallBack( uv_handle_t* handle )
    {
        auto client = reinterpret_cast< KFNetClient* >( handle->data );
        client->_net_service->_net_event->AddEvent( KFNetDefine::ShutEvent, client->_object_id );
    }
}