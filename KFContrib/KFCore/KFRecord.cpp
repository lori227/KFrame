#include "KFRecord.h"

namespace KFrame
{
    KFRecord::KFRecord()
    {
        _data_type = KFDataDefine::Type_Record;
    }

    void KFRecord::Reset()
    {
        _data.Clear();
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

    bool KFRecord::IsFull()
    {
        return _data.Size() >= _data_setting->_int_max_value;
    }

    KFData* KFRecord::First()
    {
        return _data.First();
    }

    KFData* KFRecord::Next()
    {
        return _data.Next();
    }

    void KFRecord::CopyFrom( KFData* kfother )
    {
        __LOG_ERROR__( "record can't copy anohter!" );
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

    void KFRecord::SaveTo( KFData* kfother )
    {
        kfother->CopyFrom( this );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFRecord::Find( uint64 key )
    {
        return _data.Find( key );
    }

    KFData* KFRecord::Find( uint64 key, const std::string& dataname )
    {
        auto kfdata = Find( key );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( dataname );
    }

    KFData* KFRecord::Find( uint64 parentkey, uint64 childkey )
    {
        auto kfdata = Find( parentkey );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( childkey );
    }

    KFData* KFRecord::Find( uint64 parentkey, uint64 childkey, const std::string& dataname )
    {
        auto kfdata = Find( parentkey );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( childkey, dataname );
    }

    KFData* KFRecord::Find( uint64 parentkey, const std::string& dataname, uint64 childkey )
    {
        auto kfdata = Find( parentkey );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->Find( dataname, childkey );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRecord::Add( uint64 key, KFData* data )
    {
        data->SetKeyID( key );
        data->SetParent( this );
        data->_data_setting = _data_setting;

        _data.Insert( key, data );
        return true;
    }

    bool KFRecord::Add( uint64 parentkey, uint64 childkey, KFData* data )
    {
        auto kfdata = Find( parentkey );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return kfdata->Add( childkey, data );
    }

    bool KFRecord::Add( uint64 key, const std::string& dataname, KFData* data )
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
    KFData* KFRecord::Move( uint64 key )
    {
        auto kfdata = _data.Find( key );
        if ( kfdata != nullptr )
        {
            _data.Remove( key, false );
        }

        return kfdata;
    }

    bool KFRecord::Remove( uint64 key )
    {
        return _data.Remove( key );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRecord::Find( const std::string& dataname, uint64 value, std::list<KFData*>& findlist, bool findall )
    {
        auto child = First();
        while ( child != nullptr )
        {
            auto datavalue = child->Get( dataname );
            if ( datavalue == value )
            {
                findlist.push_back( child );
                if ( !findall )
                {
                    break;
                }
            }

            child = Next();
        }
    }

    bool KFRecord::Check( const std::string& dataname, uint64 value, const std::string& checkname, uint64 checkvalue )
    {
        uint64 totalvalue = 0;

        auto child = First();
        while ( child != nullptr )
        {
            auto datavalue = child->Get( dataname );
            if ( datavalue == value )
            {
                totalvalue += child->Get( checkname );
                if ( totalvalue >= checkvalue )
                {
                    return true;
                }
            }

            child = Next();
        }

        return false;
    }


    std::string KFRecord::ToString()
    {
        MapString values;
        ToMap( values );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_FROM_MAP__( kfjson, values );
        return __JSON_SERIALIZE__( kfjson );
    }

    void KFRecord::FromString( const std::string& value )
    {
        __JSON_PARSE_STRING__( kfjson, value );

        MapString values;
        __JSON_TO_MAP__( kfjson, values );
        FromMap( values );
    }

    void KFRecord::ToMap( MapString& values )
    {
        for ( auto& iter : _data._objects )
        {
            auto key = __TO_STRING__( iter.first );
            auto kfdata = iter.second;

            values[ key ] = kfdata->ToString();
        }
    }

    void KFRecord::FromMap( const MapString& values )
    {
        __LOG_ERROR__( "record can't copy from map!" );

        //for ( auto iter : values )
        //{
        //    auto kfdata = KFDataFactory::CreateData( _data_setting );
        //    if ( kfdata == nullptr )
        //    {
        //        continue;
        //    }

        //    auto key = KFUtility::ToValue< uint64 >( iter.first );
        //    kfdata->SetKeyID( key );
        //    kfdata->FromString( iter.second );
        //    Add( key, kfdata );
        //}
    }
}