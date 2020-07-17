#include "KFrame.h"
#include "KFNetServices.hpp"
#include "KFNetSession.hpp"
#include "KFNetLocker.hpp"
#include "lz4/lib/lz4.h"
#include "zstd/lib/zstd.h"

namespace KFrame
{
    KFNetServices::KFNetServices()
    {
        _is_shutdown = false;
        _thread_run = false;

        _uv_event_async = new uv_async_t();
        _uv_close_async = new uv_async_t();
    }

    KFNetServices::~KFNetServices()
    {
        _max_serialize_buff_length = 0;
        free( _serialize_buff );
        delete _net_event;

        uv_loop_delete( _uv_loop );
        _uv_loop = nullptr;

        delete _uv_event_async;
        delete _uv_close_async;
    }

    void KFNetServices::InitServices( uint32 eventcount, uint32 queuecount, uint32 messagetype )
    {
        if ( queuecount != 0 )
        {
            _queue_size = queuecount;
        }

        _net_event = new KFNetEvent();
        _net_event->InitEvent( eventcount );

        _event_session.InitQueue( 10000u );

        _message_type = messagetype;
        _max_serialize_buff_length = KFNetDefine::SerializeBuffLength;
        _serialize_buff = reinterpret_cast< char* >( malloc( _max_serialize_buff_length ) );

        _uv_loop = uv_loop_new();
        uv_async_init( _uv_loop, _uv_event_async, OnAsyncEventCallBack );
        uv_async_init( _uv_loop, _uv_close_async, OnAsyncCloseCallBack );
    }

    void KFNetServices::InitCompress( uint32 compresstype, uint32 compresslevel, uint32 compresslength )
    {
        _compress_encrypt._compress_type = compresstype;
        _compress_encrypt._compress_level = compresslevel;
        _compress_encrypt._compress_length = compresslength;

        _max_compress_buff_length = KFNetDefine::SerializeBuffLength;
        _compress_buff = reinterpret_cast< char* >( malloc( _max_compress_buff_length ) );
    }

    void KFNetServices::InitEncrypt( const std::string& encryptkey, bool openencrypt )
    {
        _compress_encrypt._encrypt_key = encryptkey;
        _compress_encrypt._open_encrypt = openencrypt;
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

        uv_close( ( uv_handle_t* )( netservices->_uv_event_async ), nullptr );
        uv_close( ( uv_handle_t* )( netservices->_uv_close_async ), nullptr );

        uv_stop( netservices->_uv_loop );
        uv_loop_close( netservices->_uv_loop );
    }

    void KFNetServices::SendEventToServices( KFNetSession* netsession, uint32 eventtype )
    {
        if ( !netsession->SetMainLoopEvent( eventtype ) )
        {
            return;
        }

        // 加入队列
        _event_session.PushObject( netsession );

        // 通知io线程
        _uv_event_async->data = this;
        uv_async_send( _uv_event_async );
    }

    void KFNetServices::OnAsyncEventCallBack( uv_async_t* handle )
    {
        auto netservices = reinterpret_cast< KFNetServices* >( handle->data );

        // 处理事件队列
        auto netsession = netservices->_event_session.PopObject();
        while ( netsession != nullptr )
        {
            netsession->HandleMainLoopEvent();
            netsession = netservices->_event_session.PopObject();
        }
    }

    char* KFNetServices::GetBuffAddress( uint32 msgid, uint32 length )
    {
        if ( length > _max_serialize_buff_length )
        {
            if ( length < 2 * KFNetDefine::SerializeBuffLength )
            {
                auto newaddress = reinterpret_cast< char* >( realloc( _serialize_buff, length ) );
                if ( newaddress != nullptr )
                {
                    _max_serialize_buff_length = length;
                    _serialize_buff = newaddress;
                }
            }
        }

        return _serialize_buff;
    }

