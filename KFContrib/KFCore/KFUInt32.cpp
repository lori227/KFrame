#include "KFUInt32.hpp"

namespace KFrame
{
    KFUInt32::KFUInt32()
    {
        _data_type = KFDataDefine::DataTypeUInt32;
    }

    uint32 KFUInt32::GetUInt32()
    {
        return _data;
    }

    uint32 KFUInt32::SetUInt32( uint32 value )
    {
        _data = value;
        if ( _data > ( uint32 )_data_setting->_int_max_value )
        {
            _data = ( uint32 )_data_setting->_int_max_value;
        }
        else if ( _data < ( uint32 )_data_setting->_int_min_value )
        {
            _data = ( uint32 )_data_setting->_int_min_value;
        }

        return _data;
    }

    void KFUInt32::Reset()
    {
        _data = _invalid_int;
        KFData::Reset();
    }

    bool KFUInt32::IsValid()
    {
        return _data != _invalid_int;
    }

    bool KFUInt32::IsFull()
    {
        return _data >= ( uint32 )_data_setting->_int_max_value;
    }

    void KFUInt32::InitData()
    {
        _data = _data_setting->_int_init_value;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    void KFUInt32::CopyFrom( DataPtr& other )
    {
        Set( other->Get<uint32>() );
    }

    void KFUInt32::SaveTo( DataPtr& other )
    {
        other->Set( Get<uint32>() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFUInt32::ToString()
    {
        return KFUtility::ToString( _data );
    }

    void KFUInt32::FromString( const std::string& value )
    {
        SetUInt32( __TO_UINT32__( value ) );
    }
}
