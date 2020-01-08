#include "KFProjectModule.hpp"

namespace KFrame
{
    void KFProjectModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////
        auto projectstarttime = GetUInt64( "projectstarttime" );
        KFGlobal::Instance()->UUIDStartTime( projectstarttime );

        // 修改uuid 系数
        auto timerbits = GetUInt64( "uuidtimerbits" );
        auto zonebits = GetUInt64( "uuidzonebits" );
        auto workerbits = GetUInt64( "uuidworkerbits" );
        auto seqbits = GetUInt64( "uuidseqbits" );
        KFGlobal::Instance()->UUIDSetting( timerbits, zonebits, workerbits, seqbits );
        ////////////////////////////////////////////////////////////////////
    }

    ///////////////////////////////////////////////////////////////////////////////
    uint64 KFProjectModule::GetUInt64( const std::string& name )
    {
        auto kfsetting = KFProjectConfig::Instance()->FindSetting( name );
        if ( kfsetting == nullptr )
        {
            return _invalid_int;
        }

        return kfsetting->_uint64_value;
    }

    ///////////////////////////////////////////////////////////////////////////////
    double KFProjectModule::GetDouble( const std::string& name )
    {
        auto kfsetting = KFProjectConfig::Instance()->FindSetting( name );
        if ( kfsetting == nullptr )
        {
            return _invalid_double;
        }

        return kfsetting->_double_value;
    }

    ///////////////////////////////////////////////////////////////////////////////
    const std::string& KFProjectModule::GetString( const std::string& name )
    {
        auto kfsetting = KFProjectConfig::Instance()->FindSetting( name );
        if ( kfsetting == nullptr )
        {
            return _invalid_string;
        }

        return kfsetting->_str_value;
    }
}