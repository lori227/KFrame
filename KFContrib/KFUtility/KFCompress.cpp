#include "KFrame.h"
#include "KFCompress.h"
#include "KFDecode.h"
#include "lz4/lib/lz4.h"
#include "zstd/lib/zstd.h"

namespace KFrame
{
    static uint32 _compress_length = KFBufferEnum::Buff_20M;
    static uint32 _data_length = KFBufferEnum::Buff_40M;

    bool KFCompress::Compress( const std::string& value, std::string& result, uint32 compresstype, uint32 compresslevel, bool convert )
    {
        result.clear();
        if ( value.empty() )
        {
            return true;
        }

        // 压缩数据
        auto length = 0;
        auto compressbuffer = __KF_UINT8__( _compress_length );
        switch ( compresstype )
        {
        case KFCompressEnum::LZ4:
            length = LZ4_compress_fast( value.data(), ( int8* )compressbuffer, ( int32 )value.size(), ( int32 )_compress_length, ( int32 )compresslevel );
            break;
        case KFCompressEnum::ZSTD:
            length = ( int32 )ZSTD_compress( compressbuffer, _compress_length, value.data(), value.size(), ( int32 )compresslevel );
            break;
        default:
            result = value;
            break;
        }

        if ( length > 0 )
        {
            if ( convert )
            {
                auto databuffer = __KF_INT8__( _data_length );

                // 转为可视字符串
                auto datalength = KFDecode::UByteToString( compressbuffer, length, databuffer, _data_length );
                result.assign( databuffer, datalength );

                __LOG_DEBUG__( "compress source=[{}] fast=[{}] string=[{}]", value.size(), length, datalength );
            }
            else
            {
                result.assign( ( int8* )compressbuffer, length );
            }
        }
        else if ( length < 0 )
        {
            __LOG_ERROR__( "compress failed=[{}]", length );
        }

        return length >= 0;
    }

    bool KFCompress::UnCompress( const std::string& value, std::string& result, uint32 compresstype, bool convert )
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
        auto datalength = 0;
        auto databuffer = __KF_INT8__( _data_length );
        switch ( compresstype )
        {
        case KFCompressEnum::LZ4:
            datalength = LZ4_decompress_safe( ( int8* )buffer, databuffer, length, _data_length );
            break;
        case KFCompressEnum::ZSTD:
            datalength = ( int32 )ZSTD_decompress( databuffer, _data_length, buffer, length );
            break;
        default:
            result = value;
            break;
        }

        if ( datalength > 0 )
        {
            result.assign( databuffer, datalength );
        }
        else if ( datalength < 0 )
        {
            __LOG_ERROR__( "uncompress failed=[{}]", datalength );
        }

        return datalength >= 0;
    }
}