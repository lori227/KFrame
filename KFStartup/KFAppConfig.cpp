#include "KFAppConfig.h"
#include "Poco/File.h"

namespace KFrame
{
    std::string KFAppSetting::GetName() const
    {
        auto name = _name;
#ifdef __KF_DEBUG__
        name += "d";
#endif
        return name;
    }

    void KFAppConfig::AddStartupSetting( KFAppSetting& setting )
    {
        for ( auto& startup : _startups )
        {
            if ( startup._name == setting._name )
            {
                return;
            }
        }

        setting._sort = static_cast< uint32 >( _startups.size() + 1 );
        _startups.push_back( setting );
    }


    const KFAppSetting* KFAppConfig::FindStartupSetting( const std::string& name )
    {
        for ( auto& startup : _startups )
        {
            if ( startup._name == name )
            {
                return &startup;
            }
        }

        return nullptr;
    }

    bool KFAppConfig::LoadStartupConfig( const std::string& file )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        _plugin_path = "./";
#else
        _plugin_path = "./bin/";
#endif

        ReadStartupConfig( file );
        return true;
    }

    void KFAppConfig::ReadStartupConfig( const std::string& file )
    {
        KFXml kfxml( file );
        auto root = kfxml.FindNode( "Setting" );
        //////////////////////////////////////////////////////////////////////////
        auto includesnode = root.FindNode( "Includes" );
        if ( includesnode.IsValid() )
        {
            auto includenode = includesnode.FindNode( "Include" );
            while ( includenode.IsValid() )
            {
                auto filename = includenode.GetString( "File" );
                ReadStartupConfig( filename );
                includenode.NextNode();
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        auto plugins = root.FindNode( "Plugins" );
        auto kfglobal = KFGlobal::Instance();

        auto node = plugins.FindNode( "Plugin" );
        while ( node.IsValid() )
        {
            auto channel = node.GetUInt32( "Channel", true, _invalid_int );
            if ( kfglobal->CheckChannelService( channel, _invalid_int ) )
            {
                KFAppSetting setting;
                setting._name = node.GetString( "Name" );
                setting._config_file = node.GetString( "Config" );
                if ( !setting._config_file.empty() )
                {
                    auto configfile = __FORMAT__( setting._config_file, kfglobal->_channel, kfglobal->_service );

                    // 判断文件是否存在
                    Poco::File file( configfile );
                    if ( !file.exists() )
                    {
                        configfile = __FORMAT__( setting._config_file, kfglobal->_channel, _invalid_int );
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