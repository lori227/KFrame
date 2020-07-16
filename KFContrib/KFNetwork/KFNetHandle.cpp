#include "KFNetHandle.h"
#include "KFNetEvent.h"
#include "KFNetServerEngine.h"
#include "KFNetClientServices.h"
#include "uv.h"

namespace KFrame
{
    KFNetHandle::KFNetHandle()
    {
        _uv_tcp = nullptr;
        _is_trustee = true;
    }

    KFNetHandle::~KFNetHandle()
    {
        delete _uv_tcp;
    }

    void KFNetHandle::InitHandle( uint64 id, void* uvtcp, KFNetServerServices* netservices )
    {
        _is_trustee = true;
        _net_services = netservices;
        InitConnector( id, netservices, nullptr );

        _uv_tcp = reinterpret_cast< uv_tcp_t* >( uvtcp );
        _uv_tcp->data = this;
        uv_tcp_nodelay( _uv_tcp, 1 );
        uv_tcp_keepalive( _uv_tcp, 1, 20 );

        {
            struct sockaddr peername;
            int32 length = static_cast< int32 >( sizeof( peername ) );
            uv_tcp_getpeername( _uv_tcp, &peername, &length );

            sockaddr_in sin;
            memcpy( &sin, &peername, length );
            _remote_ip = inet_ntoa( sin.sin_addr );
        }

        OnConnect( reinterpret_cast< uv_stream_t* >( _uv_tcp ) );
    }

    void KFNetHandle::InitCompressEncrypt()
    {
        _net_compress_encrypt = &_net_services->_compress_encrypt;
    }

    void KFNetHandle::OnDisconnect( int32 code, const char* function, uint32 line )
    {
        KFNetSession::OnDisconnect( code, function, line );
        _net_services->_net_event->AddEvent( KFNetDefine::DisconnectEvent, _session_id );
    }

    void KFNetHandle::CloseHandle()
    {
        if ( !_is_shutdown )
        {
            _net_services->SendEventToServices( this, KFNetDefine::CloseEvent );
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
        auto nethandle = reinterpret_cast< KFNetHandle* >( handle->data );

        uint64 istrustee = nethandle->_is_trustee ? 1 : 0;
        nethandle->_net_services->_net_event->AddEvent( KFNetDefine::ShutEvent, nethandle->_session_id, reinterpret_cast< void*>( istrustee ) );
    }
}
