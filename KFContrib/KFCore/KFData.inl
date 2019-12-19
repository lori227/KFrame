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
    T KFData::Get( uint64 key )
    {
        auto kfdata = Find( key );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->Get< T >();
    }

    template< class T >
    T KFData::Get( const std::string& dataname )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->Get< T >();
    }

    template< class T >
    T KFData::Get( const std::string& parentname, const std::string& dataname )
    {
        auto kfdata = Find( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->Get< T >();
    }

    template< class T >
    T KFData::Get( uint64 key, const std::string& dataname )
    {
        auto kfdata = Find( key, dataname );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->Get< T >();
    }

    template< class T >
    T KFData::Get( const std::string& dataname, uint64 key )
    {
        auto kfdata = Find( dataname, key );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->Get< T >();
    }

    template< class T >
    T KFData::Get( const std::string& parentname, uint64 key, const std::string& dataname )
    {
        auto kfdata = Find( parentname, key );
        if ( kfdata == nullptr )
        {
            return InvalidValue< T >();
        }

        return kfdata->Get< T >( dataname );
    }

    template< class T >
    T KFData::Set( uint64 key, T value )
    {
        auto kfdata = Find( key );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->Set< T >( value );
    }

    template< class T >
    T KFData::Set( const std::string& dataname, T value )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->Set< T >( value );
    }

    template< class T >
    T KFData::Set( const std::string& parentname, const std::string& dataname, T value )
    {
        auto kfdata = Find( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->Set< T >( value );
    }

    template< class T >
    T KFData::Set( uint64 key, const std::string& dataname, T value )
    {
        auto kfdata = Find( key, dataname );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->Set< T >( value );
    }

    template< class T >
    T KFData::Set( const std::string& dataname, uint64 key, T value )
    {
        auto kfdata = Find( dataname, key );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->Set< T >( value );
    }

    template< class T >
    T KFData::Set( const std::string& parentname, uint64 key, const std::string& dataname, T value )
    {
        auto kfdata = Find( parentname, key );
        if ( kfdata == nullptr )
        {
            return value;
        }

        return kfdata->Set< T >( dataname, value );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template< class T >
    T KFData::Operate( uint32 operate, T value )
    {
        auto base = Get< T >();
        auto final = KFUtility::Operate< T >( operate, base, value );
        final = Set< T >( final );
        return final;
    }

    template< class T >
    T KFData::Operate( const std::string& dataname, uint32 operate, T value )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->Operate< T >( operate, value );
    }

    template< class T >
    T KFData::Operate( const std::string& parentname, const std::string& dataname, uint32 operate, T value )
    {
        auto kfdata = Find( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->Operate< T >( operate, value );
    }

    template< class T >
    T KFData::Operate( uint64 key, const std::string& dataname, uint32 operate, T value )
    {
        auto kfdata = Find( key, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->Operate< T >( operate, value );
    }

    template< class T >
    T KFData::Operate( const std::string& dataname, uint64 key, uint32 operate, T value )
    {
        auto kfdata = Find( dataname, key );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->Operate< T >( operate, value );
    }

    template< class T >
    T KFData::Operate( const std::string& parentname, uint64 key, const std::string& dataname, uint32 operate, T value )
    {
        auto kfdata = Find( parentname, key, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_int;
        }

        return kfdata->Operate< T >( operate, value );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template< class T >
    T KFData::Get()
    {
        switch ( _data_type )
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
    T KFData::Set( T value )
    {
        switch ( _data_type )
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
    template< class T >
    void KFData::GetLists( T& outlist )
    {
        for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
        {
            auto value = kfchild->Get();
            if ( value != 0u )
            {
                outlist.push_back( value );
            }
        }
    }

    template< class T >
    void KFData::GetLists( const std::string& dataname, T& outlist )
    {
        auto kfdata = Find( dataname );
        if ( kfdata != nullptr )
        {
            kfdata->GetLists( outlist );
        }
    }

    template< class T >
    void KFData::GetHashs( T& outlist )
    {
        for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
        {
            auto value = kfchild->Get();
            if ( value != 0u )
            {
                outlist.insert( value );
            }
        }
    }

    template< class T >
    void KFData::GetHashs( const std::string& dataname, T& outlist )
    {
        auto kfdata = Find( dataname );
        if ( kfdata != nullptr )
        {
            kfdata->GetHashs( outlist );
        }
    }
    ///////////////////////////////////////////////////////////////////////////
    template<>
    inline std::string KFData::InvalidValue()
    {
        return _invalid_string;
    }
    ///////////////////////////////////////////////////////////////////////////

    template<>
    inline std::string KFData::Get()
    {
        return GetString();
    }

    template<>
    inline std::string KFData::Set( std::string value )
    {
        FromString( value );
        return value;
    }
    ///////////////////////////////////////////////////////////////////////////
    template<>
    inline Math3D::Vector3D KFData::Get()
    {
        return GetVector3D();
    }

    template<>
    inline Math3D::Vector3D  KFData::Set( Math3D::Vector3D value )
    {
        SetVector3D( value );
        return value;
    }
    ///////////////////////////////////////////////////////////////////////////
}


#endif