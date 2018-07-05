#include "KFTcpServerConfig.h"

namespace KFrame
{
    KFTcpSetting* KFTcpServerConfig::FindTcpSetting( const std::string& appname, const std::string& apptype )
    {
        for ( auto& kfdata : _tcp_data_list )
        {
            if ( kfdata._app_name == appname &&
                    kfdata._app_type == apptype )
            {
                return &kfdata;
            }
        }

        return nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFTcpServerConfig::LoadConfig( const char* file )
    {
        _tcp_data_list.clear();
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////

            auto tcpnode = config.FindNode( "TcpServer" );
            if ( tcpnode.IsValid() )
            {
                auto maxqueue = tcpnode.GetUInt32( "MaxQueue" );

                auto servernode = tcpnode.FindNode( "Server" );
                while ( servernode.IsValid() )
                {
                    KFTcpSetting kfdata;

                    kfdata._app_name = servernode.GetString( "AppName" );
                    kfdata._app_type = servernode.GetString( "AppType" );
                    kfdata._interanet_ip = servernode.GetString( "InteranetIp" );
                    kfdata._port_type = servernode.GetUInt32( "Type", true, 0 );
                    kfdata._port = servernode.GetUInt32( "Port", true, 0 );
                    kfdata._time_out = servernode.GetUInt32( "TimeOut", true, 20 );
                    kfdata._max_queue_size = servernode.GetUInt32( "MaxQueue", true, maxqueue );

                    _tcp_data_list.push_back( kfdata );

                    servernode.NextNode();
                }
            }
            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}