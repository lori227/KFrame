#ifndef __KF_REDIS_CONFIG_H__
#define __KF_REDIS_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRedisSetting
    {
    public:
        KFRedisSetting()
        {
            _id = 0;
            _port = 6379;
        }

        uint32 _id;
        std::string _ip;
        uint32 _port;
        std::string _password;
    };

    class KFRedisConfig : public KFConfig, public KFSingleton< KFRedisConfig >
    {
    public:
        KFRedisConfig();
        ~KFRedisConfig();

        bool LoadConfig( const char* file );

        // 查找redis配置
        const KFRedisSetting* FindRedisSetting( uint32 id ) const;
        const KFRedisSetting* FindRedisSetting( const std::string& field, uint32 logicid ) const;

    protected:
        // 添加redis配置
        void AddRedisSetting( KFRedisSetting* kfsetting );

        // 查找redis
        uint32 FindLogicRedisId( const std::string& filed, uint32 logicid ) const;
    public:
        // 连接列表
        KFMap< uint32, uint32, KFRedisSetting > _redis_setting;

        // 默认数据库
        uint32 _default_redis_id;

        // 逻辑数据库映射
        typedef std::pair< std::string, uint32 > LogicKey;
        std::map< LogicKey, uint32 > _logic_redis_map;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_redis_config = KFRedisConfig::Instance();
}

#endif