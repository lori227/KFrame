#ifndef __KF_RANK_CONFIG_H__
#define __KF_RANK_CONFIG_H__

#include "KFConfig.h"


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
        std::vector<KFRandCalcData> _calc_data;
    };

    class KFRankConfig : public KFConfigT<KFRankSetting>, public KFInstance<KFRankConfig>
    {
    public:
        KFRankConfig()
        {
            _file_name = "rank";
        }

        // 获得排行榜列表
        std::vector<std::shared_ptr<const KFRankSetting>>& FindRankSetting( const std::string& parent_name, uint64 key, const std::string& child_name )
        {
            static std::vector<std::shared_ptr<const KFRankSetting>> _empty_list;
            auto key_name = __FORMAT__( "{}:{}:{}", parent_name, key, child_name );
            auto iter = _rank_data_list.find( key_name );
            if ( iter == _rank_data_list.end() )
            {
                return _empty_list;
            }

            return iter->second;
        }

    protected:
        // 清除配置
        virtual void ClearSetting()
        {
            _show_data_list.clear();
            _rank_data_list.clear();
            KFConfigT<KFRankSetting>::ClearSetting();
        }

        // 读取配置
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFRankSetting> setting )
        {
            setting->_zone_type = xml_node.ReadUInt32( "zonetype", true );
            setting->_max_count = xml_node.ReadUInt32( "maxcount", true );
            setting->_refresh_time_id = xml_node.ReadUInt32( "refreshtimeid", true );
            setting->_reset_data_type = xml_node.ReadUInt32( "refreshreset", true );

            auto calc_data = xml_node.ReadString( "calcdata", true );
            while ( !calc_data.empty() )
            {
                auto data = KFUtility::SplitString( calc_data, __SPLIT_STRING__ );
                if ( !data.empty() )
                {
                    KFRandCalcData rank_calc_data;
                    rank_calc_data._parent_name = KFUtility::SplitString( data, __DOMAIN_STRING__ );
                    rank_calc_data._key = KFUtility::SplitValue<uint64>( data, __DOMAIN_STRING__ );
                    rank_calc_data._child_name = KFUtility::SplitString( data, __DOMAIN_STRING__ );
                    rank_calc_data._max_value = KFUtility::SplitValue<uint32>( data, __DOMAIN_STRING__ );
                    setting->_calc_data.emplace_back( rank_calc_data );

                    auto key_name = __FORMAT__( "{}:{}:{}", rank_calc_data._parent_name, rank_calc_data._key, rank_calc_data._child_name );
                    _rank_data_list[key_name].push_back( setting );
                }
            }

            if ( _show_data_list.empty() )
            {
                _show_data_list = xml_node.ReadStringSet( "showdata", true );
            }
        }

    public:
        // 显示的数据
        StringSet _show_data_list;

    private:
        std::map<std::string, std::vector<std::shared_ptr<const KFRankSetting>>> _rank_data_list;
    };
}

#endif