#include "KFData.h"

namespace KFrame
{
    KFData::~KFData()
    {
        _data_setting = nullptr;
    }

    // 父属性
    void KFData::SetParent( KFData* data )
    {
        _parent = data;
    }

    KFData* KFData::GetParent()
    {
        return _parent;
    }

    DataPtr KFData::GetDataPtr()
    {
        return shared_from_this();
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

    DataPtr KFData::First()
    {
        return shared_from_this();
    }

    DataPtr KFData::Next()
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
    DataPtr KFData::Find( const std::string& data_name )
    {
        return nullptr;
    }
    DataPtr KFData::Find( const std::string& data_name, uint64 key )
    {
        return nullptr;
    }
    DataPtr KFData::Find( const std::string& parent_name, const std::string& child_name )
    {
        return nullptr;
    }
    DataPtr KFData::Find( const std::string& parent_name, uint64 key, const std::string& child_name )
    {
        return nullptr;
    }
    DataPtr KFData::Find( const std::string& parent_name, const std::string& child_name, uint64 key )
    {
        return nullptr;
    }
    DataPtr KFData::Find( const std::string& parent_name, const std::string& child_name, const std::string& data_name )
    {
        return nullptr;
    }
    bool KFData::Add( const std::string& data_name, DataPtr& data )
    {
        return false;
    }
    bool KFData::Add( const std::string& data_name, uint64 key, DataPtr& data )
    {
        return false;
    }
    bool KFData::Add( const std::string& parent_name, const std::string& child_name, DataPtr& data )
    {
        return false;
    }

    DataPtr KFData::Remove( const std::string& data_name )
    {
        return nullptr;
    }

    DataPtr KFData::Remove( const std::string& data_name, uint64 key )
    {
        return nullptr;
    }

    std::string KFData::ToString( const std::string& data_name )
    {
        auto data = Find( data_name );
        if ( data == nullptr )
        {
            return _invalid_string;
        }

        return data->ToString();
    }

    std::string KFData::ToString( const std::string& parent_name, const std::string& data_name )
    {
        auto data = Find( parent_name, data_name );
        if ( data == nullptr )
        {
            return _invalid_string;
        }

        return data->ToString();
    }

    std::string KFData::ToString( uint64 key, const std::string& data_name )
    {
        auto data = Find( key, data_name );
        if ( data == nullptr )
        {
            return _invalid_string;
        }

        return data->ToString();
    }

    std::string KFData::ToString( const std::string& parent_name, uint64 key, const std::string& data_name )
    {
        auto data = Find( parent_name, key, data_name );
        if ( data == nullptr )
        {
            return _invalid_string;
        }

        return data->ToString();
    }

    // record
    bool KFData::IsFull()
    {
        return false;
    }

    DataPtr KFData::Find( uint64 key )
    {
        return nullptr;
    }
    DataPtr KFData::Find( uint64 key, const std::string& data_name )
    {
        return nullptr;
    }
    DataPtr KFData::Find( uint64 parent_key, uint64 child_key )
    {
        return nullptr;
    }
    DataPtr KFData::Find( uint64 parent_key, uint64 child_key, const std::string& data_name )
    {
        return nullptr;
    }
    DataPtr KFData::Find( uint64 parent_key, const std::string& data_name, uint64 child_key )
    {
        return nullptr;
    }

    bool KFData::Add( uint64 key, DataPtr& data )
    {
        return false;
    }
    bool KFData::Add( uint64 parent_key, uint64 child_key, DataPtr& data )
    {
        return false;
    }
    bool KFData::Add( uint64 key, const std::string& data_name, DataPtr& data )
    {
        return false;
    }

    DataPtr KFData::Remove( uint64 key )
    {
        return nullptr;
    }

    void KFData::Find( const std::string& data_name, uint64 value, std::list<DataPtr>& find_list, bool find_all )
    {
    }
    bool KFData::Check( const std::string& data_name, uint64 value, const std::string& check_name, uint64 check_value )
    {
        return false;
    }

    DataPtr KFData::Insert( uint64 value )
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
    void KFData::InitSetting( std::shared_ptr<const KFDataSetting> data_setting )
    {
        _data_setting = data_setting;
        InitData();
    }

    void KFData::InitData()
    {

    }

    void KFData::Reset()
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
            if ( !_data_setting->HaveMask( KFDataDefine::DataMaskRequest ) ||
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

    // 获得配置表属性
    uint64 KFData::GetConfigValue( const std::string& name, uint64 id, uint64 maxvalue )
    {
        return 0u;
    }
}