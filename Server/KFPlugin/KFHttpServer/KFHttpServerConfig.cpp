#include "KFHttpServerConfig.h"

namespace KFrame
{
    KFHttpServerConfig::KFHttpServerConfig()
    {
        _port = 0;
        _keep_alive = false;
        _max_thread = 0;
        _max_queue = 0;
        _idle_time = 0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFHttpServerConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            auto httpserver = config.FindNode( "HttpServer" );
            _local_ip = httpserver.GetString( "LocalIp" );
            _interanet_ip = httpserver.GetString( "InteranetIp" );
            _port = httpserver.GetUInt32( "Port" );
            _keep_alive = httpserver.GetBoolen( "KeepAlive" );
            _max_thread = httpserver.GetUInt32( "MaxThread" );
            _max_queue = httpserver.GetUInt32( "MaxQueue" );
            _idle_time = httpserver.GetUInt32( "IdleTime" );
            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

}