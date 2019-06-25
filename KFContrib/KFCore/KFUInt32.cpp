#include "KFUInt32.h"

namespace KFrame
{
    uint32 KFUInt32::GetUInt32()
    {
        return _data;
    }

    uint32 KFUInt32::SetUInt32( uint32 value )
    {
        _data = value;
        if ( _data_setting->_max_value != 0 )
        {
            if ( _data > _data_setting->_max_value )
            {
                _data = _data_setting->_max_value;
            }
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
        if ( _data_setting->_max_value != 0 )
        {
            if ( _data >= _data_setting->_max_value )
            {
                return true;
            }
        }

        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    void KFUInt32::CopyFrom( KFData* kfother )
    {
        SetValue( kfother->GetValue<uint32>() );
    }

    void KFUInt32::SaveTo( KFData* kfother )
    {
        kfother->SetValue( GetValue<uint32>() );
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
