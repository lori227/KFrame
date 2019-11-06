#include "KFRankConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRankConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        _player_data.clear();
        _kf_rank_data_list.clear();
        _kf_rank_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();
        auto playernode = config.FindNode( "PlayerData" );
        auto datanode = playernode.FindNode( "Data" );
        while ( datanode.IsValid() )
        {
            KFCalcData calcdata;
            calcdata._parent_name = datanode.GetString( "ParentName" );
            calcdata._data_name = datanode.GetString( "DataName" );
            _player_data.push_back( calcdata );

            datanode.NextNode();
        }

        auto ranksnode = config.FindNode( "Ranks" );
        auto ranknode = ranksnode.FindNode( "Rank" );
        while ( ranknode.IsValid() )
        {
            auto id = ranknode.GetUInt32( "Id" );
            auto kfsetting = _kf_rank_setting.Create( id );

            kfsetting->_id = id;
            kfsetting->_parent_name = ranknode.GetString( "ParentName" );
            kfsetting->_data_name = ranknode.GetString( "DataName" );
            kfsetting->_zone_type = ranknode.GetUInt32( "ZoneType" );
            kfsetting->_max_count = ranknode.GetUInt32( "MaxCount" );

            auto refreshnode = ranknode.FindNode( "Refresh" );
            kfsetting->_is_reset_data = refreshnode.GetBoolen( "Reset" );
            kfsetting->_refresh_type = refreshnode.GetUInt32( "RefreshType" );
            kfsetting->_refresh_time = refreshnode.GetUInt32( "RefreshTime" );
            kfsetting->_refresh_hour = refreshnode.GetUInt32( "RefreshHour" );
            kfsetting->_refresh_minute = refreshnode.GetUInt32( "RefreshMinute" );

            // 计算的数据
            auto calcnodes = ranknode.FindNode( "CalcData" );
            auto calcnode = calcnodes.FindNode( "Data" );
            while ( calcnode.IsValid() )
            {
                // 计算的属性
                KFCalcData calcdata;
                calcdata._parent_name = datanode.GetString( "ParentName" );
                calcdata._data_name = datanode.GetString( "DataName" );
                calcdata._max_value = datanode.GetUInt32( "Max" );
                calcdata._text = datanode.GetString( "Text" );
                kfsetting->_calc_data.push_back( calcdata );

                // 数据关联的内标
                if ( calcdata.IsCalcData() )
                {
                    RankDataType key( calcdata._parent_name, calcdata._data_name );
                    _kf_rank_data_list[ key ].push_back( kfsetting );
                }

                calcnode.NextNode();
            }

            ranknode.NextNode();
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }

    const KFRankSetting* KFRankConfig::FindSetting( uint32 rankid ) const
    {
        return _kf_rank_setting.Find( rankid );
    }

    std::vector< KFRankSetting* >& KFRankConfig::FindSetting( const std::string& parentdata, const std::string& rankdata )
    {
        RankDataType key( parentdata, rankdata );

        auto iter = _kf_rank_data_list.find( key );
        if ( iter == _kf_rank_data_list.end() )
        {
            static std::vector< KFRankSetting* > _empty_list;
            return _empty_list;
        }

        return iter->second;
    }
}