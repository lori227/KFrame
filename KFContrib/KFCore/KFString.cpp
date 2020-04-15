#include "KFString.hpp"

namespace KFrame
{
    KFString::KFString()
    {
        _data_type = KFDataDefine::Type_String;
    }

    const std::string& KFString::GetString()
    {
        return _data;
    }

    const std::string& KFString::SetString( const std::string& value )
    {
        _data = value;
        return _data;
    }

    void KFString::Reset( bool isdelete /* = true */ )
    {
        _data.clear();
        KFData::Reset( isdelete );
    }

    bool KFString::IsValid()
    {
        return !_data.empty();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFString::CopyFrom( KFData* kfother )
    {
        _data = kfother->Get<std::string>();
    }

    void KFString::SaveTo( KFData* kfother )
    {
        kfother->Set( _data );
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
