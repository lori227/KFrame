#ifndef __KF_RANK_SHARD_CONFIG_H__
#define __KF_RANK_SHARD_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFRankSetting
    {
    public:
        KFRankSetting()
        {
            _rank_id = 0;
            _zone_type = 0;
            _max_count = 0;
            _is_reset_data = false;
        }

    public:
        // 排行榜id
        uint32 _rank_id;

        // 父属性
        std::string _parent_data;

        // 排行榜属性
        std::string _rank_data;

        // 分区类型 0 全区全服排行榜  1 分区排行榜
        uint32 _zone_type;

        // 最大数量
        uint32 _max_count;

        // 是否清除数据
        bool _is_reset_data;
    };

    class KFRankShardConfig : public KFConfig, public KFSingleton< KFRankShardConfig >
    {
    public:
        KFRankShardConfig() = default;
        ~KFRankShardConfig() = default;

        // 加载配置
        bool LoadConfig();

        // 查找排行榜设定
        const KFRankSetting* FindRankSetting( uint32 rankid ) const;
    public:
        // 排行榜配置列表
        KFHashMap< uint32, uint32, KFRankSetting > _kf_rank_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_rank_config = KFRankShardConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif