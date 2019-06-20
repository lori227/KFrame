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
            _port = 6379;
            _type = KFDatabaseEnum::Write;
        }

        // 名字
        std::string _name;

        // 类型
        uint32 _type;

        // ip
        std::string _ip;

        // port
        uint32 _port;

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
        KFRedisSetting* _kfseting;
    };

    class KFRedisType
    {
    public:
        // 查询redislist
        KFRedisList* FindRedisList( uint32 type );

        // 添加redislist
        KFRedisList* AddRedisList( uint32 type );

    public:
        uint32 _id;

        // Redis列表
        KFHashMap< uint32, uint32, KFRedisList > _redis_list;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRedisConfig : public KFConfig, public KFSingleton< KFRedisConfig >
    {
    public:
        KFRedisConfig( const std::string& file )
            : KFConfig( file )
        {
        }

        void LoadConfig( const std::string& file );

        // 查找redis配置
        KFRedisType* FindRedisType( const std::string& module, uint32 logicid );

    public:
        // 逻辑数据库映射
        typedef std::pair< std::string, uint32 > ModuleKey;
        KFMap< ModuleKey, const ModuleKey&, KFRedisType > _redis_type;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_redis_config = KFRedisConfig::Instance( "redis.setting" );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif