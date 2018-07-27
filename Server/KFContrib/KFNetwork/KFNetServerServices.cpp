#include "KFrame.h"
#include "uv.h"
#include "KFNetHandle.h"
#include "KFNetServerServices.h"

namespace KFrame
{
    KFNetServerServices::KFNetServerServices()
    {
        _trustee_id = 0;
    }

    KFNetServerServices::~KFNetServerServices()
    {
    }

    void KFNetServerServices::InitServices( uint32 eventcount, uint32 queuesize )
    {
        KFNetServices::InitServices( eventcount, queuesize );

        _uv_server.data = this;
        uv_tcp_init( _uv_loop, &_uv_server );
    }

    int32 KFNetServerServices::StartServices( const KFNetSetting* kfsetting )
    {
        uv_tcp_nodelay( &_uv_server, 1 );
        uv_tcp_keepalive( &_uv_server, 1, kfsetting->_timeout );

        sockaddr_in addr;
        uv_ip4_addr( kfsetting->_ip.c_str(), kfsetting->_port, &addr );
        uv_tcp_bind( &_uv_server, reinterpret_cast< const sockaddr* >( &addr ), 0 );

        _uv_server.data = this;
        auto result = uv_listen( reinterpret_cast< uv_stream_t* >( &_uv_server ), SOMAXCONN, OnConnectionCallBack );
        if ( result != 0 )
        {
            return result;
        }

        return KFNetServices::StartServices( kfsetting );
    }

    void KFNetServerServices::ShutServices()
    {
        uv_close( reinterpret_cast< uv_handle_t* >( &_uv_server ), nullptr );

        KFNetServices::ShutServices();
    }

    uint32 KFNetServerServices::MakeTrusteeID()
    {
        if ( _trustee_id >= 10000 )
        {
            _trustee_id = 0;
        }

        return ++_trustee_id;
    }

    // 连接
    void KFNetServerServices::OnConnectionCallBack( uv_stream_t* stream, int status )
    {
        auto netservices = reinterpret_cast< KFNetServerServices* >( stream->data );

        // 创建一个id
        auto id = netservices->MakeTrusteeID();

        auto uvtcp = __KF_NEW__( uv_tcp_t );
        uv_tcp_init( netservices->_uv_loop, uvtcp );

        uvtcp->data = stream;
        uv_accept( stream, reinterpret_cast< uv_stream_t* >( uvtcp ) );

        // 创建句柄
        auto kfhandle = __KF_CREATE__( KFNetHandle );
        kfhandle->InitHandle( id, uvtcp, netservices );

        //  添加连接成功事件
        netservices->_net_event->AddEvent( KFNetDefine::ConnectEvent, id, kfhandle );
    }
}