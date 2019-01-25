#include "KFClassSetting.h"

namespace KFrame
{
    const KFDataSetting* KFClassSetting::FindDataSetting( const std::string& name ) const
    {
        return _static_data.Find( name );
    }

    bool KFClassSetting::IsChildData( const std::string& name ) const
    {
        return FindDataSetting( name ) != nullptr;
    }
}