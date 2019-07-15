#include "KFString.h"

namespace KFrame
{
    const std::string& KFString::GetString()
    {
        return _data;
    }

    const std::string& KFString::SetString( const std::string& value )
    {
        _data = value;
        return _data;
    }

    void KFString::Reset()
    {
        _data.clear();
    }

    bool KFString::IsValid()
    {
        return !_data.empty();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFString::CopyFrom( KFData* kfother )
    {
        _data = kfother->GetValue<std::string>();
    }

    void KFString::SaveTo( KFData* kfother )
    {
        kfother->SetValue( _data );
    }

    std::string KFString::ToString()
    {
        return _data;
    }

    void KFString::FromString( const std::string& value )
    {
        _data = value;
    }
}
