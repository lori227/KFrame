#include "KFAppConfig.h"

namespace KFrame
{
    KFAppConfig::KFAppConfig()
    {

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

        if ( _app_name.empty() )
        {
            _app_name = plugins.GetString( "AppName" );
        }

        if ( _app_type.empty() )
        {
            _app_type = plugins.GetString( "AppType" );
        }

        if ( _plugin_path.empty() )
        {
#if __KF_SYSTEM__ == __KF_WIN__
            _plugin_path = plugins.GetString( "WinPluginPath" );
#else
            _plugin_path = plugins.GetString( "LinuxPluginPath" );
#endif
        }

        if ( _common_startup_file.empty() )
        {
            _common_startup_file = plugins.GetString( "Common", true );
        }

        auto node = plugins.FindNode( "Plugin" );
        while ( node.IsValid() )
        {
            auto channel = node.GetUInt32( "Channel", true, _invalid_int );
            if ( KFGlobal::Instance()->CheckChannelService( channel, _invalid_int ) )
            {
                KFAppSetting setting;
                setting._sort = node.GetUInt32( "Sort" );
                setting._name = node.GetString( "Name" );
                setting._config_file = node.GetString( "Config" );
                AddStartupSetting( setting );
            }

            node.NextNode();
        }
    }

}