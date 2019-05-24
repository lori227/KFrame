#include "KFRankClientConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRankClientConfig::LoadConfig( const std::string& file )
    {
        _player_data.clear();
        _kf_rank_data_list.clear();
        _kf_rank_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
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

            kfsetting->_rank_id = id;
            kfsetting->_parent_name = ranknode.GetString( "ParentName" );
            kfsetting->_data_name = ranknode.GetString( "DataName" );
            kfsetting->_zone_type = ranknode.GetUInt32( "ZoneType" );

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


    const KFRankSetting* KFRankClientConfig::FindRankSetting( uint32 rankid ) const
    {
        return _kf_rank_setting.Find( rankid );
    }

    std::vector< KFRankSetting* >& KFRankClientConfig::FindRankSetting( const std::string& parentdata, const std::string& rankdata )
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