#include "KFUtility.h"
#include "KFMacros.h"
#include "Poco/File.h"
#include "KFGlobal/KFGlobal.h"

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

    bool KFUtility::IsFloatEqual( float first, float second )
    {
        return std::abs( first - second ) <= 1e-6;
    }

    bool KFUtility::IsDoubleEqual( double first, double second )
    {
        return std::abs( first - second ) <= 1e-15;
    }

    uint32 KFUtility::CalcDistance( uint32 now_x, uint32 now_y, uint32 target_x, uint32 target_y )
    {
        auto x = __MAX__( now_x, target_x ) - __MIN__( now_x, target_x );
        auto y = __MAX__( now_y, target_y ) - __MIN__( now_y, target_y );

        auto length = sqrt( x * x + y * y );
        return static_cast<uint32>( length );
    }

    bool KFUtility::CheckInDistance( uint32 now_x, uint32 now_y, uint32 target_x, uint32 target_y, uint32 distance )
    {
        auto length = CalcDistance( now_x, now_y, target_x, target_y );
        return length <= distance;
    }

    std::string KFUtility::SplitString( std::string& source, const std::string& split )
    {
        if ( source.empty() )
        {
            return "";
        }

        std::string result = "";
        auto pos = source.find( split );
        if ( pos == std::string::npos )
        {
            result = source;
            source.clear();
            return result;
        }

        result = source.substr( 0, pos );
        source = source.substr( pos + split.size() );
        return result;
    }

    void KFUtility::ReplaceString( std::string& source, const std::string& str_old, const std::string& str_new )
    {
        std::string::size_type pos = 0;
        auto old_length = str_old.size();
        auto new_length = str_new.size();

        while ( ( pos = source.find( str_old, pos ) ) != std::string::npos )
        {
            source.replace( pos, old_length, str_new );
            pos += new_length;
        }
    }

    void KFUtility::ReplaceString( std::string& source, const std::string& str_old, const StringVector& params )
    {
        size_t pos = 0;
        auto old_length = str_old.size();
        for ( auto& str_new : params )
        {
            pos = source.find( str_old, pos );
            if ( pos == std::string::npos )
            {
                break;
            }

            source.replace( pos, old_length, str_new );
            pos += str_new.size();
        }
    }

    bool KFUtility::DelString( std::string& source, const std::string& str_del, std::string& str_new )
    {
        if ( str_del.size() > source.size() )
        {
            return false;
        }
        auto pos = source.find( str_del );
        if ( pos == std::string::npos )
        {
            return false;
        }

        if ( pos + str_del.size() > source.size() )
        {
            return false;
        }

        str_new = source.substr( 0, pos ) + source.substr( pos + str_del.size(), std::string::npos );
        return true;
    }

    // 格式化标题
    std::string KFUtility::FormatTitleText( const std::string& app_name, const std::string& app_type, const std::string& app_id )
    {
        return __FORMAT__( "{}.{}.{}", app_name, app_type, app_id );
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

    std::string KFUtility::FormatConfigFile( const std::string& file_name, uint32 channel, uint32 service )
    {
        if ( file_name.empty() )
        {
            return _invalid_string;
        }

        auto configfile = __FORMAT__( file_name, channel, service );

        // 判断文件是否存在
        Poco::File file( configfile );
        if ( !file.exists() )
        {
            configfile = __FORMAT__( file_name, channel, _invalid_int );
            Poco::File file( configfile );
            if ( !file.exists() )
            {
                configfile = __FORMAT__( file_name, _invalid_int, _invalid_int );
            }
        }

        return configfile;
    }

    uint32 KFUtility::GetMaxMapValue( const UInt32Map& map_values, uint32 value )
    {
        auto find_value = 0u;
        for ( auto& iter : map_values )
        {
            if ( value < iter.first )
            {
                break;
            }

            find_value = iter.second;
        }

        return find_value;
    }

    uint32 KFUtility::RandMapValue( const UInt32Map& map_values, uint32 total_weight, uint32 rand )
    {
        if ( total_weight > 0u )
        {
            auto value = 0u;
            rand = rand % total_weight;
            for ( auto& iter : map_values )
            {
                value += iter.second;
                if ( rand < value )
                {
                    return iter.first;
                }
            }
        }

        return 0u;
    }

    uint32 KFUtility::RandVectorValue( const UInt32Vector& vector_values, const UInt32Set& exclude_list )
    {
        UInt32Vector temp_values;
        if ( exclude_list.empty() || exclude_list.size() >= vector_values.size() )
        {
            temp_values = vector_values;
        }
        else
        {
            for ( auto value : vector_values )
            {
                if ( exclude_list.find( value ) == exclude_list.end() )
                {
                    temp_values.push_back( value );
                }
            }
        }

        auto size = ( uint32 )temp_values.size();
        if ( size == 0u )
        {
            return 0u;
        }

        auto index = KFGlobal::Instance()->RandRatio( size );
        return temp_values.at( index );
    }
}