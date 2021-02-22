#include "KFrame.h"
#include "KFCompress.h"
#include "KFDecode.h"
#include "lz4/lib/lz4.h"
#include "zstd/lib/zstd.h"

namespace KFrame
{
    static uint32 _compress_length = KFBufferEnum::Buff_20M;
    static uint32 _data_length = KFBufferEnum::Buff_40M;

    bool KFCompress::Compress( const std::string& value, std::string& result, uint32 compress_type, uint32 compress_level, bool convert )
    {
        result.clear();
        if ( value.empty() )
        {
            return true;
        }

        // 压缩数据
        auto length = 0;
        auto compress_buffer = __KF_UINT8__( _compress_length );
        switch ( compress_type )
        {
        case KFCompressEnum::LZ4:
            length = LZ4_compress_fast( value.data(), ( int8* )compress_buffer, ( int32 )value.size(), ( int32 )_compress_length, ( int32 )compress_level );
            break;
        case KFCompressEnum::ZSTD:
            length = ( int32 )ZSTD_compress( compress_buffer, _compress_length, value.data(), value.size(), ( int32 )compress_level );
            break;
        default:
            result = value;
            break;
        }

        if ( length > 0 )
        {
            if ( convert )
            {
                auto data_buffer = __KF_INT8__( _data_length );

                // 转为可视字符串
                auto data_length = KFDecode::UByteToString( compress_buffer, length, data_buffer, _data_length );
                result.assign( data_buffer, data_length );
            }
            else
            {
                result.assign( ( int8* )compress_buffer, length );
            }
        }
        else if ( length < 0 )
        {
            __LOG_ERROR__( "compress failed=[{}]", length );
        }

        return length >= 0;
    }

    bool KFCompress::UnCompress( const std::string& value, std::string& result, uint32 compress_type, bool convert )
    {
        result.clear();
        if ( value.empty() )
        {
            return true;
        }

        // 转成压缩数据
        auto buffer = ( uint8* )value.data();
        auto length = ( int32 )value.size();
        if ( convert )
        {
            buffer = __KF_UINT8__( _compress_length );
            length = KFDecode::StringToUByte( value.data(), ( uint32 )value.size(), buffer, _compress_length );
        }

        // 解压缩
        auto data_length = 0;
        auto data_buffer = __KF_INT8__( _data_length );
        switch ( compress_type )
        {
        case KFCompressEnum::LZ4:
            data_length = LZ4_decompress_safe( ( int8* )buffer, data_buffer, length, _data_length );
            break;
        case KFCompressEnum::ZSTD:
            data_length = ( int32 )ZSTD_decompress( data_buffer, _data_length, buffer, length );
            break;
        default:
            result = value;
            break;
        }

        if ( data_length > 0 )
        {
            result.assign( data_buffer, data_length );
        }
        else if ( data_length < 0 )
        {
            __LOG_ERROR__( "uncompress failed=[{}]", data_length );
        }

        return data_length >= 0;
    }
}