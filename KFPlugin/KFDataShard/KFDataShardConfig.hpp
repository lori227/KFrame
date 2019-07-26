#ifndef __KF_DATA_SHARD_CONFIG_H__
#define __KF_DATA_SHARD_CONFIG_H__

#include "KFrame.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFDatabaseSetting : public KFStrSetting
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

        void LoadConfig( const std::string& file, uint32 loadmask );

    public:
        // 数据库信息
        KFHashMap< std::string, const std::string&, KFDatabaseSetting > _settings;
    };
}

#endif