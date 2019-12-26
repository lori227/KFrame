#include "KFProjectModule.hpp"

namespace KFrame
{
    void KFProjectModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////
        // 修改uuid 系数
        auto timerbits = GetUInt64( "uuidtimerbits" );
        auto zonebits = GetUInt64( "uuidzonebits" );
        auto workerbits = GetUInt64( "uuidworkerbits" );
        auto seqbits = GetUInt64( "uuidseqbits" );
        auto projectstarttime = GetUInt64( "projectstarttime" );

        KFGlobal::Instance()->UUIDSetting( timerbits, zonebits, workerbits, seqbits, projectstarttime );
        ////////////////////////////////////////////////////////////////////
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