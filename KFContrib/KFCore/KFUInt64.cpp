#include "KFUInt64.h"

namespace KFrame
{
    uint64 KFUInt64::GetUInt64()
    {
        return _data;
    }

    uint64 KFUInt64::SetUInt64( uint64 value )
    {
        _data = value;
        return _data;
    }

    void KFUInt64::Reset()
    {
        _data = _invalid_int;
    }

    bool KFUInt64::IsValid()
    {
        return _data != _invalid_int;
    }

    bool KFUInt64::IsFull()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFUInt64::CopyFrom( KFData* kfother )
    {
        SetValue( kfother->GetValue<uint64>() );
    }

    void KFUInt64::SaveTo( KFData* kfother )
    {
        kfother->SetValue( GetValue<uint64>() );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFUInt64::ToString()
    {
        return KFUtility::ToString( _data );
    }

    void KFUInt64::FromString( const std::string& value )
    {
        SetUInt64( KFUtility::ToValue<uint64>( value ) );
    }
}
