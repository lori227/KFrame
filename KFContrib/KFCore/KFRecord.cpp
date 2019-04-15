#include "KFRecord.h"
#include "KFDataFactory.h"

namespace KFrame
{
    KFRecord::KFRecord()
    {
        _type = KFDataDefine::Type_Record;
    }

    KFRecord::~KFRecord()
    {

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
        auto kfchild = kfother->FirstData();
        while ( kfchild != nullptr )
        {
            auto kfdata = KFDataFactory::CreateData( kfchild->GetDataSetting() );
            if ( kfdata != nullptr )
            {
                kfdata->CopyFrom( kfchild );
                AddData( kfdata->GetKeyID(), kfdata );
            }

            kfchild = kfother->NextData();
        }
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
        // [1={xxxx}=1,2={xxxx}=2]

        std::string result = "";
        result += "[";

        for ( auto& iter : _data._objects )
        {
            auto key = __TO_STRING__( iter.first );
            auto kfdata = iter.second;

            auto strdata = kfdata->ToString();
            auto temp = __FORMAT__( "{}={}={},", key, strdata, key );
            result += temp;
        }

        result += "]";

        return result;
    }

    void KFRecord::FromString( const std::string& value )
    {
        auto temp = value;
        KFUtility::SplitString( temp, "[" );

        while ( !temp.empty() )
        {
            auto strkey = KFUtility::SplitString( temp, "=" );

            auto split = "=" + strkey;
            auto data = KFUtility::SplitString( temp, split );
            if ( !data.empty() )
            {
                auto kfdata = KFDataFactory::CreateData( _data_setting );
                if ( kfdata != nullptr )
                {
                    auto key = KFUtility::ToValue< uint64 >( strkey );
                    kfdata->SetKeyID( key );
                    kfdata->FromString( data );
                    AddData( key, kfdata );
                }
            }

            KFUtility::SplitString( temp, "," );
        }
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
        for ( auto iter : values )
        {
            auto kfdata = KFDataFactory::CreateData( _data_setting );
            if ( kfdata == nullptr )
            {
                continue;
            }

            auto key = KFUtility::ToValue< uint64 >( iter.first );
            kfdata->SetKeyID( key );
            kfdata->FromString( iter.second );
            AddData( key, kfdata );
        }
    }
}