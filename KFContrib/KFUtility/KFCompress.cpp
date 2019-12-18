#include "KFrame.h"
#include "KFCompress.h"
#include "KFDecode.h"
#include "lz4/lib/lz4.h"

namespace KFrame
{
    static uint32 _zib_length = KFBufferEnum::Buff_20M;
    static uint32 _data_length = KFBufferEnum::Buff_40M;
    static uint32 _compress_level = 1;

    int32 KFCompress::Zib( const std::string& value, std::string& result )
    {
        result.clear();
        if ( value.empty() )
        {
            return 0;
        }

        // 压缩数据
        auto zibbuffer = __KF_UINT8__( _zib_length );
        auto ziblength = LZ4_compress_fast( value.data(), ( int8* )zibbuffer, ( int32 )value.size(), ( int32 )_zib_length, ( int32 )_compress_level );
        if ( ziblength > 0 )
        {
            result.assign( ( int8* )zibbuffer, ziblength );
        }
        else
        {
            __LOG_ERROR__( "compress failed=[{}]", ziblength );
        }

        return ziblength;
    }

    int32 KFCompress::UnZib( const std::string& value, std::string& result )
    {
        result.clear();
        if ( value.empty() )
        {
            return 0;
        }

        // 解压缩
        auto zibbuffer = __KF_UINT8__( _zib_length );
        auto ziblength = LZ4_decompress_safe( value.data(), ( int8* )zibbuffer, ( int32 )value.size(), ( int32 )_zib_length );
        if ( ziblength > 0 )
        {
            result.assign( ( int8* )zibbuffer, ziblength );
        }
        else
        {
            __LOG_ERROR__( "uncompress failed=[{}]", ziblength );
        }

        return ziblength;
    }


    int32 KFCompress::Compress( const std::string& value, std::string& result )
    {
        result.clear();
        if ( value.empty() )
        {
            return 0;
        }

        // 压缩数据
        auto zibbuffer = __KF_UINT8__( _zib_length );
        auto length = LZ4_compress_fast( value.data(), ( int8* )zibbuffer, ( int32 )value.size(), ( int32 )_zib_length, ( int32 )_compress_level );
        if ( length > 0 )
        {
            auto databuffer = __KF_INT8__( _data_length );

            // 转为可视字符串
            auto datalength = KFDecode::UByteToString( zibbuffer, length, databuffer, _data_length );
            result.assign( databuffer, datalength );
        }
        else
        {
            __LOG_ERROR__( "compress failed=[{}]", length );
        }

        return length;
    }

    int32 KFCompress::UnCompress( const std::string& value, std::string& result )
    {
        result.clear();
        if ( value.empty() )
        {
            return 0;
        }

        // 转成压缩数据
        auto zibbuffer = __KF_UINT8__( _zib_length );
        auto ziblength = KFDecode::StringToUByte( value.data(), ( uint32 )value.size(), zibbuffer, _zib_length );

        // 解压缩
        auto databuffer = __KF_INT8__( _data_length );
        auto datalength = LZ4_decompress_safe( ( int8* )zibbuffer, databuffer, ziblength, _data_length );
        if ( datalength > 0 )
        {
            result.assign( databuffer, datalength );
        }
        else
        {
            __LOG_ERROR__( "uncompress failed=[{}]", datalength );
        }

        return datalength;
    }
}