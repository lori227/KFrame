#include "KFString.hpp"

namespace KFrame
{
    KFString::KFString()
    {
        _data_type = KFDataDefine::DataTypeString;
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

    void KFString::Reset()
    {
        _data.clear();
        KFData::Reset();
    }

    bool KFString::IsValid()
    {
        return !_data.empty();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFString::CopyFrom( DataPtr& other )
    {
        _data = other->Get<std::string>();
    }

    void KFString::SaveTo( DataPtr& other )
    {
        other->Set( _data );
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
