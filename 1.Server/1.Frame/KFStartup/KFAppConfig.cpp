#include "KFAppConfig.h"
#include "Poco/File.h"

namespace KFrame
{
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
#if __KF_SYSTEM__ == __KF_WIN__
        _plugin_path = "./";
#else
        _plugin_path = "./bin/";
#endif

        std::string includefile;
        LoadStarupConfig( file, includefile );
        return true;
    }

    void KFAppConfig::LoadStarupConfig( const std::string& file, std::string& includefile )
    {
        KFXml kfxml( file );
        auto root = kfxml.FindNode( "Setting" );
        //////////////////////////////////////////////////////////////////////////
        ReadPluginSetting( root, includefile );
        if ( !includefile.empty() )
        {
            std::string subincludefile;
            LoadStarupConfig( includefile, subincludefile );
        }
    }

    void KFAppConfig::ReadPluginSetting( KFNode& root, std::string& includefile )
    {
        auto plugins = root.FindNode( "Plugins" );
        includefile = plugins.GetString( "Include", true );

        auto kfglobal = KFGlobal::Instance();

        auto node = plugins.FindNode( "Plugin" );
        while ( node.IsValid() )
        {
            auto channel = node.GetUInt32( "Channel", true, _invalid_int );
            if ( kfglobal->CheckChannelService( channel, _invalid_int ) )
            {
                KFAppSetting setting;
                setting._sort = node.GetUInt32( "Sort" );
                setting._name = node.GetString( "Name" );
                setting._config_file = node.GetString( "Config" );
                if ( !setting._config_file.empty() )
                {
                    auto configfile = __FORMAT__( setting._config_file, kfglobal->_app_id._union._app_data._channel_id, kfglobal->_service_type );

                    // 判断文件是否存在
                    Poco::File file( configfile );
                    if ( !file.exists() )
                    {
                        configfile = __FORMAT__( setting._config_file, kfglobal->_app_id._union._app_data._channel_id, _invalid_int );
                        Poco::File file( configfile );
                        if ( !file.exists() )
                        {
                            configfile = __FORMAT__( setting._config_file, _invalid_int, _invalid_int );
                        }
                    }

                    setting._config_file = configfile;
                }

                AddStartupSetting( setting );
            }

            node.NextNode();
        }
    }

}