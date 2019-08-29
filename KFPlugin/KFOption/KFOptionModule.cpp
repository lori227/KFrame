#include "KFOptionModule.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////
    const KFOptionSetting* KFOptionModule::FindOption( const std::string& name )
    {
        return KFOptionConfig::Instance()->FindOption( name, _invalid_str );
    }

    const KFOptionSetting* KFOptionModule::FindOption( const std::string& name, uint32 key )
    {
        auto strkey = __TO_STRING__( key );
        return KFOptionConfig::Instance()->FindOption( name, strkey );
    }

    const KFOptionSetting* KFOptionModule::FindOption( const std::string& name, const std::string& key )
    {
        return KFOptionConfig::Instance()->FindOption( name, key );
    }

    ///////////////////////////////////////////////////////////////////////////////
    void KFOptionModule::AddOption( const std::string& name, uint32 value )
    {
        AddOption( name, _invalid_str, value );
    }

    void KFOptionModule::AddOption( const std::string& name, uint32 key, uint32 value )
    {
        auto strkey = __TO_STRING__( key );
        AddOption( name, strkey, value );
    }

    void KFOptionModule::AddOption( const std::string& name, const std::string& key, uint32 value )
    {
        auto kfsetting = KFOptionConfig::Instance()->_option_list.Create( KFOptionConfig::OptionKey( name, key ) );
        kfsetting->_uint32_value = value;
        kfsetting->_double_value = value;
        kfsetting->_str_value = __TO_STRING__( value );
    }

    void KFOptionModule::AddOption( const std::string& name, double value )
    {
        AddOption( name, _invalid_str, value );
    }

    void KFOptionModule::AddOption( const std::string& name, uint32 key, double value )
    {
        auto strkey = __TO_STRING__( key );
        AddOption( name, strkey, value );
    }

    void KFOptionModule::AddOption( const std::string& name, const std::string& key, double value )
    {
        auto kfsetting = KFOptionConfig::Instance()->_option_list.Create( KFOptionConfig::OptionKey( name, key ) );
        kfsetting->_uint32_value = value;
        kfsetting->_double_value = value;
        kfsetting->_str_value = __TO_STRING__( value );
    }

    void KFOptionModule::AddOption( const std::string& name, const std::string& value )
    {
        AddOption( name, _invalid_str, value );
    }

    void KFOptionModule::AddOption( const std::string& name, uint32 key, const std::string& value )
    {
        auto strkey = __TO_STRING__( key );
        AddOption( name, strkey, value );
    }

    void KFOptionModule::AddOption( const std::string& name, const std::string& key, const std::string& value )
    {
        auto kfsetting = KFOptionConfig::Instance()->_option_list.Create( KFOptionConfig::OptionKey( name, key ) );
        kfsetting->_uint32_value = KFUtility::ToValue<uint32>( value );
        kfsetting->_double_value = KFUtility::ToValue<double>( value );
        kfsetting->_str_value = value;
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