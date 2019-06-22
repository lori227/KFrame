#include "KFRecord.h"

namespace KFrame
{
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
        if ( _data_setting->_max_value != 0u )
        {
            if ( Size() >= _data_setting->_max_value )
            {
                return true;
            }
        }

        return false;
    }

    KFData* KFRecord::FirstData()
    {
        return _data.First();
    }

    KFData* KFRecord::NextData()
    {
        return _data.Next();
    }

    void KFRecord::CopyFrom( KFData* kfother )
    {
        __LOG_ERROR__( "record can't copy anohter!" );
        //auto kfchild = kfother->FirstData();
        //while ( kfchild != nullptr )
        //{
        //    auto kfdata = KFDataFactory::CreateData( kfchild->_data_setting );
        //    if ( kfdata != nullptr )
        //    {
        //        kfdata->CopyFrom( kfchild );
        //        AddData( kfdata->GetKeyID(), kfdata );
        //    }

        //    kfchild = kfother->NextData();
        //}
    }

    void KFRecord::SaveTo( KFData* kfother )
    {
        kfother->CopyFrom( this );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFRecord::FindData( uint64 key )
    {
        return _data.Find( key );
    }

    KFData* KFRecord::FindData( uint64 key, const std::string& dataname )
    {
        auto kfdata = FindData( key );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->FindData( dataname );
    }

    KFData* KFRecord::FindData( uint64 parentkey, uint64 childkey )
    {
        auto kfdata = FindData( parentkey );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->FindData( childkey );
    }

    KFData* KFRecord::FindData( uint64 parentkey, uint64 childkey, const std::string& dataname )
    {
        auto kfdata = FindData( parentkey );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->FindData( childkey, dataname );
    }

    KFData* KFRecord::FindData( uint64 parentkey, const std::string& dataname, uint64 childkey )
    {
        auto kfdata = FindData( parentkey );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->FindData( dataname, childkey );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRecord::AddData( uint64 key, KFData* data )
    {
        data->SetKeyID( key );
        data->SetParent( this );

        _data.Insert( key, data );

        return true;
    }

    bool KFRecord::AddData( uint64 parentkey, uint64 childkey, KFData* data )
    {
        auto kfdata = FindData( parentkey );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return kfdata->AddData( childkey, data );
    }

    bool KFRecord::AddData( uint64 key, const std::string& dataname, KFData* data )
    {
        auto kfdata = FindData( key );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return kfdata->AddData( dataname, data );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFRecord::MoveData( uint64 key )
    {
        auto kfdata = _data.Find( key );
        if ( kfdata != nullptr )
        {
            _data.Remove( key, false );
        }

        return kfdata;
    }

    bool KFRecord::RemoveData( uint64 key )
    {
        return _data.Remove( key );
    }

    bool KFRecord::RemoveData( uint64 key, const std::string& dataname )
    {
        auto kfdata = FindData( key );
        if ( kfdata == nullptr )
        {
            return false;
        }

        return kfdata->RemoveData( dataname );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRecord::FindData( const std::string& dataname, uint64 value, std::list<KFData*>& findlist )
    {
        auto child = FirstData();
        while ( child != nullptr )
        {
            auto datavalue = child->GetValue( dataname );
            if ( datavalue == value )
            {
                findlist.push_back( child );
            }

            child = NextData();
        }
    }

    bool KFRecord::CheckData( const std::string& dataname, uint64 value, const std::string& checkname, uint64 checkvalue )
    {
        uint64 totalvalue = 0;

        auto child = FirstData();
        while ( child != nullptr )
        {
            auto datavalue = child->GetValue( dataname );
            if ( datavalue == value )
            {
                totalvalue += child->GetValue( checkname );
                if ( totalvalue >= checkvalue )
                {
                    return true;
                }
            }

            child = NextData();
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
        //    AddData( key, kfdata );
        //}
    }
}