#include "KFrame.h"
#include "KFNetServices.h"
#include "KFNetSession.h"
#include "KFNetLocker.h"

namespace KFrame
{
    KFNetServices::KFNetServices()
    {
        _is_shutdown = false;
        _thread_run = false;

        _uv_shut_mutex = new uv_mutex_t();
        _uv_send_mutex = new uv_mutex_t();
        _uv_connect_mutex = new uv_mutex_t();

        _uv_shut_async = new uv_async_t();
        _uv_send_async = new uv_async_t();
        _uv_connect_async = new uv_async_t();
        _uv_close_async = new uv_async_t();
    }

    KFNetServices::~KFNetServices()
    {
        _buff_length = 0;
        free( _buff_address );
        delete _net_event;

        uv_loop_delete( _uv_loop );
        _uv_loop = nullptr;

        delete _uv_shut_async;
        delete _uv_send_async;
        delete _uv_connect_async;
        delete _uv_close_async;

        delete ( uv_mutex_t* )_uv_shut_mutex;
        delete ( uv_mutex_t* )_uv_send_mutex;
        delete ( uv_mutex_t* )_uv_connect_mutex;
    }

    void KFNetServices::InitServices( uint32 eventcount, uint32 queuecount, uint32 messagetype )
    {
        if ( queuecount != 0 )
        {
            _queue_size = queuecount;
        }

        _net_event = new KFNetEvent();
        _net_event->InitEvent( eventcount );

        _message_type = messagetype;
        _buff_length = KFNetDefine::SerializeBuffLength;
        _buff_address = reinterpret_cast< char* >( malloc( _buff_length ) );

        _uv_loop = uv_loop_new();
        uv_mutex_init( ( uv_mutex_t* )_uv_shut_mutex );
        uv_mutex_init( ( uv_mutex_t* )_uv_send_mutex );
        uv_mutex_init( ( uv_mutex_t* )_uv_connect_mutex );

        uv_async_init( _uv_loop, _uv_shut_async, OnAsyncShutCallBack );
        uv_async_init( _uv_loop, _uv_send_async, OnAsyncSendCallBack );
        uv_async_init( _uv_loop, _uv_connect_async, OnAsyncConnectCallBack );
        uv_async_init( _uv_loop, _uv_close_async, OnAsyncCloseCallBack );
    }

    int32 KFNetServices::StartServices( const KFNetData* netdata )
    {
        _thread_run = true;
        return uv_thread_create( ( uv_thread_t* )&_thread_id, KFNetServices::RunServices, this );
    }

    void KFNetServices::RunServices( void* arguments )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( arguments );

        do
        {
            uv_run( netservices->_uv_loop, UV_RUN_DEFAULT );
        } while ( netservices->_thread_run );
    }

    void KFNetServices::ShutServices()
    {
        _thread_run = false;
        _is_shutdown = true;

        _uv_close_async->data = this;
        uv_async_send( _uv_close_async );

        uv_thread_join( ( uv_thread_t* )&_thread_id );
    }

    void KFNetServices::OnAsyncCloseCallBack( uv_async_t* handle )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( handle->data );

        uv_mutex_destroy( ( uv_mutex_t* )netservices->_uv_shut_mutex );
        uv_mutex_destroy( ( uv_mutex_t* )netservices->_uv_send_mutex );
        uv_mutex_destroy( ( uv_mutex_t* )netservices->_uv_connect_mutex );

        uv_close( ( uv_handle_t* )( netservices->_uv_shut_async ), nullptr );
        uv_close( ( uv_handle_t* )( netservices->_uv_send_async ), nullptr );
        uv_close( ( uv_handle_t* )( netservices->_uv_connect_async ), nullptr );
        uv_close( ( uv_handle_t* )( netservices->_uv_close_async ), nullptr );

        uv_stop( netservices->_uv_loop );
        uv_loop_close( netservices->_uv_loop );
    }

    void KFNetServices::SendNetMessage( KFNetSession* netsession )
    {
        if ( !netsession->IsNeedSend() )
        {
            return;
        }

        // 加入发送列表
        {
            KFNetLocker locker( ( uv_mutex_t* )_uv_send_mutex );
            _send_session.insert( netsession );
        }

        _uv_send_async->data = this;
        uv_async_send( _uv_send_async );
    }

    void KFNetServices::OnAsyncSendCallBack( uv_async_t* handle )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( handle->data );
        std::set< KFNetSession* > templist;
        {
            KFNetLocker locker( ( uv_mutex_t* )netservices->_uv_send_mutex );
            templist.swap( netservices->_send_session );
        }

        for ( auto netsession : templist )
        {
            netsession->StartSendMessage();
        }
    }

    void KFNetServices::CloseSession( KFNetSession* netsession )
    {
        if ( netsession->_is_shutdown )
        {
            return;
        }

        netsession->_is_shutdown = true;

        {
            // 加入到列表中
            KFNetLocker locker( ( uv_mutex_t* )_uv_shut_mutex );
            _shut_session.insert( netsession );
        }

        {
            // 删除发送列表
            KFNetLocker locker( ( uv_mutex_t* )_uv_send_mutex );
            _send_session.erase( netsession );
        }

        _uv_shut_async->data = this;
        uv_async_send( _uv_shut_async );
    }

    void KFNetServices::OnAsyncShutCallBack( uv_async_t* handle )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( handle->data );

        std::set< KFNetSession* > templist;
        {
            KFNetLocker locker( ( uv_mutex_t* )netservices->_uv_shut_mutex );
            templist.swap( netservices->_shut_session );
        }

        for ( auto netsession : templist )
        {
            netsession->CloseSession();
        }
    }

    void KFNetServices::StartSession( KFNetSession* netsession )
    {
        {
            // 加入到列表中
            KFNetLocker locker( ( uv_mutex_t* )_uv_connect_mutex );
            _connect_session.insert( netsession );
        }

        // 启动连接
        _uv_connect_async->data = this;
        uv_async_send( _uv_connect_async );
    }

    void KFNetServices::OnAsyncConnectCallBack( uv_async_t* handle )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( handle->data );

        std::set< KFNetSession* > templist;
        {
            KFNetLocker locker( ( uv_mutex_t* )netservices->_uv_connect_mutex );
            templist.swap( netservices->_connect_session );
        }

        for ( auto netsession : templist )
        {
            netsession->StartSession();
        }
    }

    char* KFNetServices::GetBuffAddress( uint32 msgid, uint32 length )
    {
        if ( length > _buff_length )
        {
            if ( length < 2 * KFNetDefine::SerializeBuffLength )
            {
                _buff_address = reinterpret_cast< char* >( realloc( _buff_address, length ) );
            }
        }

        return _buff_address;
    }
}