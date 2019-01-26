#include "KFOptionConfig.h"

namespace KFrame
{
    bool KFOptionConfig::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto channel = xmlnode.GetUInt32( "Channel" );
            auto service = xmlnode.GetUInt32( "Service" );
            if ( KFGlobal::Instance()->CheckChannelService( channel, service ) )
            {
                auto name = xmlnode.GetString( "Name" );
                auto key = xmlnode.GetString( "Key", true );
                auto value = xmlnode.GetString( "Value" );

                auto kfoption = _option_list.Create( OptionKey( name, key ) );
                kfoption->_str_value = value;
                kfoption->_uint32_value = KFUtility::ToValue<uint32>( value );
                kfoption->_double_value = KFUtility::ToValue<double>( value );
            }

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////
        return true;
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