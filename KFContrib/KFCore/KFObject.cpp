#include "KFObject.h"

namespace KFrame
{
    KFObject::KFObject()
    {
        _data_type = KFDataDefine::DataTypeObject;
    }

    uint32 KFObject::GetConfigID()
    {
        return Get<uint32>( _data_setting->_config_key_name );
    }

    uint64 KFObject::GetKeyID()
    {
        if ( _key == 0u )
        {
            _key = Get( _data_setting->_key_name );
        }

        return _key;
    }

    void KFObject::SetKeyID( uint64 id )
    {
        _key = id;
        Operate<uint64>( _data_setting->_key_name, KFEnum::Set, id );
    }

    void KFObject::Reset()
    {
        _key = _invalid_int;
        for ( auto& iter : _data._objects )
        {
            iter.second->Reset();
        }
        KFData::Reset();
    }

    void KFObject::InitData()
    {
        for ( auto& iter : _data._objects )
        {
            iter.second->InitData();
        }
    }

    bool KFObject::IsValid()
    {
        return true;
    }

    uint32 KFObject::Size()
    {
        return _data.Size();
    }

    DataPtr KFObject::First()
    {
        return _data.First();
    }

    DataPtr KFObject::Next()
    {
        return _data.Next();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void KFObject::CopyFrom( DataPtr& other )
    {
        SetKeyID( other->GetKeyID() );

        for ( auto& iter : _data._objects )
        {
            auto& key = iter.first;
            auto data = iter.second;

            auto find_data = other->Find( key );
            if ( find_data != nullptr )
            {
                data->CopyFrom( find_data );
            }
        }
    }

    void KFObject::SaveTo( DataPtr& other )
    {
        other->SetKeyID( _key );
        for ( auto& iter : _data._objects )
        {
            auto& key = iter.first;
            auto data = iter.second;

            auto find_data = other->Find( key );
            if ( find_data != nullptr )
            {
                data->SaveTo( find_data );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFObject::Find( const std::string& data_name )
    {
        return _data.Find( data_name );
    }

    DataPtr KFObject::Find( const std::string& data_name, uint64 key )
    {
        auto data = Find( data_name );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( key );
    }

    DataPtr KFObject::Find( const std::string& parent_name, const std::string& child_name )
    {
        if ( parent_name.empty() )
        {
            return Find( child_name );
        }

        auto data = Find( parent_name );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( child_name );
    }

    DataPtr KFObject::Find( const std::string& parent_name, uint64 key, const std::string& child_name )
    {
        auto data = Find( parent_name );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( key, child_name );
    }

    DataPtr KFObject::Find( const std::string& parent_name, const std::string& child_name, uint64 key )
    {
        auto data = Find( parent_name );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( child_name, key );
    }

    DataPtr KFObject::Find( const std::string& parent_name, const std::string& child_name, const std::string& data_name )
    {
        auto data = Find( parent_name );
        if ( data == nullptr )
        {
            return nullptr;
        }

        return data->Find( child_name, data_name );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFObject::Add( const std::string& data_name, DataPtr& data )
    {
        data->SetParent( this );
        _data.Insert( data_name, data );

        auto data_setting = _data_setting->_class_setting->FindSetting( data_name );
        if ( data_setting != nullptr )
        {
            data->_data_setting = data_setting;
        }

        return true;
    }

    bool KFObject::Add( const std::string& data_name, uint64 key, DataPtr& data )
    {
        auto parent_data = Find( data_name );
        if ( parent_data == nullptr )
        {
            return false;
        }

        return parent_data->Add( key, data );
    }

    bool KFObject::Add( const std::string& parent_name, const std::string& child_name, DataPtr& data )
    {
        auto parent_data = Find( parent_name );
        if ( parent_data == nullptr )
        {
            return false;
        }

        return parent_data->Add( child_name, data );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DataPtr KFObject::Remove( const std::string& data_name )
    {
        return _data.Remove( data_name );
    }

    DataPtr KFObject::Remove( const std::string& data_name, uint64 key )
    {
        auto parent_data = Find( data_name );
        if ( parent_data == nullptr )
        {
            return nullptr;
        }

        return parent_data->Remove( key );
    }

    std::string KFObject::ToString()
    {
        StringMap values;
        ToMap( values );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_FROM_MAP__( kfjson, values );
        return __JSON_SERIALIZE__( kfjson );
    }

    void KFObject::FromString( const std::string& value )
    {
        __JSON_PARSE_STRING__( kfjson, value );

        StringMap values;
        __JSON_TO_MAP__( kfjson, values );

        FromMap( values );
    }

    void KFObject::ToMap( StringMap& values )
    {
        for ( auto& iter : _data._objects )
        {
            auto& key = iter.first;
            auto data = iter.second;
            if ( data->IsValid() )
            {
                values[ key ] = data->ToString();
            }
        }
    }

    void KFObject::FromMap( const StringMap& values )
    {
        for ( auto& iter : values )
        {
            auto data = Find( iter.first );
            if ( data != nullptr )
            {
                data->FromMap( values );
            }
        }
    }
}