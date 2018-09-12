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
            _refresh_type = 0;
            _refresh_time = 0;
            _refresh_hour = 0;
            _refresh_minute = 0;
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

        // 刷新类型
        // 0 间隔时间 1 每小时  2 每天 3 每周 4 每月
        uint32 _refresh_type;

        // 刷新时间
        uint32 _refresh_time;

        // 刷新小时
        uint32 _refresh_hour;

        // 刷新分钟
        uint32 _refresh_minute;

        // 最大数量
        uint32 _max_count;

        // 显示属性
        VectorString _show_data;

        // 是否清除数据
        bool _is_reset_data;
    };

    class KFRankShardConfig : public KFConfig, public KFSingleton< KFRankShardConfig >
    {
    public:
        KFRankShardConfig();
        ~KFRankShardConfig();

        // 加载配置
        bool LoadConfig();

        // 查找排行榜设定
        const KFRankSetting* FindRankSetting( uint32 rankid ) const;

    protected:
        // 添加ranksetting
        void AddRankSetting( KFRankSetting* kfsetting );

    public:

        // 玩家属性
        VectorString _player_data;

        // 排行榜配置列表
        KFMap< uint32, uint32, KFRankSetting > _kf_rank_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_rank_config = KFRankShardConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif