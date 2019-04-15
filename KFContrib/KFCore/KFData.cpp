#include "KFData.h"

namespace KFrame
{
    KFData::KFData()
    {
        _parent = nullptr;
        _class_setting = nullptr;
        _data_setting = nullptr;
    }

    KFData::~KFData()
    {
        _parent = nullptr;
        _data_setting = nullptr;
        _class_setting = nullptr;
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

    // 数量
    uint32 KFData::Size()
    {
        return 1;
    }

    uint64 KFData::GetKeyID()
    {
        return 0;
    }

    void KFData::SetKeyID( uint64 id )
    {

    }

    KFData* KFData::FirstData()
    {
        return this;
    }

    KFData* KFData::NextData()
    {
        return nullptr;
    }


    int32 KFData::GetInt32()
    {
        return 0;
    }
    void KFData::SetInt32( int32 )
    {
    }

    uint32 KFData::GetUInt32()
    {
        return 0;
    }
    void KFData::SetUInt32( uint32 )
    {
    }

    int64 KFData::GetInt64()
    {
        return 0;
    }
    void KFData::SetInt64( int64 )
    {
    }

    uint64 KFData::GetUInt64()
    {
        return 0;
    }
    void KFData::SetUInt64( uint64 )
    {
    }

    double KFData::GetDouble()
    {
        return 0.0f;
    }
    void KFData::SetDouble( double )
    {
    }

    const std::string& KFData::GetString()
    {
        return _invalid_str;
    }
    void KFData::SetString( const std::string& )
    {
    }

    const Math3D::Vector3D& KFData::GetVector3D()
    {
        return Math3D::Vector3D::xAxis;
    }
    void KFData::SetVector3D( const Math3D::Vector3D& )
    {
    }

    void KFData::ToMap( MapString& values )
    {

    }

    void KFData::FromMap( const MapString& values )
    {

    };

    // object
    KFData* KFData::FindData( const std::string& dataname )
    {
        return nullptr;
    }

    KFData* KFData::FindData( const std::string& dataname, uint64 key )
    {
        return nullptr;
    }
    KFData* KFData::FindData( const std::string& parentname, const std::string& childname )
    {
        return nullptr;
    }
    KFData* KFData::FindData( const std::string& parentname, uint64 key, const std::string& childname )
    {
        return nullptr;
    }
    KFData* KFData::FindData( const std::string& parentname, const std::string& childname, uint64 key )
    {
        return nullptr;
    }
    KFData* KFData::FindData( const std::string& parentname, const std::string& childname, const std::string& dataname )
    {
        return nullptr;
    }
    bool KFData::AddData( const std::string& dataname, KFData* data )
    {
        return false;
    }
    bool KFData::AddData( const std::string& dataname, uint64 key, KFData* data )
    {
        return false;
    }
    bool KFData::AddData( const std::string& parentname, const std::string& childname, KFData* data )
    {
        return false;
    }

    bool KFData::RemoveData( const std::string& dataname )
    {
        return false;
    }
    bool KFData::RemoveData( const std::string& dataname, uint64 key )
    {
        return false;
    }
    bool KFData::RemoveData( const std::string& parentname, const std::string& childname )
    {
        return false;
    }


    // record
    KFData* KFData::FindData( uint64 key )
    {
        return nullptr;
    }
    KFData* KFData::FindData( uint64 key, const std::string& dataname )
    {
        return nullptr;
    }
    KFData* KFData::FindData( uint64 parentkey, uint64 childkey )
    {
        return nullptr;
    }
    KFData* KFData::FindData( uint64 parentkey, uint64 childkey, const std::string& dataname )
    {
        return nullptr;
    }
    KFData* KFData::FindData( uint64 parentkey, const std::string& dataname, uint64 childkey )
    {
        return nullptr;
    }

    bool KFData::AddData( uint64 key, KFData* data )
    {
        return false;
    }
    bool KFData::AddData( uint64 parentkey, uint64 childkey, KFData* data )
    {
        return false;
    }
    bool KFData::AddData( uint64 key, const std::string& dataname, KFData* data )
    {
        return false;
    }

    bool KFData::RemoveData( uint64 key )
    {
        return false;
    }
    bool KFData::RemoveData( uint64 key, const std::string& dataname )
    {
        return false;
    }

    void KFData::FindData( const std::string& dataname, uint64 value, std::list< KFData* >& findlist )
    {
    }
    bool KFData::CheckData( const std::string& dataname, uint64 value, const std::string& checkname, uint64 checkvalue )
    {
        return false;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void KFData::Initialize( const KFClassSetting* classsetting, const KFDataSetting* datasetting )
    {
        _data_setting = datasetting;
        _class_setting = classsetting;

        FromString( datasetting->_init_value );
    }

    const std::string& KFData::GetName() const
    {
        return _data_setting->_name;
    }

    uint32 KFData::GetType() const
    {
        return _type;
    }

    const KFDataSetting* KFData::GetDataSetting() const
    {
        return _data_setting;
    }

    const KFClassSetting* KFData::GetClassSetting() const
    {
        return _class_setting;
    }

    bool KFData::HaveFlagMask( uint32 mask )
    {
        return _data_setting->HaveFlagMask( mask );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
}