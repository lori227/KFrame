#include "KFObject.h"

namespace KFrame
{
    KFObject::KFObject()
    {
        _data_type = KFDataDefine::Type_Object;
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
        Set<uint64>( _data_setting->_key_name, id );
    }

    void KFObject::Reset()
    {
        _key = _invalid_int;
        for ( auto& iter : _data._objects )
        {
            iter.second->Reset();
        }
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

    KFData* KFObject::First()
    {
        return _data.First();
    }

    KFData* KFObject::Next()
    {
        return _data.Next();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void KFObject::CopyFrom( KFData* kfother )
    {
        SetKeyID( kfother->GetKeyID() );

        for ( auto& iter : _data._objects )
        {
            auto& key = iter.first;
            auto kfdata = iter.second;

            auto finddata = kfother->Find( key );
            if ( finddata != nullptr )
            {
                kfdata->CopyFrom( finddata );
            }
        }
    }

    void KFObject::SaveTo( KFData* kfother )
    {
        kfother->CopyFrom( this );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFObject::Find( const std::string& dataname )
    {
        return _data.Find( dataname );
    }

    KFData* KFObject::Find( const std::string& dataname, uint64 key )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( key );
    }

    KFData* KFObject::Find( const std::string& parentname, const std::string& childname )
    {
        if ( parentname.empty() )
        {
            return Find( childname );
        }

        auto kfdata = Find( parentname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( childname );
    }

    KFData* KFObject::Find( const std::string& parentname, uint64 key, const std::string& childname )
    {
        auto kfdata = Find( parentname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( key, childname );
    }

    KFData* KFObject::Find( const std::string& parentname, const std::string& childname, uint64 key )
    {
        auto kfdata = Find( parentname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( childname, key );
    }

    KFData* KFObject::Find( const std::string& parentname, const std::string& childname, const std::string& dataname )
    {
        auto kfdata = Find( parentname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( childname, dataname );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFObject::Add( const std::string& dataname, KFData* data )
    {
        data->SetParent( this );
        _data.Insert( dataname, data );

        auto datasetting = _class_setting->FindSetting( dataname );
        if ( datasetting != nullptr )
        {
            data->_data_setting = datasetting;
        }

        return true;
    }

    bool KFObject::Add( const std::string& dataname, uint64 key, KFData* data )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return kfdata->Add( key, data );
    }


    bool KFObject::Add( const std::string& parentname, const std::string& childname, KFData* data )
    {
        auto kfdata = Find( parentname );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return kfdata->Add( childname, data );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFObject::Move( const std::string& dataname, bool usekey )
    {
        auto kfdata = _data.Find( dataname );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        if ( usekey )
        {
            auto key = kfdata->GetKeyID();
            if ( key == 0u )
            {
                return nullptr;
            }
        }

        _data.Remove( dataname, false );
        return kfdata;
    }

    bool KFObject::Remove( const std::string& dataname )
    {
        return _data.Remove( dataname );
    }

    bool KFObject::Remove( const std::string& dataname, uint64 key )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return kfdata->Remove( key );
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
            auto kfdata = iter.second;
            if ( kfdata->IsValid() )
            {
                values[ key ] = kfdata->ToString();
            }
        }
    }

    void KFObject::FromMap( const StringMap& values )
    {
        for ( auto& iter : values )
        {
            auto kfdata = Find( iter.first );
            if ( kfdata == nullptr )
            {
                continue;
            }

            kfdata->FromMap( values );
        }
    }
}