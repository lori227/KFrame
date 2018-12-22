#include "KFrame.h"
#include "uv.h"
#include "KFNetServices.h"
#include "KFNetSession.h"
#include "KFNetLocker.h"

namespace KFrame
{
    KFNetServices::KFNetServices()
    {
        _thread_run = false;
        _is_shutdown = false;
        _uv_loop = nullptr;
        _net_event = nullptr;
        _buff_address = nullptr;
        _buff_length = 0;
        _queue_size = 10000;
        _now_time = 0;
        _message_type = 0;
    }

    KFNetServices::~KFNetServices()
    {
        _buff_length = 0;
        free( _buff_address );
        __KF_DELETE__( _net_event );

        uv_mutex_destroy( &_uv_shut_mutex );
        uv_mutex_destroy( &_uv_send_mutex );
        uv_mutex_destroy( &_uv_connect_mutex );

        uv_loop_delete( _uv_loop );
        _uv_loop = nullptr;
    }

    void KFNetServices::InitServices( uint32 eventcount, uint32 queuecount, uint32 messagetype )
    {
        if ( queuecount != 0 )
        {
            _queue_size = queuecount;
        }

        _net_event = __KF_NEW__( KFNetEvent );
        _net_event->InitEvent( eventcount );

        _message_type = messagetype;
        _buff_length = KFNetDefine::SerializeBuffLength;
        _buff_address = reinterpret_cast< char* >( malloc( _buff_length ) );

        _uv_loop = uv_loop_new();
        uv_mutex_init( &_uv_shut_mutex );
        uv_mutex_init( &_uv_send_mutex );
        uv_mutex_init( &_uv_connect_mutex );

        uv_async_init( _uv_loop, &_uv_shut_async, OnAsyncShutCallBack );
        uv_async_init( _uv_loop, &_uv_send_async, OnAsyncSendCallBack );
        uv_async_init( _uv_loop, &_uv_connect_async, OnAsyncConnectCallBack );
    }

    int32 KFNetServices::StartServices( const KFNetSetting* kfsetting )
    {
        _thread_run = true;
        return uv_thread_create( &_thread_id, KFNetServices::RunServices, this );
    }

    void KFNetServices::RunServices( void* arguments )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( arguments );

        do
        {
            uv_run( netservices->_uv_loop, UV_RUN_DEFAULT );
        } while ( netservices->_thread_run );

        uv_loop_close( netservices->_uv_loop );
    }

    void KFNetServices::ShutServices()
    {
        _thread_run = false;
        _is_shutdown = true;

        //uv_mutex_destroy( &_uv_shut_mutex );
        //uv_mutex_destroy( &_uv_send_mutex );
        //uv_mutex_destroy( &_uv_connect_mutex );

        //uv_close( reinterpret_cast< uv_handle_t* >( &_uv_shut_async ), nullptr );
        //uv_close( reinterpret_cast< uv_handle_t* >( &_uv_send_async ), nullptr );
        //uv_close( reinterpret_cast< uv_handle_t* >( &_uv_connect_async ), nullptr );

        uv_stop( _uv_loop );
        uv_thread_join( &_thread_id );
    }

    void KFNetServices::SendNetMessage( KFNetSession* netsession )
    {
        if ( !netsession->IsNeedSend() )
        {
            return;
        }

        // 加入发送列表
        {
            KFNetLocker locker( &_uv_send_mutex );
            _send_session.insert( netsession );
        }

        _uv_send_async.data = this;
        uv_async_send( &_uv_send_async );
    }

    void KFNetServices::OnAsyncSendCallBack( uv_async_t* handle )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( handle->data );
        std::set< KFNetSession* > templist;
        {
            KFNetLocker locker( &netservices->_uv_send_mutex );
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
            KFNetLocker locker( &_uv_shut_mutex );
            _shut_session.insert( netsession );
        }

        {
            // 删除发送列表
            KFNetLocker locker( &_uv_send_mutex );
            _send_session.erase( netsession );
        }

        _uv_shut_async.data = this;
        uv_async_send( &_uv_shut_async );
    }

    void KFNetServices::OnAsyncShutCallBack( uv_async_t* handle )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( handle->data );

        std::set< KFNetSession* > templist;
        {
            KFNetLocker locker( &netservices->_uv_shut_mutex );
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
            KFNetLocker locker( &_uv_connect_mutex );
            _connect_session.insert( netsession );
        }

        // 启动连接
        _uv_connect_async.data = this;
        uv_async_send( &_uv_connect_async );
    }

    void KFNetServices::OnAsyncConnectCallBack( uv_async_t* handle )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( handle->data );

        std::set< KFNetSession* > templist;
        {
            KFNetLocker locker( &netservices->_uv_connect_mutex );
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