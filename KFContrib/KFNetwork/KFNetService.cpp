#include "KFrame.h"
#include "KFNetService.hpp"
#include "KFNetSession.hpp"
#include "KFNetLocker.hpp"
#include "lz4/lib/lz4.h"
#include "zstd/lib/zstd.h"

namespace KFrame
{
    KFNetService::KFNetService()
    {
        _is_shutdown = false;
        _thread_run = false;

        _uv_event_async = __NEW_OBJECT__( uv_async_t );
        _uv_close_async = __NEW_OBJECT__( uv_async_t );
    }

    KFNetService::~KFNetService()
    {
        _max_serialize_buff_length = 0;
        free( _serialize_buff );
        __DELETE_OBJECT__( _net_event );

        uv_loop_delete( _uv_loop );
        _uv_loop = nullptr;

        __DELETE_OBJECT__( _uv_event_async );
        __DELETE_OBJECT__( _uv_close_async );
    }

    void KFNetService::InitService( uint32 event_count, uint32 queue_count, uint32 message_type )
    {
        if ( queue_count != 0 )
        {
            _queue_size = queue_count;
        }

        _net_event = __NEW_OBJECT__( KFNetEvent );
        _net_event->InitEvent( event_count );

        _async_event_list.InitQueue( 10000u, 10000u );

        _message_type = message_type;
        _max_serialize_buff_length = KFNetDefine::SerializeBuffLength;
        _serialize_buff = reinterpret_cast< char* >( malloc( _max_serialize_buff_length ) );

        _uv_loop = uv_loop_new();
        uv_async_init( _uv_loop, _uv_event_async, OnAsyncEventCallBack );
        uv_async_init( _uv_loop, _uv_close_async, OnAsyncCloseCallBack );
    }

    void KFNetService::InitCompress( uint32 compress_type, uint32 compress_level, uint32 compress_length )
    {
        _compress_encrypt._compress_type = compress_type;
        _compress_encrypt._compress_level = compress_level;
        _compress_encrypt._compress_length = compress_length;

        _max_compress_buff_length = KFNetDefine::SerializeBuffLength;
        _compress_buff = reinterpret_cast< char* >( malloc( _max_compress_buff_length ) );
    }

    void KFNetService::InitEncrypt( const std::string& encrypt_key, bool open_encrypt )
    {
        _compress_encrypt._encrypt_key = encrypt_key;
        _compress_encrypt._open_encrypt = open_encrypt;
    }

    int32 KFNetService::StartService( const KFNetData* net_data )
    {
        _thread_run = true;
        return uv_thread_create( ( uv_thread_t* )&_thread_id, KFNetService::RunServices, this );
    }

    void KFNetService::RunServices( void* arguments )
    {
        auto net_service = reinterpret_cast< KFNetService* >( arguments );

        do
        {
            uv_run( net_service->_uv_loop, UV_RUN_DEFAULT );
        } while ( net_service->_thread_run );
    }

    void KFNetService::CloseService()
    {
        _thread_run = false;
        _is_shutdown = true;

        _uv_close_async->data = this;
        uv_async_send( _uv_close_async );

        uv_thread_join( ( uv_thread_t* )&_thread_id );
    }

    void KFNetService::OnAsyncCloseCallBack( uv_async_t* handle )
    {
        auto net_service = reinterpret_cast< KFNetService* >( handle->data );

        uv_close( ( uv_handle_t* )( net_service->_uv_event_async ), nullptr );
        uv_close( ( uv_handle_t* )( net_service->_uv_close_async ), nullptr );

        uv_stop( net_service->_uv_loop );
        uv_loop_close( net_service->_uv_loop );
    }

    void KFNetService::SendEventToService( uint32 event_type, std::shared_ptr<KFNetSession> net_session )
    {
        auto async_event = __MAKE_SHARED__( KFAsyncEvent );
        async_event->_type = event_type;
        async_event->_session = net_session;
        _async_event_list.Push( async_event );

        // 通知io线程
        _uv_event_async->data = this;
        uv_async_send( _uv_event_async );
    }

    void KFNetService::OnAsyncEventCallBack( uv_async_t* handle )
    {
        auto net_service = reinterpret_cast< KFNetService* >( handle->data );

        // 处理事件队列
        auto async_event = net_service->_async_event_list.Pop();
        while ( async_event != nullptr )
        {
            async_event->_session->HandleServiceEvent( async_event->_type );
            async_event = net_service->_async_event_list.Pop();
        }
    }

    char* KFNetService::GetBuffAddress( uint32 msg_id, uint32 msg_length )
    {
        if ( msg_length > _max_serialize_buff_length )
        {
            if ( msg_length < 2 * KFNetDefine::SerializeBuffLength )
            {
                auto new_address = reinterpret_cast< char* >( realloc( _serialize_buff, msg_length ) );
                if ( new_address != nullptr )
                {
                    _serialize_buff = new_address;
                    _max_serialize_buff_length = msg_length;
                }
            }
        }

        return _serialize_buff;
    }

