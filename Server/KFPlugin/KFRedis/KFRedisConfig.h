#ifndef __KF_REDIS_CONFIG_H__
#define __KF_REDIS_CONFIG_H__

#include "KFrame.h"
#include "KFConHash/KFConHash.h"
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

    class KFRedisList
    {
    public:
        // 清空
        void Reset();

        const KFRedisSetting* FindSetting();
        void AddSetting( KFRedisSetting& kfsetting );

    private:
        KFConHash _redis_hash;
        std::vector< KFRedisSetting > _redis_list;

        // 返回的连接设置
        KFRedisSetting* _kfseting;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////

    class KFRedisConfig : public KFConfig, public KFSingleton< KFRedisConfig >
    {
    public:
        KFRedisConfig() = default;
        ~KFRedisConfig() = default;

        bool LoadConfig();

        // 查找redis配置
        const KFRedisSetting* FindRedisSetting( const std::string& module, uint32 logicid );
    public:
        // 逻辑数据库映射
        typedef std::pair< std::string, uint32 > ModuleKey;
        KFMap< ModuleKey, const ModuleKey&, KFRedisList > _redis_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_redis_config = KFRedisConfig::Instance();
}

#endif