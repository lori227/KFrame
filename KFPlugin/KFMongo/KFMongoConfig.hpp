#ifndef __KF_MONGO_CONFIG_H__
#define __KF_MONGO_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFMongoConnnectData
    {
    public:
        // ip
        std::string _ip;

        // port
        uint32 _port = 27017;
    };

    class KFMongoConnectOption
    {
    public:
        // 运行时id
        uint32 _runtime_id = 0u;

        // 最小逻辑id
        uint32 _min_logic_id = 0u;

        // 最大逻辑id
        uint32 _max_logic_id = 0u;

        // 用户名
        std::string _user;

        // 密码
        std::string _password;

        // 数据库名
        std::string _database;

        // 连接超时时间
        uint32 _connect_timeout = 10000;

        // 执行超时时间
        uint32 _execute_timeout = 10000;

        // 是否使用ssl
        std::string _use_ssl;

        // 认证方式
        std::string _auth_type;

        // 写连接
        KFMongoConnnectData _write_connect_data;

        // 读连接
        KFMongoConnnectData _read_connect_data;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFMongoSetting : public KFStrSetting
    {
    public:
        // redis 连接配置
        std::vector< KFMongoConnectOption > _connect_option;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFMongoConfig : public KFConfigT< KFMongoSetting >, public KFInstance< KFMongoConfig >
    {
    public:
        KFMongoConfig()
        {
            _key_name = "name";
            _file_name = "mongo";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xml_node, KFMongoSetting* setting )
        {
            KFMongoConnectOption option;
            option._min_logic_id = xml_node.ReadUInt32( "minid" );
            option._max_logic_id = xml_node.ReadUInt32( "maxid" );
            option._connect_timeout = xml_node.ReadUInt32( "connecttimeout", true, 10000 );
            option._execute_timeout = xml_node.ReadUInt32( "executetimeout", true, 10000 );
            option._use_ssl = xml_node.ReadString( "usessl" );
            option._auth_type = xml_node.ReadString( "auth" );

            option._database = xml_node.ReadString( "database" );
            option._user = xml_node.ReadString( "user" );
            option._password = xml_node.ReadString( "password" );

            option._write_connect_data._ip = xml_node.ReadString( "writeip" );
            option._write_connect_data._port = xml_node.ReadUInt32( "writeport" );

            option._read_connect_data._ip = xml_node.ReadString( "readip" );
            option._read_connect_data._port = xml_node.ReadUInt32( "readport" );

            option._runtime_id = ( uint32 )setting->_connect_option.size() + 1u;
            setting->_connect_option.push_back( option );
        }
    };
}

#endif