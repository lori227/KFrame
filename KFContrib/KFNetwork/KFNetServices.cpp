#include "KFrame.h"
#include "KFNetServices.h"
#include "KFNetSession.h"
#include "KFNetLocker.h"
#include "lz4/lib/lz4.h"

namespace KFrame
{
    KFNetServices::KFNetServices()
    {
        _is_shutdown = false;
        _thread_run = false;

        _uv_event_mutex = new uv_mutex_t();
        _uv_event_async = new uv_async_t();
        _uv_close_async = new uv_async_t();
    }

    KFNetServices::~KFNetServices()
    {
        _buff_length = 0;
        free( _buff_address );
        delete _net_event;

        uv_loop_delete( _uv_loop );
        _uv_loop = nullptr;

        delete _uv_event_async;
        delete _uv_close_async;
        delete ( uv_mutex_t* )_uv_event_mutex;
    }

    void KFNetServices::InitServices( uint32 eventcount, uint32 queuecount, uint32 messagetype, uint32 compress )
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

        _compress = compress;
        if ( _compress > 0 )
        {
            _compress_length = KFNetDefine::SerializeBuffLength;
            _compress_address = reinterpret_cast< char* >( malloc( _buff_length ) );
        }

        _uv_loop = uv_loop_new();
        uv_mutex_init( ( uv_mutex_t* )_uv_event_mutex );
        uv_async_init( _uv_loop, _uv_event_async, OnAsyncEventCallBack );
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

        uv_mutex_destroy( ( uv_mutex_t* )netservices->_uv_event_mutex );

        uv_close( ( uv_handle_t* )( netservices->_uv_event_async ), nullptr );
        uv_close( ( uv_handle_t* )( netservices->_uv_close_async ), nullptr );

        uv_stop( netservices->_uv_loop );
        uv_loop_close( netservices->_uv_loop );
    }


    void KFNetServices::SendEventToServices( KFNetSession* netsession, uint32 evnettype )
    {
        // 加入事件列表
        {
            KFNetLocker locker( ( uv_mutex_t* )_uv_event_mutex );
            _event_session[ netsession ] = evnettype;
        }

        _uv_event_async->data = this;
        uv_async_send( _uv_event_async );
    }

    void KFNetServices::OnAsyncEventCallBack( uv_async_t* handle )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( handle->data );
        std::map< KFNetSession*, uint32 > templist;
        {
            KFNetLocker locker( ( uv_mutex_t* )netservices->_uv_event_mutex );
            templist.swap( netservices->_event_session );
        }

        for ( auto& iter : templist )
        {
            auto netsession = iter.first;
            switch ( iter.second )
            {
            case KFNetDefine::ConnectEvent:
                netsession->StartSession();
                break;
            case KFNetDefine::SendEvent:
                netsession->StartSendMessage();
                break;
            case KFNetDefine::CloseEvent:
                netsession->CloseSession();
                break;
            case KFNetDefine::DisconnectEvent:
                netsession->CloseSession();
                netsession->OnDisconnect( 0, __FUNC_LINE__ );
                break;
            default:
                break;
            }
        }
    }

    char* KFNetServices::GetBuffAddress( uint32 msgid, uint32 length )
    {
        if ( length > _buff_length )
        {
            if ( length < 2 * KFNetDefine::SerializeBuffLength )
            {
                auto newaddress = reinterpret_cast< char* >( realloc( _buff_address, length ) );
                if ( newaddress != nullptr )
                {
                    _buff_length = length;
                    _buff_address = newaddress;
                }
            }
        }

        return _buff_address;
    }

    void KFNetServices::CalcCompressLength( uint32 length )
    {
        auto compresslength = LZ4_compressBound( length );
        if ( ( uint32 )compresslength > _compress_length )
        {
            auto newaddress = reinterpret_cast< char* >( realloc( _compress_address, compresslength ) );
            if ( newaddress != nullptr )
            {
                _compress_length = compresslength;
                _compress_address = newaddress;
            }
        }
    }

    const char* KFNetServices::Encode( const char* data, uint32& length )
    {
        if ( _compress > 0u )
        {
            CalcCompressLength( length );

            // 压缩
            length = LZ4_compress_fast( data, _compress_address, length, _compress_length, _compress );
            data = _compress_address;
        }

        return data;
    }

    const char* KFNetServices::Decode( const char* data, uint32& length )
    {
        if ( _compress > 0u )
        {
            CalcCompressLength( length );

            // 解压
            length = LZ4_decompress_safe( data, _compress_address, length, _compress_length );
            data = _compress_address;
        }

        return data;
    }
}