#ifndef __KF_REDIS_CONFIG_H__
#define __KF_REDIS_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFRedisConnectData
    {
    public:
        // ip
        std::string _ip;

        // port
        uint32 _port = 6379;
    };

    class KFRedisConnectOption
    {
    public:
        // 运行时id
        uint32 _runtime_id = 0u;

        // 最小逻辑id
        uint32 _min_logic_id = 0u;

        // 最大逻辑id
        uint32 _max_logic_id = 0u;

        // 密码
        std::string _password;

        // 写连接
        KFRedisConnectData _write_connect_data;

        // 读连接
        KFRedisConnectData _read_connect_data;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRedisSetting : public KFStrSetting
    {
    public:
        // redis 连接配置
        std::vector< KFRedisConnectOption > _connect_option;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRedisConfig : public KFConfigT<KFRedisSetting>, public KFInstance<KFRedisConfig>
    {
    public:
        KFRedisConfig()
        {
            _key_name = "name";
            _file_name = "redis";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFRedisSetting> setting )
        {
            KFRedisConnectOption option;
            option._min_logic_id = xml_node.ReadUInt32( "minid" );
            option._max_logic_id = xml_node.ReadUInt32( "maxid" );
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