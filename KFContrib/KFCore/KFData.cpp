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

    uint32 KFData::MaxSize()
    {
        return _data_setting->_int_max_value;
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
    int32 KFData::SetInt32( int32 value )
    {
        return value;
    }

    uint32 KFData::GetUInt32()
    {
        return 0;
    }
    uint32 KFData::SetUInt32( uint32 value )
    {
        return value;
    }

    int64 KFData::GetInt64()
    {
        return 0;
    }
    int64 KFData::SetInt64( int64 value )
    {
        return value;
    }

    uint64 KFData::GetUInt64()
    {
        return 0;
    }
    uint64 KFData::SetUInt64( uint64 value )
    {
        return value;
    }

    double KFData::GetDouble()
    {
        return 0.0f;
    }
    double KFData::SetDouble( double value )
    {
        return value;
    }

    const std::string& KFData::GetString()
    {
        return _invalid_str;
    }
    const std::string& KFData::SetString( const std::string& value )
    {
        return value;
    }

    const Math3D::Vector3D& KFData::GetVector3D()
    {
        return Math3D::Vector3D::xAxis;
    }

    const Math3D::Vector3D& KFData::SetVector3D( const Math3D::Vector3D& value )
    {
        return value;
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

    KFData* KFData::MoveData( uint64 key )
    {
        return nullptr;
    }

    KFData* KFData::MoveData( const std::string& dataname )
    {
        return nullptr;
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
    bool KFData::IsFull()
    {
        return false;
    }

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

    void KFData::FindData( const std::string& dataname, uint64 value, std::list< KFData* >& findlist, bool findall )
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
        KFData::Reset();
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
        if ( _data_setting->HaveMask( KFDataDefine::Mask_Client ) )
        {
            if ( !_data_setting->HaveMask( KFDataDefine::Mask_Delay ) ||
                    KFUtility::HaveBitMask<uint32>( _run_mask, KFDataDefine::Mask_Client ) )
            {
                return true;
            }
        }

        return false;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
}