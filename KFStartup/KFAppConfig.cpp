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
        KFXml kfxml( file );
        auto root = kfxml.FindNode( "Setting" );
        //////////////////////////////////////////////////////////////////////////
        auto includesnode = root.FindNode( "Includes" );
        if ( includesnode.IsValid() )
        {
            auto includenode = includesnode.FindNode( "Include" );
            while ( includenode.IsValid() )
            {
                auto filename = includenode.ReadString( "File" );
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
            auto channel = node.ReadUInt32( "Channel", true, _invalid_int );
            if ( kfglobal->CheckChannelService( channel, _invalid_int ) )
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