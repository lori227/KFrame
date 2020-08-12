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
        uint64 _key = 0u;
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

        // 刷新的时间id
        uint32 _refresh_time_id = 0u;

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
        std::vector< KFRankSetting* >& FindRankSetting( const std::string& parentname, uint64 key, const std::string& childname );

    protected:
        // 清除配置
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRankSetting* kfsetting );

    public:
        // 显示的数据
        StringSet _show_data_list;

    private:
        std::map< std::string, std::vector< KFRankSetting* > > _kf_rank_data_list;
    };
}

#endif