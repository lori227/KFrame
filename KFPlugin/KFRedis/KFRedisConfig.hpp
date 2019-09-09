#ifndef __KF_REDIS_CONFIG_H__
#define __KF_REDIS_CONFIG_H__

#include "KFrame.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRedisSetting : public KFIntSetting
    {
    public:
        // 名字
        std::string _module;

        // ip
        std::string _ip;

        // port
        uint32 _port = 6379;

        // 密码
        std::string _password;
    };

    class KFRedisList
    {
    public:
        // 清空
        void Reset();

        const KFRedisSetting* FindSetting();
        void AddSetting( KFRedisSetting& kfsetting );

    private:
        std::vector< KFRedisSetting > _redis_list;

        // 返回的连接设置
        KFRedisSetting* _kf_seting = nullptr;
    };

    class KFRedisType
    {
    public:
        // 查询redislist
        KFRedisList* FindRedisList( uint32 type );

        // 添加redislist
        KFRedisList* AddRedisList( uint32 type );

    public:
        uint32 _id = 0;

        // Redis列表
        KFHashMap< uint32, uint32, KFRedisList > _redis_list;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRedisConfig : public KFConfig, public KFInstance< KFRedisConfig >
    {
    public:
        KFRedisConfig()
        {
            _file_name = "redis";
        }

        bool LoadConfig( const std::string& file, uint32 loadmask );

        // 查找redis配置
        KFRedisType* FindRedisType( const std::string& module, uint32 logicid );

    public:
        // 逻辑数据库映射
        typedef std::pair< std::string, uint32 > ModuleKey;
        KFMap< ModuleKey, const ModuleKey&, KFRedisType > _redis_type;
    };
}

#endif