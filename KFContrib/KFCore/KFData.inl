#ifndef __KF_DATA_INL__
#define __KF_DATA_INL__

#include "KFData.h"

namespace KFrame
{
    template< class T >
    T KFData::InvalidValue()
    {
        return 0;
    }

    template< class T >
    T KFData::GetValue( uint64 key )
    {
        auto kfdata = FindData( key );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->GetValue< T >();
    }

    template< class T >
    T KFData::GetValue( const std::string& dataname )
    {
        auto kfdata = FindData( dataname );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->GetValue< T >();
    }

    template< class T >
    T KFData::GetValue( const std::string& parentname, const std::string& dataname )
    {
        auto kfdata = FindData( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->GetValue< T >();
    }

    template< class T >
    T KFData::GetValue( uint64 key, const std::string& dataname )
    {
        auto kfdata = FindData( key, dataname );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->GetValue< T >();
    }

    template< class T >
    T KFData::GetValue( const std::string& dataname, uint64 key )
    {
        auto kfdata = FindData( dataname, key );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->GetValue< T >();
    }

    template< class T >
    T KFData::GetValue( const std::string& parentname, uint64 key, const std::string& dataname )
    {
        auto kfdata = FindData( parentname, key );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->GetValue< T >( dataname );
    }

    template< class T >
    T KFData::SetValue( uint64 key, T value )
    {
        auto kfdata = FindData( key );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->SetValue< T >( value );
    }

    template< class T >
    T KFData::SetValue( const std::string& dataname, T value )
    {
        auto kfdata = FindData( dataname );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->SetValue< T >( value );
    }

    template< class T >
    T KFData::SetValue( const std::string& parentname, const std::string& dataname, T value )
    {
        auto kfdata = FindData( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->SetValue< T >( value );
    }

    template< class T >
    T KFData::SetValue( uint64 key, const std::string& dataname, T value )
    {
        auto kfdata = FindData( key, dataname );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->SetValue< T >( value );
    }

    template< class T >
    T KFData::SetValue( const std::string& dataname, uint64 key, T value )
    {
        auto kfdata = FindData( dataname, key );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->SetValue< T >( value );
    }

    template< class T >
    T KFData::SetValue( const std::string& parentname, uint64 key, const std::string& dataname, T value )
    {
        auto kfdata = FindData( parentname, key );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->SetValue< T >( dataname, value );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template< class T >
    T KFData::OperateValue( uint32 operate, T value )
    {
        auto base = GetValue< T >();
        auto final = KFUtility::Operate< T >( operate, base, value );
        final = SetValue< T >( final );
        return final;
    }

    template< class T >
    T KFData::OperateValue( const std::string& dataname, uint32 operate, T value )
    {
        auto kfdata = FindData( dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->OperateValue< T >( operate, value );
    }

    template< class T >
    T KFData::OperateValue( const std::string& parentname, const std::string& dataname, uint32 operate, T value )
    {
        auto kfdata = FindData( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->OperateValue< T >( operate, value );
    }

    template< class T >
    T KFData::OperateValue( uint64 key, const std::string& dataname, uint32 operate, T value )
    {
        auto kfdata = FindData( key, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->OperateValue< T >( operate, value );
    }

    template< class T >
    T KFData::OperateValue( const std::string& dataname, uint64 key, uint32 operate, T value )
    {
        auto kfdata = FindData( dataname, key );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->OperateValue< T >( operate, value );
    }

    template< class T >
    T KFData::OperateValue( const std::string& parentname, uint64 key, const std::string& dataname, uint32 operate, T value )
    {
        auto kfdata = FindData( parentname, key, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->OperateValue< T >( operate, value );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template< class T >
    T KFData::GetValue()
    {
        switch ( _data_setting->_logic_type )
        {
        case KFDataDefine::Type_Int32:
            return static_cast<T>( GetInt32() );
            break;
        case KFDataDefine::Type_UInt32:
            return static_cast<T>( GetUInt32() );
            break;
        case KFDataDefine::Type_Int64:
            return static_cast<T>( GetInt64() );
            break;
        case KFDataDefine::Type_UInt64:
            return static_cast<T>( GetUInt64() );
            break;
        case KFDataDefine::Type_Double:
            return static_cast<T>( GetDouble() );
            break;
        default:
            break;
        }

        return _invalid_int;
    }

    template< class T >
    T KFData::SetValue( T value )
    {
        switch ( _data_setting->_logic_type )
        {
        case KFDataDefine::Type_Int32:
            return static_cast< T >( SetInt32( static_cast<int32>( value ) ) );
            break;
        case KFDataDefine::Type_UInt32:
            return static_cast< T >( SetUInt32( static_cast<uint32>( value ) ) );
            break;
        case KFDataDefine::Type_Int64:
            return static_cast< T >( SetInt64( static_cast<int64>( value ) ) );
            break;
        case KFDataDefine::Type_UInt64:
            return static_cast< T >( SetUInt64( static_cast<uint64>( value ) ) );
            break;
        case KFDataDefine::Type_Double:
            return static_cast< T >( SetDouble( static_cast<double>( value ) ) );
            break;
        default:
            break;
        }

        return value;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<>
    inline std::string KFData::InvalidValue()
    {
        return _invalid_str;
    }
    ///////////////////////////////////////////////////////////////////////////

    template<>
    inline std::string KFData::GetValue()
    {
        return GetString();
    }

    template<>
    inline std::string KFData::SetValue( std::string value )
    {
        FromString( value );
        return value;
    }
    ///////////////////////////////////////////////////////////////////////////
    template<>
    inline Math3D::Vector3D KFData::GetValue()
    {
        return GetVector3D();
    }

    template<>
    inline Math3D::Vector3D  KFData::SetValue( Math3D::Vector3D value )
    {
        SetVector3D( value );
        return value;
    }
    ///////////////////////////////////////////////////////////////////////////
}


#endif