#ifndef __KF_RANK_CONFIG_H__
#define __KF_RANK_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    typedef std::pair< std::string, std::string > DataType;

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
        std::string _parent_data;

        // 排行榜属性
        std::string _rank_data;

        // 分区类型 0 全区全服排行榜  1 分区排行榜
        uint32 _zone_type;

        // 计算属性
        std::vector< DataType > _calc_data;

        // 显示属性
        std::vector< DataType > _show_data;
    };

    class KFRankClientConfig : public KFConfig, public KFSingleton< KFRankClientConfig >
    {
    public:
        KFRankClientConfig();
        ~KFRankClientConfig();

        // 加载配置
        bool LoadConfig();

        // 查找排行榜设定
        const KFRankSetting* FindRankSetting( uint32 rankid ) const;

        // 获得排行榜列表
        std::vector< KFRankSetting* >& FindRankSetting( const std::string& parentdata, const std::string& rankdata );

    protected:
        // 添加ranksetting
        void AddRankSetting( KFRankSetting* kfsetting );
    public:
        // 玩家属性
        std::vector< DataType > _player_data;

        // 排行榜配置列表
        KFMap< uint32, uint32, KFRankSetting > _kf_rank_setting;

        typedef std::pair< std::string, std::string > RankDataType;
        std::map< RankDataType, std::vector< KFRankSetting* > > _kf_rank_data_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_rank_config = KFRankClientConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif