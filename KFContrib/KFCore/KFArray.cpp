#include "KFArray.hpp"

namespace KFrame
{
    KFArray::KFArray()
    {
        _data_type = KFDataDefine::DataTypeArray;
    }

    void KFArray::Resize( uint32 size )
    {
        _data.Clear();
        _data.Resize( size );
    }

    void KFArray::Reset()
    {
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            child->Reset();
        }

        KFData::Reset();
    }

    bool KFArray::IsValid()
    {
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            if ( child->IsValid() )
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
            for ( auto child = First(); child != nullptr; child = Next() )
            {
                child->Set( _data_setting->_int_init_value );
            }
        }
    }

    bool KFArray::IsFull()
    {
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            auto value = child->Get();
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
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            auto value = child->Get();
            if ( value != 0u )
            {
                ++count;
            }
        }

        return count;
    }

    DataPtr KFArray::First()
    {
        auto data = _data.First();
        if ( KFGlobal::Instance()->_array_index == 1u )
        {
            data = _data.Next();
        }

        return data;
    }

    DataPtr KFArray::Next()
    {
        return _data.Next();
    }

    void KFArray::CopyFrom( DataPtr& other )
    {
        uint32 key = KFGlobal::Instance()->_array_index;
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            auto find_data = child->Find( key++ );
            if ( find_data != nullptr )
            {
                child->CopyFrom( find_data );
            }
        }
    }

    void KFArray::SaveTo( DataPtr& other )
    {
        uint32 key = KFGlobal::Instance()->_array_index;
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            auto find_data = child->Find( key++ );
            if ( find_data != nullptr )
            {
                child->SaveTo( find_data );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFArray::GetEmpty()
    {
        auto max_size = _data.MaxSize();
        for ( uint32 i = KFGlobal::Instance()->_array_index; i < max_size; ++i )
        {
            auto data = Find( i );
            if ( data != nullptr && data->Get() == 0u )
            {
                return i;
            }
        }

        return max_size;
    }

    uint32 KFArray::GetIndex( uint64 value )
    {
        auto max_size = _data.MaxSize();
        for ( uint32 i = KFGlobal::Instance()->_array_index; i < max_size; ++i )
        {
            auto data = Find( i );
            if ( data != nullptr && data->Get() == value )
            {
                return i;
            }
        }

        return 0u;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFArray::Find( uint64 key )
    {
        return _data.Find( static_cast<uint32>( key ) );
    }

    DataPtr KFArray::Find( uint64 key, const std::string& data_name )
    {
        auto data = Find( key );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( data_name );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFArray::Insert( uint64 value )
    {
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            if ( child->Get() == 0u )
            {
                child->Set( value );
                return child;
            }
        }

        return nullptr;
    }

    bool KFArray::Add( uint64 key, DataPtr& data )
    {
        data->SetParent( this );
        _data.Insert( static_cast<uint32>( key ), data );
        return true;
    }

    bool KFArray::Add( uint64 key, const std::string& data_name, DataPtr& data )
    {
        auto child = Find( key );
        if ( child == nullptr )
        {
            return false;
        }

        return child->Add( data_name, data );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFArray::Remove( uint64 key )
    {
        auto data = Find( key );
        if ( data == nullptr )
        {
            return nullptr;
        }

        data->Reset();
        return nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string KFArray::ToString()
    {
        __JSON_ARRAY_DOCUMENT__( kfjson );
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            if ( child != nullptr )
            {
                __JSON_ADD_VALUE__( kfjson, child->Get< int64 >() );
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
            auto data = Find( i + KFGlobal::Instance()->_array_index );
            if ( data != nullptr )
            {
                auto& object = __JSON_ARRAY_INDEX__( kfjson, i );
                data->Set( object.GetInt64() );
            }
        }
    }
}
