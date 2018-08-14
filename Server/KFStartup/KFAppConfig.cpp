#include "KFAppConfig.h"

namespace KFrame
{
    KFAppConfig::KFAppConfig()
    {
        _server_id = 0;
    }

    KFAppConfig::~KFAppConfig()
    {

    }

    KFAppSetting* KFAppConfig::GetStartupSetting( const std::string& name )
    {
        auto iter = _startups.find( name );
        if ( iter == _startups.end() )
        {
            return nullptr;
        }

        return &iter->second;
    }

    void KFAppConfig::AddStartupSetting( KFAppSetting& setting )
    {
        _startups[ setting._name ] = setting;
    }

    bool KFAppConfig::LoadStartupConfig( const std::string& file )
    {
        auto ok = LoadServerConfig( file );
        if ( ok )
        {
            if ( !_common_startup_file.empty() )
            {
                ok = LoadCommonConfig( _common_startup_file );
            }
        }

        return ok;
    }

    bool KFAppConfig::LoadServerConfig( const std::string& file )
    {
        try
        {
            KFXml kfxml( file );
            auto root = kfxml.FindNode( "Setting" );

            // apptype
            _app_name = root.GetString( "AppName" );
            _app_type = root.GetString( "AppType" );
            _common_startup_file = root.GetString( "Common", true );
            //////////////////////////////////////////////////////////////////////////
            ReadPluginSetting( root );
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    bool KFAppConfig::LoadCommonConfig( const std::string& file )
    {
        try
        {
            KFXml kfxml( file );
            auto root = kfxml.FindNode( "Setting" );
            ReadPluginSetting( root );
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    void KFAppConfig::ReadPluginSetting( KFNode& root )
    {
        auto plugins = root.FindNode( "Plugins" );
        auto node = plugins.FindNode( "Plguin" );
        while ( node.IsValid() )
        {
            KFAppSetting setting;

            setting._name = node.GetString( "Name" );
            setting._starup = node.GetUInt32( "Startup" );
            setting._sort = node.GetUInt32( "Sort" );
            setting._debug = node.GetString( "Debug" );
            setting._release = node.GetString( "Release" );
            setting._config_file = node.GetString( "Config" );

            AddStartupSetting( setting );
            node.NextNode();
        }
    }

}