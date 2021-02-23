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

    class KFTcpServerConfig : public KFConfigT<KFTcpSetting>, public KFInstance<KFTcpServerConfig>
    {
    public:
        KFTcpServerConfig()
        {
            _key_name = "id";
            _file_name = "tcp";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFTcpSetting> setting )
        {
            setting->_app_name = xml_node.ReadString( "appname" );
            setting->_app_type = xml_node.ReadString( "apptype" );
            setting->_port_type = xml_node.ReadUInt32( "porttype" );
            setting->_port = xml_node.ReadUInt32( "port" );
            setting->_time_out = xml_node.ReadUInt32( "timeout", true, 20 );
            setting->_max_queue_size = xml_node.ReadUInt32( "maxqueue" );
            setting->_handle_message_count = xml_node.ReadUInt32( "handlemessage", true, 500 );
            setting->_message_type = xml_node.ReadUInt32( "messagetype", true, KFMessageEnum::Server );
            setting->_open_encrypt = xml_node.ReadBool( "openencrypt" );
            setting->_encrypt_key = xml_node.ReadString( "encryptkey" );
            setting->_compress_type = xml_node.ReadUInt32( "compresstype" );
            setting->_compress_level = xml_node.ReadUInt32( "compresslevel" );
            setting->_compress_length = xml_node.ReadUInt32( "compresslength" );
        }
    };
}

#endif
