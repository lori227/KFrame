#ifndef __KF_DATA_SHARD_CONFIG_H__
#define __KF_DATA_SHARD_CONFIG_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDatabaseSetting
    {
    public:
        // 名字
        std::string _name;

        // 类型
        uint32 _save_flag = 0;

        // 优先级
        uint32 _sort = 0u;

        // 是否打开
        bool _is_open = false;

        // 缓存时间
        uint32 _cache_time = 2592000;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFDataShardConfig : public KFConfig, public KFSingleton< KFDataShardConfig >
    {
    public:
        KFDataShardConfig( const std::string& file )
            : KFConfig( file )
        {
        }

        void LoadConfig( const std::string& file );

    public:
        // 数据库信息
        KFHashMap< std::string, const std::string&, KFDatabaseSetting > _settings;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_data_shard_config = KFDataShardConfig::Instance( "datashard.setting" );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif