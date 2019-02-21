#include "KFRankClientConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRankClientConfig::LoadConfig()
    {
        _player_data.clear();
        _kf_rank_data_list.clear();
        _kf_rank_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto playernode = config.FindNode( "PlayerData" );
        auto datanode = playernode.FindNode( "Data" );
        while ( datanode.IsValid() )
        {
            auto parentname = datanode.GetString( "Parent" );
            auto dataname = datanode.GetString( "Name" );
            _player_data.push_back( DataType( parentname, dataname ) );

            datanode.NextNode();
        }

        auto ranksnode = config.FindNode( "Ranks" );
        auto ranknode = ranksnode.FindNode( "Rank" );
        while ( ranknode.IsValid() )
        {
            auto id = ranknode.GetUInt32( "Id" );
            auto kfsetting = _kf_rank_setting.Create( id );

            kfsetting->_rank_id = id;
            kfsetting->_zone_type = ranknode.GetUInt32( "ZoneType" );
            kfsetting->_parent_name = ranknode.GetString( "ParentData" );
            kfsetting->_rank_name = ranknode.GetString( "RankData" );

            // 计算的数据
            auto calcnodes = ranknode.FindNode( "CalcData" );
            auto calcnode = calcnodes.FindNode( "Data" );
            while ( calcnode.IsValid() )
            {
                auto parentname = calcnode.GetString( "Parent" );
                auto dataname = calcnode.GetString( "Name" );
                kfsetting->_calc_data.push_back( DataType( parentname, dataname ) );

                // 数据关联的内标
                RankDataType key( parentname, dataname );
                _kf_rank_data_list[ key ].push_back( kfsetting );

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