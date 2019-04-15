#ifndef __KF_RANK_MASTER_CONFIG_H__
#define __KF_RANK_MASTER_CONFIG_H__

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
        }

    public:
        // 排行榜id
        uint32 _rank_id;

        // 刷新类型
        // 0 间隔时间 1 每小时  2 每天 3 每周 4 每月
        uint32 _refresh_type;

        // 刷新时间
        uint32 _refresh_time;

        // 刷新小时
        uint32 _refresh_hour;

        // 刷新分钟
        uint32 _refresh_minute;
    };

    class KFRankMasterConfig : public KFConfig, public KFSingleton< KFRankMasterConfig >
    {
    public:
        KFRankMasterConfig() = default;
        ~KFRankMasterConfig() = default;

        // 加载配置
        bool LoadConfig();

        // 查找排行榜设定
        const KFRankSetting* FindRankSetting( uint32 rankid ) const;

    public:
        // 排行榜配置列表
        KFHashMap< uint32, uint32, KFRankSetting > _kf_rank_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_rank_config = KFRankMasterConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif