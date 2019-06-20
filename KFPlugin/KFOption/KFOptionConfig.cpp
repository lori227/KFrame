#include "KFOptionConfig.hpp"

namespace KFrame
{
    KFOption* KFOptionConfig::CreateSetting( KFNode& xmlnode )
    {
        auto service = xmlnode.GetUInt32( "Service" );
        auto channel = xmlnode.GetUInt32( "Channel" );
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

        auto kfoption = _option_list.Create( OptionKey( name, key ) );
        kfoption->_str_value = value;
        kfoption->_uint32_value = KFUtility::ToValue<uint32>( value );
        kfoption->_double_value = KFUtility::ToValue<double>( value );
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