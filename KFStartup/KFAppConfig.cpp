#include "KFAppConfig.h"

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

        setting._sort = static_cast<uint32>( _startups.size() + 1 );
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
        try
        {
            ReadStartupConfig( file );
            return true;
        }
        catch ( std::exception& ex )
        {
            __LOG_INFO__( "load [{}] failed=[{}]", file, ex.what() );
        }

        return false;
    }

    void KFAppConfig::ReadStartupConfig( const std::string& file )
    {
        KFXml xml( file );
        auto root = xml.FindNode( "Setting" );
        //////////////////////////////////////////////////////////////////////////
        auto includes_node = root.FindNode( "Includes" );
        if ( includes_node.IsValid() )
        {
            auto include_node = includes_node.FindNode( "Include" );
            while ( include_node.IsValid() )
            {
                auto filename = include_node.ReadString( "File" );
                ReadStartupConfig( filename );
                include_node.NextNode();
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        auto plugins = root.FindNode( "Plugins" );
        auto global = KFGlobal::Instance();

        auto node = plugins.FindNode( "Plugin" );
        while ( node.IsValid() )
        {
            auto channel = node.ReadUInt32( "Channel", true, _invalid_int );
            if ( global->CheckChannelService( channel, _invalid_int ) )
            {
                KFAppSetting setting;
                setting._name = node.ReadString( "Name" );
                setting._param = node.ReadString( "Config", true );
                AddStartupSetting( setting );
            }

            node.NextNode();
        }
    }
}