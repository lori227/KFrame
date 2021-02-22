#include "KFConvert.h"
#include "iconv.h"

namespace KFrame
{
    static const char* _gb_code = "gb2312";
    static const char* _utf8_code = "utf-8";

    std::string KFConvert::ToCovert( void* iconv_handle, const std::string& source )
    {
        auto in_size = source.size();
        const char* in_buff = source.data();

        static const uint32 _length = 1024 * 10;

        char iconv_buff[ _length ];
        auto out_buff = iconv_buff;
        uint32 iconv_size = _length;

        auto iconv_result = iconv( iconv_handle, ( char** )( &in_buff ), ( size_t* )&in_size, ( char** )( &out_buff ), ( size_t* )&iconv_size );

        //关闭字符集转换
        iconv_close( iconv_handle );

        std::string result;
        result.assign( iconv_buff, _length - iconv_size );
        return result;
    }

    std::string KFConvert::ToUTF8( const std::string& source )
    {
        //打开字符集转换
        auto iconv_handle = iconv_open( _utf8_code, _gb_code );
        if ( iconv_handle == ( iconv_t ) -1 )
        {
            return "";
        }

        return ToCovert( iconv_handle, source );
    }

    std::string KFConvert::ToAscii( const std::string& source )
    {
        //打开字符集转换
        auto iconv_handle = iconv_open( _gb_code, _utf8_code );
        if ( iconv_handle == ( iconv_t ) -1 )
        {
            return "";
        }

        return ToCovert( iconv_handle, source );
    }
}