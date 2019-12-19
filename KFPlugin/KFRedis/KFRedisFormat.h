#ifndef __KF_FORMAT_H__
#define __KF_FORMAT_H__

#include "KFDefine.h"

namespace KFrame
{
    static std::string _redis_empty_string = "nil";
    class KFRedisFormat
    {
    public:
        template< typename T >
        inline static std::string ToString( const T& value )
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline static std::string& EmptyString()
        {
            return _redis_empty_string;
        }

        inline static bool IsEmptyString( const char* strvalue )
        {
            if ( strvalue == nullptr || strvalue[ 0 ] == 0 )
            {
                return true;
            }

            return _redis_empty_string.compare( 0, _redis_empty_string.size(), strvalue ) == 0;
        }
    };

    template<>
    inline std::string KFRedisFormat::ToString( const StringMap& value )
    {
        std::stringstream ss;
        for ( auto& iter : value )
        {
            if ( iter.second.empty() )
            {
                ss << iter.first << " " << _redis_empty_string << " ";
            }
            else
            {
                ss << iter.first << " " << iter.second << " ";
            }
        }

        return ss.str();
    }

    template<>
    inline std::string KFRedisFormat::ToString( const StringVector& value )
    {
        std::stringstream ss;
        for ( auto& iter : value )
        {
            ss << iter << " ";
        }

        return ss.str();
    }

    template<>
    inline std::string KFRedisFormat::ToString( const StringList& value )
    {
        std::stringstream ss;
        for ( auto& iter : value )
        {
            ss << iter << " ";
        }

        return ss.str();
    }


}
#endif