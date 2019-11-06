#ifndef __KF_RANK_CONFIG_H__
#define __KF_RANK_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFRankSetting.hpp"

namespace KFrame
{
    class KFRankConfig : public KFConfig, public KFInstance< KFRankConfig >
    {
    public:
        KFRankConfig()
        {
            _file_name = "rank";
        }

        // 加载配置
        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask );

        // 查找排行榜设定
        const KFRankSetting* FindSetting( uint32 rankid ) const;

        // 获得排行榜列表
        std::vector< KFRankSetting* >& FindSetting( const std::string& parentdata, const std::string& rankdata );

    public:
        // 玩家属性
        std::vector< KFCalcData > _player_data;

        // 排行榜配置列表
        KFHashMap< uint32, uint32, KFRankSetting > _kf_rank_setting;

        typedef std::pair< std::string, std::string > RankDataType;
        std::map< RankDataType, std::vector< KFRankSetting* > > _kf_rank_data_list;
    };
}

#endif