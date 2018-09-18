#include "KFOptionConfig.h"

namespace KFrame
{
    bool KFOptionConfig::LoadConfig()
    {
        _option_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto name = xmlnode.GetString( "Name" );
            auto key = xmlnode.GetString( "Key", true );
            auto value = xmlnode.GetString( "Value" );

            auto channel = xmlnode.GetUInt32( "Channel" );
            if ( channel == _invalid_int || channel == KFGlobal::Instance()->_app_channel )
            {
                OptionKey optionkey( name, key );
                _option_list[ optionkey ] = value;
            }

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }

    const std::string& KFOptionConfig::FindOption( const std::string& name, const std::string& key ) const
    {
        OptionKey optionkey( name, key );
        auto iter = _option_list.find( optionkey );
        if ( iter == _option_list.end() )
        {
            return _invalid_str;
        }

        return iter->second;
    }
}
