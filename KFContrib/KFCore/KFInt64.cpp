#include "KFInt64.h"

namespace KFrame
{
    int64 KFInt64::GetInt64()
    {
        return _data;
    }

    int64 KFInt64::SetInt64( int64 value )
    {
        _data = value;
        return _data;
    }

    void KFInt64::Reset()
    {
        _data = _invalid_int;
    }

    bool KFInt64::IsValid()
    {
        return _data != _invalid_int;
    }

    bool KFInt64::IsFull()
    {
        return false;
    }

    void KFInt64::InitData()
    {
        if ( _data_setting->_int_init_value != 0 )
        {
            _data = _data_setting->_int_init_value;
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFInt64::CopyFrom( KFData* kfother )
    {
        Set( kfother->Get<int64>() );
    }

    void KFInt64::SaveTo( KFData* kfother )
    {
        kfother->Set( Get<int64>() );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFInt64::ToString()
    {
        return KFUtility::ToString( _data );
    }

    void KFInt64::FromString( const std::string& value )
    {
        SetInt64( KFUtility::ToValue<int64>( value ) );
    }
}
