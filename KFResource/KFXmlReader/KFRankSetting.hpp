#ifndef __KF_RANK_SETTING_H__
#define __KF_RANK_SETTING_H__

#include "KFZConfig/KFSetting.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    class KFCalcData
    {
    public:
        bool IsShowData() const
        {
            return !_text.empty();
        }

        bool IsCalcData() const
        {
            return _max_value != 0u;
        }

    public:
        std::string _parent_name;
        std::string _data_name;
        uint64 _max_value = 0u;
        std::string _text;
    };
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    class KFRankSetting : public KFIntSetting
    {
    public:
        // 父属性
        std::string _parent_name;

        // 排行榜属性
        std::string _data_name;

        // 分区类型 0 全区全服排行榜  1 分区排行榜
        uint32 _zone_type = 0u;

        // 最大数量
        uint32 _max_count = 0u;

        // 刷新类型
        // 0 间隔时间 1 每小时  2 每天 3 每周 4 每月
        uint32 _refresh_type = 0u;

        // 刷新时间
        uint32 _refresh_time = 0u;

        // 刷新小时
        uint32 _refresh_hour = 0u;

        // 刷新分钟
        uint32 _refresh_minute = 0u;

        // 是否清除数据
        bool _is_reset_data = false;

        // 计算属性
        std::vector< KFCalcData > _calc_data;
    };
}

#endif