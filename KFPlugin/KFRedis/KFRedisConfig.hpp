#ifndef __KF_REDIS_CONFIG_H__
#define __KF_REDIS_CONFIG_H__

#include "KFrame.h"
#include "KFConfig.h"

namespace KFrame
{
    class KFRedisConnnectData
    {
    public:
        // ip
        std::string _ip;

        // port
        uint32 _port = 6379;

        // 密码
        std::string _password;
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

        KFRedisConnnectData _write_connect_data;
        KFRedisConnnectData _read_connect_data;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRedisSetting : public KFStrSetting
    {
    public:
        // redis 连接配置
        std::vector< KFRedisConnectOption > _redis_connect_option;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRedisConfig : public KFConfigT< KFRedisSetting >, public KFInstance< KFRedisConfig >
    {
    public:
        KFRedisConfig()
        {
            _key_name = "name";
            _file_name = "redis";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFRedisSetting* kfsetting )
        {
            KFRedisConnectOption option;
            option._min_logic_id = xmlnode.ReadUInt32( "minid" );
            option._max_logic_id = xmlnode.ReadUInt32( "maxid" );

            option._write_connect_data._ip = xmlnode.ReadString( "writeip" );
            option._write_connect_data._port = xmlnode.ReadUInt32( "writeport" );
            option._write_connect_data._password = xmlnode.ReadString( "writepassword" );

            option._read_connect_data._ip = xmlnode.ReadString( "readip" );
            option._read_connect_data._port = xmlnode.ReadUInt32( "readport" );
            option._read_connect_data._password = xmlnode.ReadString( "readpassword" );

            option._runtime_id = ( uint32 )kfsetting->_redis_connect_option.size() + 1u;
            kfsetting->_redis_connect_option.push_back( option );
        }
    };
}

#endif