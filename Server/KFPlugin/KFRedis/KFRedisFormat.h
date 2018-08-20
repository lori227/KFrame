#ifndef __KF_REDIS_FORMAT_H__
#define __KF_REDIS_FORMAT_H__

#include "KFDefine.h"

namespace KFrame
{
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
    };

    template<>
    inline std::string KFRedisFormat::ToString( const MapString& value )
    {
        std::stringstream ss;
        for ( auto& iter : value )
        {
            if ( !iter.second.empty() )
            {
                ss << iter.first << " " << iter.second << " ";
            }
        }

        return ss.str();
    }

    template<>
    inline std::string KFRedisFormat::ToString( const VectorString& value )
    {
        std::stringstream ss;
        for ( auto& iter : value )
        {
            ss << iter << " ";
        }

        return ss.str();
    }

    template<>
    inline std::string KFRedisFormat::ToString( const ListString& value )
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