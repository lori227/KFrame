#include "KFOptionConfig.h"


namespace KFrame
{
    KFOptionConfig::KFOptionConfig()
    {
    }

    KFOptionConfig::~KFOptionConfig()
    {
    }

    bool KFOptionConfig::LoadConfig()
    {
        try
        {
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto xmlnode = config.FindNode( "Option" );
            while ( xmlnode.IsValid() )
            {
                auto name = xmlnode.GetString( "Name" );
                auto key = xmlnode.GetUInt32( "Key", true );
                auto value = xmlnode.GetString( "Value" );

                OptionKey optionkey( name, key );
                _option_list[ optionkey ] = value;

                xmlnode.NextNode();
            }

            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }


    const std::string& KFOptionConfig::FindOption( const std::string& name, uint32 key ) const
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
