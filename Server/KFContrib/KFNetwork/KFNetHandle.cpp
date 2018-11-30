#include "KFNetHandle.h"
#include "KFNetEvent.h"
#include "KFNetServerEngine.h"
#include "KFNetClientServices.h"

namespace KFrame
{
    KFNetHandle::KFNetHandle()
    {
        _uv_tcp = nullptr;

        _is_trustee = true;
        _listen_port = 0;
        _trustee_timeout = 0;
        _zone_id = 0;
    }

    KFNetHandle::~KFNetHandle()
    {
        __KF_DELETE__( _uv_tcp );
    }

    void KFNetHandle::InitHandle( uint64 id, void* uvtcp, KFNetServerServices* netservices )
    {
        _is_trustee = true;
        _net_services = netservices;
        InitConnector( id, netservices );

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

    bool KFNetHandle::IsServer() const
    {
        return !_app_name.empty();
    }

    void KFNetHandle::SetTrusteeTimeout( uint64 time )
    {
        _trustee_timeout = time;
    }

    bool KFNetHandle::IsTrusteeTimeout( uint64 nowtime )
    {
        if ( _trustee_timeout == 0 )
        {
            return false;
        }

        return nowtime >= _trustee_timeout;
    }

    void KFNetHandle::OnDisconnect( const char* error, int32 code )
    {
        KFNetSession::OnDisconnect( error, code );
        _net_services->_net_event->AddEvent( KFNetDefine::DisconnectEvent, _session_id );
    }

    void KFNetHandle::CloseHandle()
    {
        if ( !_is_shutdown )
        {
            _net_services->CloseSession( this );
        }
    }

    void KFNetHandle::CloseSession()
    {
        _trustee_timeout = 0;
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
