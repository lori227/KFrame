#include "KFTcpServerConfig.h"

namespace KFrame
{
    KFTcpSetting* KFTcpServerConfig::FindTcpSetting( const std::string& appname, const std::string& apptype )
    {
        for ( auto& kfdata : _tcp_setting_list )
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

    bool KFTcpServerConfig::LoadConfig()
    {
        _tcp_setting_list.clear();
        try
        {
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////

            auto tcpnode = config.FindNode( "TcpServer" );
            if ( tcpnode.IsValid() )
            {
                auto servernode = tcpnode.FindNode( "Server" );
                while ( servernode.IsValid() )
                {
                    KFTcpSetting kfsetting;
                    kfsetting._app_name = servernode.GetString( "AppName" );
                    kfsetting._app_type = servernode.GetString( "AppType" );
                    kfsetting._port_type = servernode.GetUInt32( "Type", true, 0 );
                    kfsetting._port = servernode.GetUInt32( "Port", true, 0 );
                    kfsetting._time_out = servernode.GetUInt32( "TimeOut", true, 20 );
                    kfsetting._max_queue_size = servernode.GetUInt32( "MaxQueue" );
                    _tcp_setting_list.push_back( kfsetting );

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