#include "KFNetHandle.hpp"
#include "KFNetEvent.hpp"
#include "KFNetServerService.hpp"

namespace KFrame
{
    KFNetHandle::KFNetHandle()
    {
        _uv_tcp = nullptr;
        _is_trustee = true;
    }

    KFNetHandle::~KFNetHandle()
    {
        __DELETE_OBJECT__( _uv_tcp );
    }

    void KFNetHandle::InitHandle( uint64 id, void* uv_tcp, KFNetServerService* net_service )
    {
        _is_trustee = true;
        _net_service = net_service;
        InitConnector( id, net_service, nullptr );

        _uv_tcp = reinterpret_cast< uv_tcp_t* >( uv_tcp );
        _uv_tcp->data = this;
        uv_tcp_nodelay( _uv_tcp, 1 );
        uv_tcp_keepalive( _uv_tcp, 1, 20 );

        {
            struct sockaddr peer_name;
            int32 length = static_cast< int32 >( sizeof( peer_name ) );
            uv_tcp_getpeername( _uv_tcp, &peer_name, &length );

            sockaddr_in sin;
            memcpy( &sin, &peer_name, length );
            _remote_ip = inet_ntoa( sin.sin_addr );
        }

        OnConnect( reinterpret_cast< uv_stream_t* >( _uv_tcp ) );
    }

    void KFNetHandle::InitCompressEncrypt()
    {
        _net_compress_encrypt = &_net_service->_compress_encrypt;
    }

    void KFNetHandle::OnDisconnect( int32 code, const char* function, uint32 line )
    {
        KFNetSession::OnDisconnect( code, function, line );
        _net_service->_net_event->AddEvent( KFNetDefine::DisconnectEvent, _session_id );
    }

    void KFNetHandle::CloseHandle()
    {
        if ( !_is_shutdown )
        {
            SendServiceEvent( KFNetDefine::CloseEvent );
        }
    }

    void KFNetHandle::CloseSession()
    {
        _is_shutdown = true;
        KFNetSession::CloseSession();

        _uv_tcp->data = this;
        uv_close( reinterpret_cast< uv_handle_t* >( _uv_tcp ), OnShutCallBack );
    }

    void KFNetHandle::OnShutCallBack( uv_handle_t* handle )
    {
        auto net_handle = reinterpret_cast< KFNetHandle* >( handle->data );

        net_handle->_net_service->_net_event->AddEvent( KFNetDefine::ShutEvent, net_handle->_session_id );
    }
}
