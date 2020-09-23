#include "KFData.h"

namespace KFrame
{
    // 父属性
    void KFData::SetParent( KFData* data )
    {
        _parent = data;
    }

    KFData* KFData::GetParent()
    {
        return _parent;
    }

    bool KFData::IsValid()
    {
        return true;
    }

    // 数量
    uint32 KFData::Size()
    {
        return 1;
    }

    // 重置数量
    void KFData::Resize( uint32 size )
    {

    }

    uint32 KFData::MaxSize()
    {
        return 0;
    }

    uint32 KFData::GetConfigID()
    {
        return 0;
    }

    uint64 KFData::GetKeyID()
    {
        return 0;
    }

    void KFData::SetKeyID( uint64 id )
    {

    }

    KFData* KFData::First()
    {
        return this;
    }

    KFData* KFData::Next()
    {
        return nullptr;
    }


    int32 KFData::GetInt32()
    {
        return 0;
    }
    int32 KFData::SetInt32( int32 value )
    {
        return 0;
    }

    uint32 KFData::GetUInt32()
    {
        return 0;
    }
    uint32 KFData::SetUInt32( uint32 value )
    {
        return 0;
    }

    int64 KFData::GetInt64()
    {
        return 0;
    }
    int64 KFData::SetInt64( int64 value )
    {
        return 0;
    }

    uint64 KFData::GetUInt64()
    {
        return 0;
    }
    uint64 KFData::SetUInt64( uint64 value )
    {
        return 0;
    }

    double KFData::GetDouble()
    {
        return 0.0f;
    }
    double KFData::SetDouble( double value )
    {
        return 0.0f;
    }

    const std::string& KFData::GetString()
    {
        return _invalid_string;
    }
    const std::string& KFData::SetString( const std::string& value )
    {
        return _invalid_string;
    }

    const Math3D::Vector3D& KFData::GetVector3D()
    {
        return Math3D::Vector3D::xAxis;
    }

    const Math3D::Vector3D& KFData::SetVector3D( const Math3D::Vector3D& value )
    {
        return Math3D::Vector3D::xAxis;
    }

    void KFData::ToMap( StringMap& values )
    {

    }

    void KFData::FromMap( const StringMap& values )
    {

    };

    // object
    KFData* KFData::Find( const std::string& dataname )
    {
        return nullptr;
    }
    KFData* KFData::Find( const std::string& dataname, uint64 key )
    {
        return nullptr;
    }
    KFData* KFData::Find( const std::string& parentname, const std::string& childname )
    {
        return nullptr;
    }
    KFData* KFData::Find( const std::string& parentname, uint64 key, const std::string& childname )
    {
        return nullptr;
    }
    KFData* KFData::Find( const std::string& parentname, const std::string& childname, uint64 key )
    {
        return nullptr;
    }
    KFData* KFData::Find( const std::string& parentname, const std::string& childname, const std::string& dataname )
    {
        return nullptr;
    }
    bool KFData::Add( const std::string& dataname, KFData* data )
    {
        return false;
    }
    bool KFData::Add( const std::string& dataname, uint64 key, KFData* data )
    {
        return false;
    }
    bool KFData::Add( const std::string& parentname, const std::string& childname, KFData* data )
    {
        return false;
    }

    KFData* KFData::Move( uint64 key )
    {
        return nullptr;
    }

    KFData* KFData::Move( const std::string& dataname, bool usekey )
    {
        return nullptr;
    }

    bool KFData::Remove( const std::string& dataname )
    {
        return false;
    }

    bool KFData::Remove( const std::string& dataname, uint64 key )
    {
        return false;
    }

    std::string KFData::ToString( const std::string& dataname )
    {
        auto kfdata = Find( dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_string;
        }

        return kfdata->ToString();
    }

    std::string KFData::ToString( const std::string& parentname, const std::string& dataname )
    {
        auto kfdata = Find( parentname, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_string;
        }

        return kfdata->ToString();
    }

