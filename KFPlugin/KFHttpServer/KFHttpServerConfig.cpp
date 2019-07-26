#include "KFHttpServerConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFHttpServerConfig::LoadConfig( const std::string& file, uint32 loadmask )
    {
        _http_setting_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();
        auto httpserver = config.FindNode( "HttpServer" );
        if ( httpserver.IsValid() )
        {
            auto kepplive = httpserver.GetBoolen( "KeepAlive" );
            auto maxthread = httpserver.GetUInt32( "MaxThread" );
            auto maxqueue = httpserver.GetUInt32( "MaxQueue" );
            auto idletime = httpserver.GetUInt32( "IdleTime" );

            auto servernode = httpserver.FindNode( "Server" );
            while ( servernode.IsValid() )
            {
                KFHttpSetting kfsetting;

                kfsetting._app_name = servernode.GetString( "AppName" );
                kfsetting._app_type = servernode.GetString( "AppType" );
                kfsetting._port_type = servernode.GetUInt32( "Type" );
                kfsetting._port = servernode.GetUInt32( "Port" );
                kfsetting._keep_alive = servernode.GetBoolen( "KeepAlive", true, kepplive );
                kfsetting._max_thread = servernode.GetUInt32( "MaxThread", true, maxthread );
                kfsetting._max_queue = servernode.GetUInt32( "MaxQueue", true, maxqueue );
                kfsetting._idle_time = servernode.GetUInt32( "IdleTime", true, idletime );
                _http_setting_list.push_back( kfsetting );

                servernode.NextNode();
            }
        }
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