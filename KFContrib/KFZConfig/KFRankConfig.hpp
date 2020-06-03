#ifndef __KF_RANK_CONFIG_H__
#define __KF_RANK_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"

namespace KFrame
{
    namespace KFRankEnum
    {
        enum MyEnum
        {
            TotalRank = 1,		// 全区全服排行榜
            ZoneRank = 2,		// 分区排行榜
            ////////////////////////////////////////////////////
            DataClearNone = 0,	// 不清空数据
            DataClearRank = 1,	// 只保留排行榜数据
            DataClearAll  = 2,	// 清除所有数据
        };
    }
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    class KFRandCalcData
    {
    public:
        std::string _parent_name;
        std::string _child_name;
        uint64 _max_value = 0u;
    };
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    class KFRankSetting : public KFIntSetting
    {
    public:
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

        // 清除数据类型
        uint32 _reset_data_type = 0u;

        // 计算属性
        std::vector< KFRandCalcData > _calc_data;
    };

    class KFRankConfig : public KFConfigT< KFRankSetting >, public KFInstance< KFRankConfig >
    {
    public:
        KFRankConfig()
        {
            _file_name = "rank";
        }

        // 获得排行榜列表
        std::vector< KFRankSetting* >& FindRankSetting( const std::string& parentname, const std::string& childname );

    protected:
        // 清除配置
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRankSetting* kfsetting );

    private:
        typedef std::pair< std::string, std::string > RankDataType;
        std::map< RankDataType, std::vector< KFRankSetting* > > _kf_rank_data_list;
    };
}

#endif