    std::string KFData::ToString( uint64 key, const std::string& dataname )
    {
        auto kfdata = Find( key, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_string;
        }

        return kfdata->ToString();
    }

    std::string KFData::ToString( const std::string& parentname, uint64 key, const std::string& dataname )
    {
        auto kfdata = Find( parentname, key, dataname );
        if ( kfdata == nullptr )
        {
            return _invalid_string;
        }

        return kfdata->ToString();
    }

    // record
    bool KFData::IsFull()
    {
        return false;
    }

    KFData* KFData::Find( uint64 key )
    {
        return nullptr;
    }
    KFData* KFData::Find( uint64 key, const std::string& dataname )
    {
        return nullptr;
    }
    KFData* KFData::Find( uint64 parentkey, uint64 childkey )
    {
        return nullptr;
    }
    KFData* KFData::Find( uint64 parentkey, uint64 childkey, const std::string& dataname )
    {
        return nullptr;
    }
    KFData* KFData::Find( uint64 parentkey, const std::string& dataname, uint64 childkey )
    {
        return nullptr;
    }

    bool KFData::Add( uint64 key, KFData* data )
    {
        return false;
    }
    bool KFData::Add( uint64 parentkey, uint64 childkey, KFData* data )
    {
        return false;
    }
    bool KFData::Add( uint64 key, const std::string& dataname, KFData* data )
    {
        return false;
    }

    bool KFData::Remove( uint64 key )
    {
        return false;
    }

    void KFData::Find( const std::string& dataname, uint64 value, std::list< KFData* >& findlist, bool findall )
    {
    }
    bool KFData::Check( const std::string& dataname, uint64 value, const std::string& checkname, uint64 checkvalue )
    {
        return false;
    }

    KFData* KFData::Insert( uint64 value )
    {
        return nullptr;
    }

    uint32 KFData::GetIndex( uint64 value )
    {
        return 0u;
    }

    uint32 KFData::GetEmpty()
    {
        return 0u;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void KFData::Initialize( const KFDataSetting* datasetting )
    {
        _data_setting = datasetting;
        InitData();
    }

    void KFData::InitData()
    {

    }

    void KFData::Reset( bool isdelete /* = true */ )
    {
    }

    bool KFData::HaveMask( uint32 mask ) const
    {
        return _data_setting->HaveMask( mask ) || KFUtility::HaveBitMask( _run_mask, mask );
    }

    void KFData::AddMask( uint32 mask )
    {
        KFUtility::AddBitMask( _run_mask, mask );
    }

    void KFData::RemoveMask( uint32 mask )
    {
        KFUtility::ClearBitMask( _run_mask, mask );
    }

    bool KFData::IsNeedSyncToClient() const
    {
        if ( _data_setting->HaveMask( KFDataDefine::DataMaskSync ) )
        {
            if ( !_data_setting->HaveMask( KFDataDefine::DataMaskDelay ) ||
                    KFUtility::HaveBitMask<uint32>( _run_mask, KFDataDefine::DataMaskSync ) )
            {
                return true;
            }
        }

        return false;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFData::IsArray() const
    {
        return _data_type == KFDataDefine::DataTypeArray;
    }

    bool KFData::IsRecord() const
    {
        return _data_type == KFDataDefine::DataTypeRecord;
    }

    bool KFData::IsObject() const
    {
        return _data_type == KFDataDefine::DataTypeObject;
    }

    bool KFData::IsString() const
    {
        return _data_type == KFDataDefine::DataTypeString;
    }

    bool KFData::IsInt() const
    {
        switch ( _data_type )
        {
        case KFDataDefine::DataTypeInt32:
        case KFDataDefine::DataTypeUInt32:
        case KFDataDefine::DataTypeInt64:
        case KFDataDefine::DataTypeUInt64:
            return true;
        default:
            break;
        }

        return false;
    }
}