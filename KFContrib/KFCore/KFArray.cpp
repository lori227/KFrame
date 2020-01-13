#include "KFArray.h"

namespace KFrame
{
    KFArray::KFArray()
    {
        _data_type = KFDataDefine::Type_Array;
    }

    void KFArray::Resize( uint32 size )
    {
        _data.Clear();
        _data.Resize( size );
    }

    void KFArray::Reset( bool isdelete /* = true */ )
    {
        for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
        {
            kfchild->Reset( isdelete );
        }

        KFData::Reset( isdelete );
    }

    bool KFArray::IsValid()
    {
        for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
        {
            if ( kfchild->IsValid() )
            {
                return true;
            }
        }

        return false;
    }

    void KFArray::InitData()
    {
        if ( _data_setting->_int_init_value != _invalid_int )
        {
            for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
            {
                kfchild->Set( _data_setting->_int_init_value );
            }
        }
    }

    bool KFArray::IsFull()
    {
        for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
        {
            auto value = kfchild->Get();
            if ( value == 0u )
            {
                return false;
            }
        }

        return true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFArray::MaxSize()
    {
        return _data.MaxSize();
    }

    uint32 KFArray::Size()
    {
        auto count = 0u;
        for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
        {
            auto value = kfchild->Get();
            if ( value != 0u )
            {
                ++count;
            }
        }

        return count;
    }

    KFData* KFArray::First()
    {
        uint32 index = KFDataDefine::Array_Index;
        if ( index == 0u )
        {
            return _data.First();
        }

        _data.First();
        return _data.Next();
    }

    KFData* KFArray::Next()
    {
        return _data.Next();
    }

    void KFArray::CopyFrom( KFData* kfother )
    {
        kfother->SaveTo( this );
    }

    void KFArray::SaveTo( KFData* kfother )
    {
        uint32 key = KFDataDefine::Array_Index;
        for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
        {
            auto finddata = kfother->Find( key++ );
            if ( finddata != nullptr )
            {
                kfchild->SaveTo( finddata );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFArray::GetEmpty()
    {
        auto maxsize = _data.MaxSize();
        for ( uint32 i = KFDataDefine::Array_Index; i < maxsize; ++i )
        {
            auto kfdata = Find( i );
            if ( kfdata != nullptr && kfdata->Get() == 0u )
            {
                return i;
            }
        }

        return maxsize;
    }

    uint32 KFArray::GetIndex( uint64 value )
    {
        auto maxsize = _data.MaxSize();
        for ( uint32 i = KFDataDefine::Array_Index; i < maxsize; ++i )
        {
            auto kfdata = Find( i );
            if ( kfdata != nullptr && kfdata->Get() == value )
            {
                return i;
            }
        }

        return 0u;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFArray::Find( uint64 key )
    {
        return _data.Find( static_cast<uint32>( key ) );
    }

    KFData* KFArray::Find( uint64 key, const std::string& dataname )
    {
        auto kfdata = Find( key );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( dataname );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFArray::Insert( uint64 value )
    {
        for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
        {
            if ( kfchild->Get() == 0u )
            {
                kfchild->Set( value );
                return kfchild;
            }
        }

        return nullptr;
    }

    bool KFArray::Add( uint64 key, KFData* data )
    {
        data->SetParent( this );
        _data.Insert( static_cast<uint32>( key ), data );
        return true;
    }

    bool KFArray::Add( uint64 key, const std::string& dataname, KFData* data )
    {
        auto kfdata = Find( key );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return kfdata->Add( dataname, data );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFArray::Remove( uint64 key )
    {
        auto kfdata = Find( key );
        if ( kfdata == nullptr )
        {
            return false;
        }

        kfdata->Reset();
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string KFArray::ToString()
    {
        __JSON_ARRAY_DOCUMENT__( kfjson );
        for ( auto kfchild = First(); kfchild != nullptr; kfchild = Next() )
        {
            if ( kfchild != nullptr )
            {
                __JSON_ADD_VALUE__( kfjson, kfchild->Get< int64 >() );
            }
        }

        return __JSON_SERIALIZE__( kfjson ) ;
    }

    void KFArray::FromString( const std::string& value )
    {
        __JSON_PARSE_STRING__( kfjson, value );

        auto size = __JSON_ARRAY_SIZE__( kfjson );
        for ( uint32 i = 0u; i < size; ++i )
        {
            auto kfdata = Find( i + KFDataDefine::Array_Index );
            if ( kfdata != nullptr )
            {
                auto& object = __JSON_ARRAY_INDEX__( kfjson, i );
                kfdata->Set( object.GetInt64() );
            }
        }
    }
}
