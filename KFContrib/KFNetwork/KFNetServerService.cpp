#include "KFrame.h"
#include "KFNetHandle.hpp"
#include "KFNetServerService.hpp"

namespace KFrame
{
    KFNetServerService::KFNetServerService()
    {
        _uv_server = __NEW_OBJECT__( uv_tcp_t );
    }

    KFNetServerService::~KFNetServerService()
    {
        __DELETE_OBJECT__( _uv_server );
    }

    void KFNetServerService::InitService( uint32 event_count, uint32 queue_size, uint32 message_type )
    {
        KFNetService::InitService( event_count, queue_size, message_type );

        _uv_server->data = this;
        uv_tcp_init( _uv_loop, _uv_server );
    }

    int32 KFNetServerService::StartService( const KFNetData* net_data )
    {
        uv_tcp_nodelay( _uv_server, 1 );
        uv_tcp_keepalive( _uv_server, 1, 20 );

        sockaddr_in addr;
        uv_ip4_addr( net_data->_ip.c_str(), net_data->_port, &addr );
        uv_tcp_bind( _uv_server, reinterpret_cast< const sockaddr* >( &addr ), 0 );

        _uv_server->data = this;
        auto result = uv_listen( reinterpret_cast< uv_stream_t* >( _uv_server ), SOMAXCONN, OnConnectionCallBack );
        if ( result != 0 )
        {
            return result;
        }

        return KFNetService::StartService( net_data );
    }

    void KFNetServerService::CloseService()
    {
        uv_close( reinterpret_cast< uv_handle_t* >( _uv_server ), nullptr );
        KFNetService::CloseService();
    }

    uint64 KFNetServerService::MakeTrusteeID()
    {
        return ++_trustee_id;
    }

    // 连接
    void KFNetServerService::OnConnectionCallBack( uv_stream_t* stream, int status )
    {
        auto net_service = reinterpret_cast< KFNetServerService* >( stream->data );
        if ( net_service->_is_shutdown )
        {
            return;
        }

        // 创建一个id
        auto id = net_service->MakeTrusteeID();

        auto uv_tcp = __NEW_OBJECT__( uv_tcp_t );
        uv_tcp_init( net_service->_uv_loop, uv_tcp );

        uv_tcp->data = stream;
        uv_accept( stream, reinterpret_cast< uv_stream_t* >( uv_tcp ) );

        // 创建句柄
        auto net_handle = __MAKE_SHARED__( KFNetHandle );
        net_handle->InitHandle( id, uv_tcp, net_service );

        //  添加连接成功事件
        net_service->_net_event->AddEvent( KFNetDefine::ConnectEvent, id, net_handle );
    }
}