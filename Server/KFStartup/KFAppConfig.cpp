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

        if ( plugins.GetString( "AppName" ) != _invalid_str )
        {
            _app_name = plugins.GetString( "AppName" );
        }

        if ( plugins.GetString( "AppType" ) != _invalid_str )
        {
            _app_type = plugins.GetString( "AppType" );
        }

        if ( plugins.GetString( "Common", true ) != _invalid_str )
        {
            _common_startup_file = plugins.GetString( "Common", true );
        }

        auto node = plugins.FindNode( "Plugin" );
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