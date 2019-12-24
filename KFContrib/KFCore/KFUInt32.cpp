#include "KFUInt32.h"

namespace KFrame
{
    KFUInt32::KFUInt32()
    {
        _data_type = KFDataDefine::Type_UInt32;
    }

    uint32 KFUInt32::GetUInt32()
    {
        return _data;
    }

    uint32 KFUInt32::SetUInt32( uint32 value )
    {
        _data = value;
        if ( _data > _data_setting->_int_max_value )
        {
            _data = _data_setting->_int_max_value;
        }
        else if ( _data < _data_setting->_int_min_value )
        {
            _data = _data_setting->_int_min_value;
        }

        return _data;
    }

    void KFUInt32::Reset( bool isdelete /* = true */ )
    {
        _data = _invalid_int;
        KFData::Reset( isdelete );
    }

    bool KFUInt32::IsValid()
    {
        return _data != _invalid_int;
    }

    bool KFUInt32::IsFull()
    {
        return _data >= _data_setting->_int_max_value;
    }

    void KFUInt32::InitData()
    {
        if ( _data_setting->_int_init_value != 0u )
        {
            _data = _data_setting->_int_init_value;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    void KFUInt32::CopyFrom( KFData* kfother )
    {
        Set( kfother->Get<uint32>() );
    }

    void KFUInt32::SaveTo( KFData* kfother )
    {
        kfother->Set( Get<uint32>() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFUInt32::ToString()
    {
        return KFUtility::ToString( _data );
    }

    void KFUInt32::FromString( const std::string& value )
    {
        SetUInt32( KFUtility::ToValue<uint32>( value ) );
    }
}
