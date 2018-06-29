#include "KFUtility.h"
#include "KFEnum.h"
#include "KFTime/KFDate.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <time.h>
#else
    #include <sys/time.h>
#endif

namespace KFrame
{
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

    char* KFUtility::Format( const char* format, ... )
    {
        static const uint32 _length = 2048;
        static char buff[ _length ];

        va_list args;
        va_start( args, format );
        vsnprintf( buff, _length - 1, format, args );
        va_end( args );

        buff[ _length - 1 ] = 0;
        return buff;
    }

    uint64 KFUtility::Make64Guid( uint32 dataid )
    {
        static uint32 _sequence = 0;

        auto time = KFDate::GetTimeEx();
        time &= 0xFFFFFFFF;		// 32位
        dataid &= 0xFFFFFF;		// 24位
        _sequence = ( _sequence + 1 ) & 0xFF; // 8位

        return ( time << 32 ) | ( dataid << 8 ) | _sequence;
    }
}