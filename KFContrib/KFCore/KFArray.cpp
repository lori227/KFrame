#include "KFArray.h"

namespace KFrame
{
    void KFArray::Resize( uint32 size )
    {
        _data.Clear();
        _data.Resize( size );
    }

    void KFArray::Reset()
    {
        for ( auto kfdata : _data._objects )
        {
            if ( kfdata != nullptr )
            {
                kfdata->Reset();
            }
        }
    }

    bool KFArray::IsValid()
    {
        for ( auto kfdata : _data._objects )
        {
            if ( kfdata != nullptr )
            {
                if ( kfdata->IsValid() )
                {
                    return true;
                }
            }
        }

        return false;
    }

    void KFArray::InitData()
    {
        if ( _data_setting->_int_init_value != _invalid_int )
        {
            for ( auto kfdata : _data._objects )
            {
                if ( kfdata != nullptr )
                {
                    kfdata->Set( _data_setting->_int_init_value );
                }
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFArray::Size()
    {
        return _data.MaxSize();
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
        for ( auto kfdata : _data._objects )
        {
            auto finddata = kfother->Find( key++ );
            if ( finddata != nullptr )
            {
                kfdata->SaveTo( finddata );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        for ( auto kfdata : _data._objects )
        {
            if ( kfdata != nullptr )
            {
                __JSON_ADD_VALUE__( kfjson, kfdata->Get< int64 >() );
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
