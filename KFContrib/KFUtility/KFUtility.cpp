#include "KFUtility.h"
#include "KFMacros.h"
#include "Poco/File.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <winsock2.h>
    #include <time.h>
#else
    #include <sys/time.h>
#endif

namespace KFrame
{
    bool KFUtility::IsNumber( const std::string& value )
    {
        // 这里只判断第一个字符
        if ( value.empty() )
        {
            return false;
        }

        auto ch = value.at( 0 );
        return ch >= '0' && ch <= '9';
    }

    bool KFUtility::IsZeroFloat( float value, float epsilon )
    {
        return std::abs( value ) <= epsilon;
    }

    bool KFUtility::IsZeroDouble( double value, double epsilon )
    {
        return std::abs( value ) <= epsilon;
    }

    uint32 KFUtility::CalcDistance( uint32 nowx, uint32 nowy, uint32 targetx, uint32 targety )
    {
        auto x = __MAX__( nowx, targetx ) - __MIN__( nowx, targetx );
        auto y = __MAX__( nowy, targety ) - __MIN__( nowy, targety );

        auto length = sqrt( x * x + y * y );
        return static_cast<uint32>( length );
    }

    bool KFUtility::CheckInDistance( uint32 nowx, uint32 nowy, uint32 targetx, uint32 targety, uint32 distance )
    {
        auto length = CalcDistance( nowx, nowy, targetx, targety );
        return length <= distance;
    }

    std::string KFUtility::SplitString( std::string& srcstring, std::string split/* = DEFAULT_SPLIT_STRING*/ )
    {
        if ( srcstring.empty() )
        {
            return "";
        }

        std::string result = "";
        auto pos = srcstring.find( split );
        if ( pos == std::string::npos )
        {
            result = srcstring;
            srcstring.clear();
            return result;
        }

        result = srcstring.substr( 0, pos );
        srcstring = srcstring.substr( pos + split.size() );
        return result;
    }

    void KFUtility::ReplaceString( std::string& srcstring, const std::string& strold, const std::string& strnew )
    {
        std::string::size_type pos = 0;
        auto oldlength = strold.size();
        auto newlength = strnew.size();

        while ( ( pos = srcstring.find( strold, pos ) ) != std::string::npos )
        {
            srcstring.replace( pos, oldlength, strnew );
            pos += newlength;
        }
    }

    void KFUtility::ReplaceString( std::string& srcstring, const std::string& strold, const VectorString& params )
    {
        size_t pos = 0;
        auto oldlength = strold.size();
        for ( auto& strnew : params )
        {
            pos = srcstring.find( strold, pos );
            if ( pos == std::string::npos )
            {
                break;
            }

            srcstring.replace( pos, oldlength, strnew );
            pos += strnew.size();
        }
    }

    bool KFUtility::DelString( std::string& srcstring, const std::string& delstr, std::string& strnew )
    {
        if ( delstr.size() > srcstring.size() )
        {
            return false;
        }
        auto npos = srcstring.find( delstr );
        if ( npos == std::string::npos )
        {
            return false;
        }

        if ( npos + delstr.size() > srcstring.size() )
        {
            return false;
        }

        strnew = srcstring.substr( 0, npos ) + srcstring.substr( npos + delstr.size(), std::string::npos );
        return true;
    }

    // 格式化标题
    std::string KFUtility::FormatTitleText( const std::string& appname, const std::string& apptype, const std::string& strappid )
    {
        return __FORMAT__( "{}.{}.{}", appname, apptype, strappid );
    }

    uint32 KFUtility::GetHashValue( const std::string& data )
    {
        std::hash< std::string > _hash;
        return ( uint32 )_hash.operator()( data );

        //auto md5value = KFCrypto::Md5Encode( data );
        //auto digest = md5value.c_str();

        //uint32 hash = 0;
        //for ( auto i = 0; i < 4; i++ )
        //{
        //    hash += ( ( uint32 )( digest[ i * 4 + 3 ] & 0xFF ) << 24 )
        //            | ( ( uint32 )( digest[ i * 4 + 2 ] & 0xFF ) << 16 )
        //            | ( ( uint32 )( digest[ i * 4 + 1 ] & 0xFF ) << 8 )
        //            | ( ( uint32 )( digest[ i * 4 + 0 ] & 0xFF ) );
        //}

        //return hash;
    }

    std::string KFUtility::FormatConfigFile( const std::string& filename, uint32 channel, uint32 service )
    {
        if ( filename.empty() )
        {
            return _invalid_str;
        }

        auto configfile = __FORMAT__( filename, channel, service );

        // 判断文件是否存在
        Poco::File file( configfile );
        if ( !file.exists() )
        {
            configfile = __FORMAT__( filename, channel, _invalid_int );
            Poco::File file( configfile );
            if ( !file.exists() )
            {
                configfile = __FORMAT__( filename, _invalid_int, _invalid_int );
            }
        }

        return configfile;
    }

    uint32 KFUtility::GetMaxMapValue( MapUInt32& mapvalues, uint32 value )
    {
        auto findvalue = 0u;
        for ( auto& iter : mapvalues )
        {
            if ( value < iter.first )
            {
                break;
            }

            findvalue = iter.second;
        }

        return findvalue;
    }
}