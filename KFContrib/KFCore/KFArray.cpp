#include "KFArray.h"
#include "KFDataFactory.h"

namespace KFrame
{
    KFArray::KFArray()
    {
        _type = KFDataDefine::Type_Array;
    }

    KFArray::~KFArray()
    {

    }

    void KFArray::Initialize( const KFClassSetting* classsetting, const KFDataSetting* datasetting )
    {
        KFData::Initialize( classsetting, datasetting );

        auto size = KFUtility::ToValue<uint32>( datasetting->_init_value ) + KFDataDefine::Array_Index;
        _data.Resize( size );

        for ( uint32 i = KFDataDefine::Array_Index; i < size; ++i )
        {
            auto kfdata = KFDataFactory::Create( _data_setting->_contain_class );
            kfdata->SetParent( this );
            kfdata->Initialize( classsetting, datasetting );

            kfdata->SetValue( 0u );
            _data.Insert( i, kfdata );
        }
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFArray::Size()
    {
        return _data.MaxSize();
    }

    KFData* KFArray::FirstData()
    {
        uint32 index = KFDataDefine::Array_Index;
        if ( index == 0u )
        {
            return _data.First();
        }

        _data.First();
        return _data.Next();
    }

    KFData* KFArray::NextData()
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
            auto finddata = kfother->FindData( key++ );
            if ( finddata != nullptr )
            {
                kfdata->SaveTo( finddata );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFArray::FindData( uint64 key )
    {
        return _data.Find( static_cast<uint32>( key ) );
    }

    KFData* KFArray::FindData( uint64 key, const std::string& dataname )
    {
        auto kfdata = FindData( key );
        if ( kfdata == nullptr )
        {
            return nullptr;
        }

        return kfdata->FindData( dataname );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFArray::AddData( uint64 key, KFData* data )
    {
        _data.Insert( static_cast<uint32>( key ), data );
        return true;
    }

    bool KFArray::AddData( uint64 key, const std::string& dataname, KFData* data )
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
    bool KFArray::RemoveData( uint64 key )
    {
        auto kfdata = FindData( key );
        if ( kfdata == nullptr )
        {
            return false;
        }

        kfdata->Reset();
        return true;
    }

    bool KFArray::RemoveData( uint64 key, const std::string& dataname )
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

    std::string KFArray::ToString()
    {
        __JSON_DOCUMENT__( kfjson );
        kfjson.SetArray();

        for ( auto kfdata : _data._objects )
        {
            if ( kfdata != nullptr )
            {
                __JSON_ADD_VALUE__( kfjson, kfdata->GetValue() );
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
            auto kfdata = FindData( i + KFDataDefine::Array_Index );
            if ( kfdata != nullptr )
            {
                auto& object = __JSON_ARRAY_INDEX__( kfjson, i );
                kfdata->FromString( object.GetString() );
            }
        }
    }
}
