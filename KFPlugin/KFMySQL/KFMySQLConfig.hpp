#ifndef __KF_MYSQL_CONFIG_H__
#define __KF_MYSQL_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFMySQLConnnectData
    {
    public:
        // ip
        std::string _ip;

        // port
        uint32 _port = 3306;
    };

    class KFMySQLConnectOption
    {
    public:
        // 运行时id
        uint32 _runtime_id = 0u;

        // 最小逻辑id
        uint32 _min_logic_id = 0u;

        // 最大逻辑id
        uint32 _max_logic_id = 0u;

        // 数据库
        std::string _database;

        // 账号
        std::string _user;

        // 密码
        std::string _password;

        // 写连接
        KFMySQLConnnectData _write_connect_data;

        // 读连接
        KFMySQLConnnectData _read_connect_data;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFMySQLSetting : public KFStrSetting
    {
    public:
        // redis 连接配置
        std::vector< KFMySQLConnectOption > _connect_option;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFMySQLConfig : public KFConfigT< KFMySQLSetting >, public KFInstance< KFMySQLConfig >
    {
    public:
        KFMySQLConfig()
        {
            _key_name = "name";
            _file_name = "mysql";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFMySQLSetting* kfsetting )
        {
            KFMySQLConnectOption option;
            option._min_logic_id = xmlnode.ReadUInt32( "minid" );
            option._max_logic_id = xmlnode.ReadUInt32( "maxid" );
            option._database = xmlnode.ReadString( "database" );
            option._user = xmlnode.ReadString( "user" );
            option._password = xmlnode.ReadString( "password" );

            option._write_connect_data._ip = xmlnode.ReadString( "writeip" );
            option._write_connect_data._port = xmlnode.ReadUInt32( "writeport" );

            option._read_connect_data._ip = xmlnode.ReadString( "readip" );
            option._read_connect_data._port = xmlnode.ReadUInt32( "readport" );

            option._runtime_id = ( uint32 )kfsetting->_connect_option.size() + 1u;
            kfsetting->_connect_option.push_back( option );
        }
    };
}

#endif