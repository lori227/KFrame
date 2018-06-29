#include "KFRedisFormat.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////

    std::string KFRedisFormat::ToString()
    {
        std::ostringstream oss;
        for ( auto iter = _params.begin(); iter != _params.end(); ++iter )
        {
            oss << *iter << " ";
        }

        return oss.str();
    }

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    void KFRedisFormat::Append( const std::string& param )
    {
        _params.push_back( param );
    }

    void KFRedisFormat::Append( const std::string& command, const std::string& key )
    {
        Append( command );
        Append( key );
    }

    void KFRedisFormat::Append( const std::string& command, const std::string& key, const std::string& value )
    {
        Append( command, key );
        Append( value );
    }

    void KFRedisFormat::Append( const std::string& command, const std::string& key, const std::string& field, const std::string& value )
    {
        Append( command, key, field );
        Append( value );
    }

    void KFRedisFormat::Append( const VectorString& values )
    {
        for ( auto iter = values.begin(); iter != values.end(); ++iter )
        {
            auto& value = *iter;
            if ( value.empty() )
            {
                continue;
            }

            Append( value );
        }
    }

    void KFRedisFormat::Append( const std::string& command, const VectorString& values )
    {
        Append( command );
        Append( values );
    }

    void KFRedisFormat::Append( const std::string& command, const std::string& key, const VectorString& values )
    {
        Append( command, key );
        Append( values );
    }

    void KFRedisFormat::Append( const MapString& values )
    {
        for ( auto iter = values.begin(); iter != values.end(); ++iter )
        {
            auto& key = iter->first;
            auto& value = iter->second;

            if ( key.empty() || value.empty() )
            {
                continue;
            }

            Append( key );
            Append( value );
        }
    }

    void KFRedisFormat::Append( const std::string& command, const MapString& values )
    {
        Append( command );
        Append( values );
    }

    void KFRedisFormat::Append( const std::string& command, const std::string& key, const MapString& values )
    {
        Append( command, key );
        Append( values );
    }
}

