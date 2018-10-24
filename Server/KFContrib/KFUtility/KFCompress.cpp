#include "KFrame.h"
#include "KFCompress.h"
#include "zlib/zlib.h"
#include "KFDecode.h"

namespace KFrame
{
    int32 KFCompress::Zib( const std::string& value, std::string& result )
    {
        result.clear();
        if ( value.empty() )
        {
            return 0;
        }


        uLongf ziplength = KFBufferEnum::Buff_20M;
        auto zipbuffer = __KF_UINT8__( ziplength );

        // 压缩数据
        // uLongf compresslength = 0;
        auto recode = compress( zipbuffer, &ziplength, reinterpret_cast< const uint8* >( value.data() ), static_cast< uLong >( value.size() ) );
        if ( recode == Z_OK )
        {
            result.assign( reinterpret_cast< int8* >( zipbuffer ), ziplength );
        }

        return recode;
    }

    int32 KFCompress::Compress( const std::string& value, std::string& result )
    {
        result.clear();
        if ( value.empty() )
        {
            return 0;
        }

        uLongf ziplength = KFBufferEnum::Buff_20M;
        auto zipbuffer = __KF_UINT8__( ziplength );

        // 压缩数据
        auto recode = compress( zipbuffer, &ziplength, reinterpret_cast< const uint8* >( value.data() ), static_cast< uLong >( value.size() ) );
        if ( recode == Z_OK )
        {
            uint32 maxlength = KFBufferEnum::Buff_40M;
            auto savebuffer = __KF_INT8__( maxlength );

            // 转为可视字符串
            auto savelength = KFDecode::UByteToString( zipbuffer, ziplength, savebuffer, maxlength );
            result.assign( savebuffer, savelength );
        }

        return recode;
    }

    int32 KFCompress::UnZib( const std::string& value, std::string& result )
    {
        result.clear();
        if ( value.empty() )
        {
            return 0;
        }

        uLongf ziplength = KFBufferEnum::Buff_20M;
        auto zipbuffer = __KF_UINT8__( ziplength );

        // 解压缩
        auto recode = uncompress( zipbuffer, &ziplength, reinterpret_cast< const uint8* >( value.c_str() ), static_cast< uLong >( value.size() ) );
        if ( recode == Z_OK )
        {
            result.assign( reinterpret_cast< int8* >( zipbuffer ), ziplength );
        }

        return recode;
    }

    int32 KFCompress::UnCompress( const std::string& value, std::string& result )
    {
        result.clear();
        if ( value.empty() )
        {
            return 0;
        }

        auto ziplength = KFBufferEnum::Buff_20M;
        auto zipbuffer = __KF_UINT8__( ziplength );

        // 转成压缩数据
        auto length = KFDecode::StringToUByte( value.data(), static_cast< uint32 >( value.size() ), zipbuffer, ziplength );

        uLongf maxlength = KFBufferEnum::Buff_40M;
        auto resultbuffer = __KF_UINT8__( maxlength );

        // 解压缩
        auto recode = uncompress( resultbuffer, &maxlength, zipbuffer, length );
        if ( recode == Z_OK )
        {
            result.assign( reinterpret_cast< int8* >( resultbuffer ), maxlength );
        }

        return recode;
    }
}