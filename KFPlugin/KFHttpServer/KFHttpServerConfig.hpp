#ifndef __KF_HTTP_SERVER_CONFIG_H__
#define __KF_HTTP_SERVER_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFHttpSetting : public KFIntSetting
    {
    public:
        // 服务器名字
        std::string _app_name;

        // 服务器类型
        std::string _app_type;

        // 内网ip
        std::string _local_ip = "0.0.0.0";

        // 端口类型
        uint32 _port_type = 0u;

        // 端口
        uint32 _port = 0u;

        // 是否保持
        bool _keep_alive = false;

        // 最大线程
        uint32 _max_thread = 4;

        // 最大队列
        uint32 _max_queue = 64u;

        // 线程idle时间
        uint32 _idle_time = 100u;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFHttpServerConfig : public KFConfigT<KFHttpSetting>, public KFInstance<KFHttpServerConfig>
    {
    public:
        KFHttpServerConfig()
        {
            _key_name = "id";
            _file_name = "http";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFHttpSetting> setting )
        {
            setting->_app_name = xml_node.ReadString( "appname" );
            setting->_app_type = xml_node.ReadString( "apptype" );
            setting->_port_type = xml_node.ReadUInt32( "porttype" );
            setting->_port = xml_node.ReadUInt32( "port" );
            setting->_keep_alive = xml_node.ReadBool( "keepalive" );
            setting->_max_thread = xml_node.ReadUInt32( "maxthread" );
            setting->_max_queue = xml_node.ReadUInt32( "maxqueue" );
            setting->_idle_time = xml_node.ReadUInt32( "idletime" );
        }
    };
}

#endif
