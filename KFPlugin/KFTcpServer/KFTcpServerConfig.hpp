#ifndef __KF_NET_CONFIG_H__
#define __KF_NET_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFTcpSetting : public KFIntSetting
    {
    public:
        std::string _app_name;
        std::string _app_type;
        std::string _local_ip = "0.0.0.0";

        uint32 _port = 0u;
        uint32 _port_type = 0u;
        uint32 _time_out = 20;
        uint32 _max_connection = 20000;
        uint32 _max_queue_size = 10000;
        uint32 _handle_message_count = 20; // 每帧处理消息数量
        uint32 _message_type = KFMessageEnum::Server;
        uint32 _compress_type = 0;
        uint32 _compress_level = 0u;
        uint32 _compress_length = 0u;
        bool _open_encrypt = false;
        std::string _encrypt_key;
    };

    class KFTcpServerConfig : public KFConfigT< KFTcpSetting >, public KFInstance< KFTcpServerConfig >
    {
    public:
        KFTcpServerConfig()
        {
            _key_name = "id";
            _file_name = "tcp";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFTcpSetting* kfsetting )
        {
            kfsetting->_app_name = xmlnode.ReadString( "appname" );
            kfsetting->_app_type = xmlnode.ReadString( "apptype" );
            kfsetting->_port_type = xmlnode.ReadUInt32( "porttype" );
            kfsetting->_port = xmlnode.ReadUInt32( "port" );
            kfsetting->_time_out = xmlnode.ReadUInt32( "timeout", true, 20 );
            kfsetting->_max_queue_size = xmlnode.ReadUInt32( "maxqueue" );
            kfsetting->_handle_message_count = xmlnode.ReadUInt32( "handlemessage", true, 500 );
            kfsetting->_message_type = xmlnode.ReadUInt32( "messagetype", true, KFMessageEnum::Server );
            kfsetting->_open_encrypt = xmlnode.ReadBoolen( "openencrypt" );
            kfsetting->_encrypt_key = xmlnode.ReadString( "encryptkey" );
            kfsetting->_compress_type = xmlnode.ReadUInt32( "compresstype" );
            kfsetting->_compress_level = xmlnode.ReadUInt32( "compresslevel" );
            kfsetting->_compress_length = xmlnode.ReadUInt32( "compresslength" );
        }
    };
}

#endif
