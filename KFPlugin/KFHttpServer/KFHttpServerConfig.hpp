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
    class KFHttpServerConfig : public KFConfigT< KFHttpSetting >, public KFInstance< KFHttpServerConfig >
    {
    public:
        KFHttpServerConfig()
        {
            _key_name = "id";
            _file_name = "http";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFHttpSetting* kfsetting )
        {
            kfsetting->_app_name = xmlnode.ReadString( "appname" );
            kfsetting->_app_type = xmlnode.ReadString( "apptype" );
            kfsetting->_port_type = xmlnode.ReadUInt32( "porttype" );
            kfsetting->_port = xmlnode.ReadUInt32( "port" );
            kfsetting->_keep_alive = xmlnode.ReadBool( "keepalive" );
            kfsetting->_max_thread = xmlnode.ReadUInt32( "maxthread" );
            kfsetting->_max_queue = xmlnode.ReadUInt32( "maxqueue" );
            kfsetting->_idle_time = xmlnode.ReadUInt32( "idletime" );
        }
    };
}

#endif
