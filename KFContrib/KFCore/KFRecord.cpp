#include "KFRecord.hpp"

namespace KFrame
{
    KFRecord::KFRecord()
    {
        _data_type = KFDataDefine::DataTypeRecord;
    }

    void KFRecord::Reset()
    {
        _data.Clear();
        KFData::Reset();
    }

    bool KFRecord::IsValid()
    {
        return _data.Size() != 0;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFRecord::Size()
    {
        return _data.Size();
    }

    uint32 KFRecord::MaxSize()
    {
        return _data_setting->_int_max_capacity;
    }

    bool KFRecord::IsFull()
    {
        return _data.Size() >= _data_setting->_int_max_capacity;
    }

    DataPtr KFRecord::First()
    {
        return _data.First();
    }

    DataPtr KFRecord::Next()
    {
        return _data.Next();
    }

    void KFRecord::CopyFrom( DataPtr& other )
    {
        __LOG_ERROR__( "record can't copy another" );
        //auto kfchild = kfother->First();
        //while ( kfchild != nullptr )
        //{
        //    auto kfdata = KFDataFactory::CreateData( kfchild->_data_setting );
        //    if ( kfdata != nullptr )
        //    {
        //        kfdata->CopyFrom( kfchild );
        //        Add( kfdata->GetKeyID(), kfdata );
        //    }

        //    kfchild = kfother->Next();
        //}
    }

    void KFRecord::SaveTo( DataPtr& other )
    {
        //other->CopyFrom( shared_from_this() );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFRecord::Find( uint64 key )
    {
        return _data.Find( key );
    }

    DataPtr KFRecord::Find( uint64 key, const std::string& data_name )
    {
        auto data = Find( key );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( data_name );
    }

    DataPtr KFRecord::Find( uint64 parent_key, uint64 child_key )
    {
        auto data = Find( parent_key );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( child_key );
    }

    DataPtr KFRecord::Find( uint64 parent_key, uint64 child_key, const std::string& data_name )
    {
        auto data = Find( parent_key );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( child_key, data_name );
    }

    DataPtr KFRecord::Find( uint64 parent_key, const std::string& data_name, uint64 child_key )
    {
        auto data = Find( parent_key );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( data_name, child_key );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRecord::Add( uint64 key, DataPtr& data )
    {
        data->SetKeyID( key );
        data->SetParent( this );
        data->_data_setting = _data_setting;

        _data.Insert( key, data );
        return true;
    }

    bool KFRecord::Add( uint64 parent_key, uint64 child_key, DataPtr& data )
    {
        auto parent_data = Find( parent_key );
        if ( parent_data == nullptr )
        {
            return false;
        }

        return parent_data->Add( child_key, data );
    }

    bool KFRecord::Add( uint64 key, const std::string& data_name, DataPtr& data )
    {
        auto parent_data = Find( key );
        if ( parent_data == nullptr )
        {
            return false;
        }

        return parent_data->Add( data_name, data );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFRecord::Remove( uint64 key )
    {
        auto data = _data.Remove( key );
        if ( data != nullptr )
        {
            data->SetParent( nullptr );
        }

        return data;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRecord::Find( const std::string& data_name, uint64 value, std::list<DataPtr>& find_list, bool find_all )
    {
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            auto data_value = child->Get<uint64>( data_name );
            if ( data_value == value )
            {
                find_list.push_back( child );
                if ( !find_all )
                {
                    break;
                }
            }
        }
    }

    bool KFRecord::Check( const std::string& data_name, uint64 value, const std::string& check_name, uint64 check_value )
    {
        uint64 total_value = 0;
        for ( auto child = First(); child != nullptr; child = Next() )
        {
            auto data_value = child->Get<uint64>( data_name );
            if ( data_value == value )
            {
                total_value += child->Get( check_name );
                if ( total_value >= check_value )
                {
                    return true;
                }
            }
        }

        return false;
    }


    std::string KFRecord::ToString()
    {
        StringMap values;
        ToMap( values );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_FROM_MAP__( kfjson, values );
        return __JSON_SERIALIZE__( kfjson );
    }

    void KFRecord::FromString( const std::string& value )
    {
        __JSON_PARSE_STRING__( kfjson, value );

        StringMap values;
        __JSON_TO_MAP__( kfjson, values );
        FromMap( values );
    }

    void KFRecord::ToMap( StringMap& values )
    {
        for ( auto& iter : _data._objects )
        {
            auto key = __TO_STRING__( iter.first );
            values[ key ] = iter.second->ToString();
        }
    }

    void KFRecord::FromMap( const StringMap& values )
    {
        __LOG_ERROR__( "record can't copy from map" );

        //for ( auto iter : values )
        //{
        //    auto kfdata = KFDataFactory::CreateData( _data_setting );
        //    if ( kfdata == nullptr )
        //    {
        //        continue;
        //    }

        //    auto key = __TO_UINT64__( iter.first );
        //    kfdata->SetKeyID( key );
        //    kfdata->FromString( iter.second );
        //    Add( key, kfdata );
        //}
    }
}