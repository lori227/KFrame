#include "KFOptionModule.h"
#include "KFOptionConfig.h"

namespace KFrame
{
    void KFOptionModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_option_config, true );
    }

    void KFOptionModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_option_config );
    }

    ////////////////////////////////////////////////////////////////
    const KFOption* KFOptionModule::FindOption( const std::string& name )
    {
        return _kf_option_config->FindOption( name, _invalid_str );
    }

    const KFOption* KFOptionModule::FindOption( const std::string& name, uint32 key )
    {
        auto strkey = __TO_STRING__( key );
        return _kf_option_config->FindOption( name, strkey );
    }

    const KFOption* KFOptionModule::FindOption( const std::string& name, const std::string& key )
    {
        return _kf_option_config->FindOption( name, key );
    }

    ///////////////////////////////////////////////////////////////////////////////
    uint32 KFOptionModule::GetUInt32( const std::string& name )
    {
        auto kfoption = FindOption( name );
        return kfoption->_uint32_value;
    }

    uint32 KFOptionModule::GetUInt32( const std::string& name, uint32 key )
    {
        auto kfoption = FindOption( name, key );
        return kfoption->_uint32_value;
    }

    uint32 KFOptionModule::GetUInt32( const std::string& name, const std::string& key )
    {
        auto kfoption = FindOption( name, key );
        return kfoption->_uint32_value;
    }

    ///////////////////////////////////////////////////////////////////////////////
    double KFOptionModule::GetDouble( const std::string& name )
    {
        auto kfoption = FindOption( name );
        return kfoption->_double_value;
    }

    double KFOptionModule::GetDouble( const std::string& name, uint32 key )
    {
        auto kfoption = FindOption( name, key );
        return kfoption->_double_value;
    }

    double KFOptionModule::GetDouble( const std::string& name, const std::string& key )
    {
        auto kfoption = FindOption( name, key );
        return kfoption->_double_value;
    }

    ///////////////////////////////////////////////////////////////////////////////
    const std::string& KFOptionModule::GetString( const std::string& name )
    {
        auto kfoption = FindOption( name );
        return kfoption->_str_value;
    }

    const std::string& KFOptionModule::GetString( const std::string& name, uint32 key )
    {
        auto kfoption = FindOption( name, key );
        return kfoption->_str_value;
    }

    const std::string& KFOptionModule::GetString( const std::string& name, const std::string& key )
    {
        auto kfoption = FindOption( name, key );
        return kfoption->_str_value;
    }
}