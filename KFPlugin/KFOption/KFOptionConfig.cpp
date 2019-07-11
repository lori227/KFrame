#include "KFOptionConfig.hpp"

namespace KFrame
{
    KFOption* KFOptionConfig::CreateSetting( KFNode& xmlnode )
    {
        auto service = xmlnode.GetUInt32( "Service", true );
        auto channel = xmlnode.GetUInt32( "Channel", true );
        auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
        if ( !ok )
        {
            return nullptr;
        }

        return _settings.Create( 0u );
    }

    void KFOptionConfig::ReadSetting( KFNode& xmlnode, KFOption* kfsetting )
    {
        auto name = xmlnode.GetString( "Name" );
        auto key = xmlnode.GetString( "Key", true );
        auto value = xmlnode.GetString( "Value" );
        if ( name == "optionfile" )
        {
            _file_list.insert( value );
        }
        else
        {
            auto kfoption = _option_list.Create( OptionKey( name, key ) );
            kfoption->_str_value = value;
            kfoption->_uint32_value = KFUtility::ToValue<uint32>( value );
            kfoption->_double_value = KFUtility::ToValue<double>( value );
        }
    }

    bool KFOptionConfig::IsFile( const std::string& configfile, const std::string& file )
    {
        if ( _file_list.find( file ) != _file_list.end() )
        {
            return true;
        }

        return KFIntConfigT< KFOption >::IsFile( configfile, file );
    }

    void KFOptionConfig::LoadComplete( const std::string& file )
    {
        for ( auto& optionfile : _file_list )
        {
            auto filename = _confit_path + optionfile;

            try
            {
                __LOG_INFO__( "load [{}] start!", filename );
                LoadConfig( filename );
                __LOG_INFO__( "load [{}] ok!", filename );
            }
            catch ( ... )
            {
                __LOG_ERROR__( "load [{}] failed!", filename );
            }
        }
    }


    const KFOption* KFOptionConfig::FindOption( const std::string& name, const std::string& key ) const
    {
        auto kfoption = _option_list.Find( OptionKey( name, key ) );
        if ( kfoption == nullptr )
        {
            static KFOption _option;
            kfoption = &_option;
        }

        return kfoption;
    }
}