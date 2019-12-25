#include "KFProjectModule.hpp"

namespace KFrame
{
    void KFProjectModule::BeforeRun()
    {
        KFGlobal::Instance()->_project_start_time = GetUInt64( "projectstarttime" );
    }

    ///////////////////////////////////////////////////////////////////////////////

    uint64 KFProjectModule::GetUInt64( const std::string& name )
    {
        auto setting = KFProjectConfig::Instance()->FindSetting( name );
        return setting->_uint64_value;
    }

    ///////////////////////////////////////////////////////////////////////////////
    double KFProjectModule::GetDouble( const std::string& name )
    {
        auto setting = KFProjectConfig::Instance()->FindSetting( name );
        return setting->_double_value;
    }

    ///////////////////////////////////////////////////////////////////////////////
    const std::string& KFProjectModule::GetString( const std::string& name )
    {
        auto setting = KFProjectConfig::Instance()->FindSetting( name );
        return setting->_str_value;
    }
}