    void KFNetService::LZ4CalcCompressLength( const char* data, uint32 length, bool is_compress )
    {
        auto compress_length = 0u;
        if ( is_compress )
        {
            compress_length = LZ4_compressBound( length );
        }

        if ( compress_length > _max_compress_buff_length )
        {
            auto new_address = reinterpret_cast< char* >( realloc( _compress_buff, compress_length ) );
            if ( new_address != nullptr )
            {
                _compress_buff = new_address;
                _max_compress_buff_length = compress_length;
            }
        }
    }

    void KFNetService::ZSTDCalcCompressLength( const char* data, uint32 length, bool is_compress )
    {
        auto compress_length = 0u;
        if ( is_compress )
        {
            compress_length = ( uint32 )ZSTD_compressBound( length );
        }
        else
        {
            compress_length = ( uint32 )ZSTD_getFrameContentSize( data, length );
        }

        if ( compress_length > _max_compress_buff_length )
        {
            auto new_address = reinterpret_cast< char* >( realloc( _compress_buff, compress_length ) );
            if ( new_address != nullptr )
            {
                _compress_buff = new_address;
                _max_compress_buff_length = compress_length;
            }
        }
    }

    const char* KFNetService::CompressData( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32& length )
    {
        switch ( compress_encrypt->_compress_type )
        {
        case KFCompressEnum::LZ4:
        {
            // 重新计算长度
            LZ4CalcCompressLength( data, length, true );
            length = LZ4_compress_fast( data, _compress_buff, length, _max_compress_buff_length, compress_encrypt->_compress_level );
            data = _compress_buff;
        }
        break;
        case KFCompressEnum::ZSTD:
        {
            // 重新计算长度
            ZSTDCalcCompressLength( data, length, true );
            length = ( uint32 )ZSTD_compress( _compress_buff, _max_compress_buff_length, data, length, ( int32 )compress_encrypt->_compress_level );
            data = _compress_buff;
        }
        break;
        default:
            break;
        }

        return data;
    }

    const char* KFNetService::DeCompressData( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32& length )
    {
        switch ( compress_encrypt->_compress_type )
        {
        case KFCompressEnum::LZ4:
        {
            // 解压长度 这里要计算一下
            LZ4CalcCompressLength( data, length, false );
            length = LZ4_decompress_safe( data, _compress_buff, length, _max_compress_buff_length );
            data = _compress_buff;
        }
        break;
        case KFCompressEnum::ZSTD:
        {
            // 重新计算长度
            ZSTDCalcCompressLength( data, length, false );
            length = ( int32 )ZSTD_decompress( _compress_buff, _max_compress_buff_length, data, length );
            data = _compress_buff;
        }
        break;
        default:
            break;
        }

        return data;
    }

    const char* KFNetService::EncryptData( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32& length )
    {
        // 加密逻辑

        return data;
    }

    const char* KFNetService::DeEncryptData( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32& length )
    {
        // 解密逻辑

        return data;
    }

    std::tuple< const char*, uint32, uint16 > KFNetService::Encode( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32 length )
    {
        uint16 ret_flag = 0;
        auto ret_data = data;
        auto ret_length = length;

        if ( compress_encrypt != nullptr && length > 0u )
        {
            // 压缩
            if ( length >= compress_encrypt->_compress_length )
            {
                ret_flag |= KFNetDefine::Compress;
                ret_data = CompressData( compress_encrypt, ret_data, ret_length );
            }

            // 加密
            if ( compress_encrypt->_open_encrypt )
            {
                ret_flag |= KFNetDefine::Encrypt;
                ret_data = EncryptData( compress_encrypt, ret_data, ret_length );
            }
        }

        return std::make_tuple( ret_data, ret_length, ret_flag );
    }

    std::tuple< const char*, uint32 > KFNetService::Decode( const KFNetCompressEncrypt* compress_encrypt, const char* data, uint32 length, uint16 flag )
    {
        auto ret_data = data;
        auto ret_length = length;

        if ( compress_encrypt != nullptr && length > 0u )
        {
            // 解密
            if ( ( flag & KFNetDefine::Encrypt ) != 0 )
            {
                ret_data = DeEncryptData( compress_encrypt, ret_data, ret_length );
            }

            // 解压缩
            if ( ( flag & KFNetDefine::Compress ) != 0 )
            {
                ret_data = DeCompressData( compress_encrypt, ret_data, ret_length );
            }
        }

        return std::make_tuple( ret_data, ret_length );
    }
}