    void KFNetServices::LZ4CalcCompressLength( const char* data, uint32 length, bool iscompress )
    {
        auto compresslength = 0u;
        if ( iscompress )
        {
            compresslength = LZ4_compressBound( length );
        }

        if ( compresslength > _max_compress_buff_length )
        {
            auto newaddress = reinterpret_cast< char* >( realloc( _compress_buff, compresslength ) );
            if ( newaddress != nullptr )
            {
                _compress_buff = newaddress;
                _max_compress_buff_length = compresslength;
            }
        }
    }

    void KFNetServices::ZSTDCalcCompressLength( const char* data, uint32 length, bool iscompress )
    {
        auto compresslength = 0u;
        if ( iscompress )
        {
            compresslength = ( uint32 )ZSTD_compressBound( length );
        }
        else
        {
            compresslength = ( uint32 )ZSTD_getFrameContentSize( data, length );
        }

        if ( compresslength > _max_compress_buff_length )
        {
            auto newaddress = reinterpret_cast< char* >( realloc( _compress_buff, compresslength ) );
            if ( newaddress != nullptr )
            {
                _compress_buff = newaddress;
                _max_compress_buff_length = compresslength;
            }
        }
    }

    const char* KFNetServices::CompressData( const KFNetCompressEncrypt* compressencrypt, const char* data, uint32& length )
    {
        switch ( compressencrypt->_compress_type )
        {
        case KFCompressEnum::LZ4:
        {
            // 重新计算长度
            LZ4CalcCompressLength( data, length, true );
            length = LZ4_compress_fast( data, _compress_buff, length, _max_compress_buff_length, compressencrypt->_compress_level );
            data = _compress_buff;
        }
        break;
        case KFCompressEnum::ZSTD:
        {
            // 重新计算长度
            ZSTDCalcCompressLength( data, length, true );
            length = ( uint32 )ZSTD_compress( _compress_buff, _max_compress_buff_length, data, length, ( int32 )compressencrypt->_compress_level );
            data = _compress_buff;
        }
        break;
        default:
            break;
        }

        return data;
    }

    const char* KFNetServices::DeCompressData( const KFNetCompressEncrypt* compressencrypt, const char* data, uint32& length )
    {
        switch ( compressencrypt->_compress_type )
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

    const char* KFNetServices::EncryptData( const KFNetCompressEncrypt* compressencrypt, const char* data, uint32& length )
    {
        // 加密逻辑

        return data;
    }

    const char* KFNetServices::DeEncryptData( const KFNetCompressEncrypt* compressencrypt, const char* data, uint32& length )
    {
        // 解密逻辑

        return data;
    }

    std::tuple< const char*, uint32, uint16 > KFNetServices::Encode( const KFNetCompressEncrypt* compressencrypt, const char* data, uint32 length )
    {
        uint16 retflag = 0;
        auto retdata = data;
        auto retlength = length;

        if ( compressencrypt != nullptr && length > 0u )
        {
            // 压缩
            if ( length >= compressencrypt->_compress_length )
            {
                retflag |= KFNetDefine::Compress;
                retdata = CompressData( compressencrypt, retdata, retlength );
            }

            // 加密
            if ( compressencrypt->_open_encrypt )
            {
                retflag |= KFNetDefine::Encrypt;
                retdata = EncryptData( compressencrypt, retdata, retlength );
            }
        }

        return std::make_tuple( retdata, retlength, retflag );
    }

    std::tuple< const char*, uint32 > KFNetServices::Decode( const KFNetCompressEncrypt* compressencrypt, const char* data, uint32 length, uint16 flag )
    {
        auto retdata = data;
        auto retlength = length;

        if ( compressencrypt != nullptr && length > 0u )
        {
            // 解密
            if ( ( flag & KFNetDefine::Encrypt ) != 0 )
            {
                retdata = DeEncryptData( compressencrypt, retdata, retlength );
            }

            // 解压缩
            if ( ( flag & KFNetDefine::Compress ) != 0 )
            {
                retdata = DeCompressData( compressencrypt, retdata, retlength );
            }
        }

        return std::make_tuple( retdata, retlength );
    }
}

