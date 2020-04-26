#ifndef __KF_DATA_SHARD_CONFIG_H__
#define __KF_DATA_SHARD_CONFIG_H__

#include "KFrame.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFDatabaseSetting : public KFIntSetting
    {
    public:
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
    class KFDataShardConfig : public KFConfig, public KFInstance< KFDataShardConfig >
    {
    public:
        KFDataShardConfig()
        {
            _file_name = "datashard";
        }

        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask );

    public:
        // 数据库信息
        KFHashMap< uint64, uint64, KFDatabaseSetting > _settings;
    };
}

#endif