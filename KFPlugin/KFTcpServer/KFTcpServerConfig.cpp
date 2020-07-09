#include "KFTcpServerConfig.hpp"

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

    bool KFTcpServerConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        _tcp_setting_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();
        auto tcpnode = config.FindNode( "TcpServer" );
        if ( tcpnode.IsValid() )
        {
            _compress_type = tcpnode.GetUInt32( "CompressType" );
            _compress_level = tcpnode.GetUInt32( "CompressLevel" );
            _encrypt_key = tcpnode.GetString( "EncryptKey" );


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
                kfsetting._handle_message_count = servernode.GetUInt64( "HandleMessage", true, 500 );
                kfsetting._message_type = servernode.GetUInt32( "MessageType", true, KFMessageEnum::Server );
                kfsetting._open_encrypt = servernode.GetBoolen( "OpenEncrypt", true );
                kfsetting._compress_length = servernode.GetUInt32( "CompressLength", true );
                if ( kfsetting._compress_length == 0u )
                {
                    kfsetting._compress_length = __MAX_UINT32__;
                }

                _tcp_setting_list.emplace_back( kfsetting );

                servernode.NextNode();
            }
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }
}