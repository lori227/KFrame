#ifndef __KF_DATA_INL__
#define __KF_DATA_INL__

#include "KFData.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////
    template<class T>
    inline T KFData::InvalidValue()
    {
        return 0;
    }

    template<>
    inline std::string KFData::InvalidValue()
    {
        return _invalid_string;
    }
    ///////////////////////////////////////////////////////////////////////////
    template<class T>
    inline T KFData::Get()
    {
        switch ( _data_type )
        {
            case KFDataDefine::DataTypeInt32:
                return static_cast<T>( GetInt32() );
                break;
            case KFDataDefine::DataTypeUInt32:
                return static_cast<T>( GetUInt32() );
                break;
            case KFDataDefine::DataTypeInt64:
                return static_cast<T>( GetInt64() );
                break;
            case KFDataDefine::DataTypeUInt64:
                return static_cast<T>( GetUInt64() );
                break;
            case KFDataDefine::DataTypeDouble:
                return static_cast<T>( GetDouble() );
                break;
            default:
                break;
        }

        return InvalidValue<T>();
    }

    template<>
    inline std::string KFData::Get()
    {
        return GetString();
    }

    template<>
    inline Math3D::Vector3D KFData::Get()
    {
        return GetVector3D();
    }

    template<class T>
    inline T KFData::Get( uint64 key )
    {
        auto data = Find( key );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Get<T>();
    }

    template<class T>
    inline T KFData::Get( const std::string& data_name )
    {
        auto data = Find( data_name );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Get<T>();
    }

    template<class T>
    inline T KFData::Get( const std::string& parent_name, const std::string& data_name )
    {
        auto data = Find( parent_name, data_name );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Get<T>();
    }

    template<class T>
    inline T KFData::Get( uint64 key, const std::string& data_name )
    {
        auto data = Find( key, data_name );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Get<T>();
    }

    template<class T>
    inline T KFData::Get( const std::string& data_name, uint64 key )
    {
        auto data = Find( data_name, key );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Get<T>();
    }

    template<class T>
    inline T KFData::Get( const std::string& parent_name, uint64 key, const std::string& data_name )
    {
        auto data = Find( parent_name, key );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Get<T>( data_name );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class T>
    inline T KFData::Set( T value )
    {
        switch ( _data_type )
        {
            case KFDataDefine::DataTypeInt32:
                return static_cast<T>( SetInt32( static_cast<int32>( value ) ) );
                break;
            case KFDataDefine::DataTypeUInt32:
                return static_cast<T>( SetUInt32( static_cast<uint32>( value ) ) );
                break;
            case KFDataDefine::DataTypeInt64:
                return static_cast<T>( SetInt64( static_cast<int64>( value ) ) );
                break;
            case KFDataDefine::DataTypeUInt64:
                return static_cast<T>( SetUInt64( static_cast<uint64>( value ) ) );
                break;
            case KFDataDefine::DataTypeDouble:
                return static_cast<T>( SetDouble( static_cast<double>( value ) ) );
                break;
            default:
                break;
        }

        return value;
    }

    template<>
    inline std::string KFData::Set( std::string value )
    {
        FromString( value );
        return value;
    }

    template<>
    inline Math3D::Vector3D  KFData::Set( Math3D::Vector3D value )
    {
        SetVector3D( value );
        return value;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class T>
    inline T KFData::Operate( uint32 operate, T value )
    {
        auto base = Get<T>();
        auto final = KFUtility::Operate<T>( base, operate, value );
        final = Set<T>( final );
        return final;
    }

    template<>
    inline std::string KFData::Operate( uint32 operate, std::string value )
    {
        Set<std::string>( value );
        return  value;
    }

    template<class T>
    inline T KFData::Operate( const std::string& data_name, uint32 operate, T value )
    {
        auto data = Find( data_name );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Operate<T>( operate, value );
    }

    template<class T>
    inline T KFData::Operate( const std::string& parent_name, const std::string& data_name, uint32 operate, T value )
    {
        auto data = Find( parent_name, data_name );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Operate<T>( operate, value );
    }

    template<class T>
    inline T KFData::Operate( uint64 key, const std::string& data_name, uint32 operate, T value )
    {
        auto data = Find( key, data_name );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Operate<T>( operate, value );
    }

    template<class T>
    inline T KFData::Operate( const std::string& data_name, uint64 key, uint32 operate, T value )
    {
        auto data = Find( data_name, key );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Operate<T>( operate, value );
    }

    template<class T>
    inline T KFData::Operate( const std::string& parent_name, uint64 key, const std::string& data_name, uint32 operate, T value )
    {
        auto data = Find( parent_name, key, data_name );
        if ( data == nullptr )
        {
            return InvalidValue<T>();
        }

        return data->Operate<T>( operate, value );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


#endif