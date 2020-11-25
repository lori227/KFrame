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

    bool KFTcpServerConfig::LoadConfig( const std::string& filepath, uint32 cleartype )
    {
        _tcp_setting_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();
        auto tcpnode = config.FindNode( "TcpServer" );
        if ( tcpnode.IsValid() )
        {
            _compress_type = tcpnode.ReadUInt32( "CompressType" );
            _compress_level = tcpnode.ReadUInt32( "CompressLevel" );
            _encrypt_key = tcpnode.ReadString( "EncryptKey" );


            auto servernode = tcpnode.FindNode( "Server" );
            while ( servernode.IsValid() )
            {
                KFTcpSetting kfsetting;
                kfsetting._app_name = servernode.ReadString( "AppName" );
                kfsetting._app_type = servernode.ReadString( "AppType" );
                kfsetting._port_type = servernode.ReadUInt32( "Type", true, 0 );
                kfsetting._port = servernode.ReadUInt32( "Port", true, 0 );
                kfsetting._time_out = servernode.ReadUInt32( "TimeOut", true, 20 );
                kfsetting._max_queue_size = servernode.ReadUInt32( "MaxQueue" );
                kfsetting._handle_message_count = servernode.ReadUInt32( "HandleMessage", true, 500 );
                kfsetting._message_type = servernode.ReadUInt32( "MessageType", true, KFMessageEnum::Server );
                kfsetting._open_encrypt = servernode.ReadBoolen( "OpenEncrypt", true );
                kfsetting._compress_length = servernode.ReadUInt32( "CompressLength", true );
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