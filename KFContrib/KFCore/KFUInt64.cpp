#include "KFUInt64.hpp"

namespace KFrame
{
    KFUInt64::KFUInt64()
    {
        _data_type = KFDataDefine::Type_UInt64;
    }

    uint64 KFUInt64::GetUInt64()
    {
        return _data;
    }

    uint64 KFUInt64::SetUInt64( uint64 value )
    {
        _data = value;
        if ( _data > ( uint64 )_data_setting->_int_max_value )
        {
            _data = ( uint64 )_data_setting->_int_max_value;
        }
        else if ( _data < ( uint64 )_data_setting->_int_min_value )
        {
            _data = ( uint64 )_data_setting->_int_min_value;
        }
        return _data;
    }

    void KFUInt64::Reset( bool isdelete /* = true */ )
    {
        _data = _invalid_int;
        KFData::Reset( isdelete );
    }

    bool KFUInt64::IsValid()
    {
        return _data != _invalid_int;
    }

    bool KFUInt64::IsFull()
    {
        return _data >= ( uint64 )_data_setting->_int_max_value;
    }

    void KFUInt64::InitData()
    {
        _data = _data_setting->_int_init_value;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFUInt64::CopyFrom( KFData* kfother )
    {
        Set( kfother->Get<uint64>() );
    }

    void KFUInt64::SaveTo( KFData* kfother )
    {
        kfother->Set( Get<uint64>() );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFUInt64::ToString()
    {
        return KFUtility::ToString( _data );
    }

    void KFUInt64::FromString( const std::string& value )
    {
        SetUInt64( __TO_UINT64__( value ) );
    }
}
