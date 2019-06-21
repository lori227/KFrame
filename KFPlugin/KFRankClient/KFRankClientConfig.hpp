#ifndef __KF_RANK_CONFIG_H__
#define __KF_RANK_CONFIG_H__

#include "KFrame.h"

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
    class KFRankSetting
    {
    public:
        KFRankSetting()
        {
            _rank_id = 0;
            _zone_type = 0;
        }

    public:
        // 排行榜id
        uint32 _rank_id;

        // 父属性
        std::string _parent_name;

        // 排行榜属性
        std::string _data_name;

        // 分区类型 0 全区全服排行榜  1 分区排行榜
        uint32 _zone_type;

        // 计算属性
        std::vector< KFCalcData > _calc_data;
    };

    class KFRankClientConfig : public KFConfig, public KFSingleton< KFRankClientConfig >
    {
    public:
        KFRankClientConfig( const std::string& file )
            : KFConfig( file )
        {
        }

        // 加载配置
        void LoadConfig( const std::string& file );

        // 查找排行榜设定
        const KFRankSetting* FindRankSetting( uint32 rankid ) const;

        // 获得排行榜列表
        std::vector< KFRankSetting* >& FindRankSetting( const std::string& parentdata, const std::string& rankdata );

    public:
        // 玩家属性
        std::vector< KFCalcData > _player_data;

        // 排行榜配置列表
        KFHashMap< uint32, uint32, KFRankSetting > _kf_rank_setting;

        typedef std::pair< std::string, std::string > RankDataType;
        std::map< RankDataType, std::vector< KFRankSetting* > > _kf_rank_data_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_rank_config = KFRankClientConfig::Instance( "rank.xml" );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif