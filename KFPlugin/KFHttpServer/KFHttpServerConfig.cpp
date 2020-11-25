#include "KFHttpServerConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFHttpServerConfig::LoadConfig( const std::string& filepath, uint32 cleartype )
    {
        _http_setting_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();
        auto httpserver = config.FindNode( "HttpServer" );
        if ( httpserver.IsValid() )
        {
            auto kepplive = httpserver.ReadBoolen( "KeepAlive" );
            auto maxthread = httpserver.ReadUInt32( "MaxThread" );
            auto maxqueue = httpserver.ReadUInt32( "MaxQueue" );
            auto idletime = httpserver.ReadUInt32( "IdleTime" );

            auto servernode = httpserver.FindNode( "Server" );
            while ( servernode.IsValid() )
            {
                KFHttpSetting kfsetting;

                kfsetting._app_name = servernode.ReadString( "AppName" );
                kfsetting._app_type = servernode.ReadString( "AppType" );
                kfsetting._port_type = servernode.ReadUInt32( "Type" );
                kfsetting._port = servernode.ReadUInt32( "Port" );
                kfsetting._keep_alive = servernode.ReadBoolen( "KeepAlive", true, kepplive );
                kfsetting._max_thread = servernode.ReadUInt32( "MaxThread", true, maxthread );
                kfsetting._max_queue = servernode.ReadUInt32( "MaxQueue", true, maxqueue );
                kfsetting._idle_time = servernode.ReadUInt32( "IdleTime", true, idletime );
                _http_setting_list.push_back( kfsetting );

                servernode.NextNode();
            }
        }

        return true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    KFHttpSetting* KFHttpServerConfig::FindHttpSetting( const std::string& appname, const std::string& apptype )
    {
        for ( auto& kfsetting : _http_setting_list )
        {
            if ( kfsetting._app_name == appname &&
                    kfsetting._app_type == apptype )
            {
                return &kfsetting;
            }
        }

        return nullptr;
    }
}