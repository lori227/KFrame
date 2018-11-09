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
            auto kfsetting = __KF_CREATE__( KFRankSetting );

            kfsetting->_rank_id = ranknode.GetUInt32( "Id" );
            kfsetting->_zone_type = ranknode.GetUInt32( "ZoneType" );
            kfsetting->_parent_data = ranknode.GetString( "ParentData" );
            kfsetting->_rank_data = ranknode.GetString( "RankData" );

            // 计算的数据
            auto calcnodes = ranknode.FindNode( "CalcData" );
            auto calcnode = calcnodes.FindNode( "Data" );
            while ( calcnode.IsValid() )
            {
                auto parentname = calcnode.GetString( "Parent" );
                auto dataname = calcnode.GetString( "Name" );

                AddUpdateSetting( parentname, dataname, kfsetting );
                kfsetting->_calc_data.push_back( DataType( parentname, dataname ) );
                calcnode.NextNode();
            }

            // 显示的属性
            auto shownodes = ranknode.FindNode( "ShowData" );
            auto shownode = shownodes.FindNode( "Data" );
            while ( shownode.IsValid() )
            {
                auto parentname = shownode.GetString( "Parent" );
                auto dataname = shownode.GetString( "Name" );

                kfsetting->_show_data.push_back( DataType( parentname, dataname ) );
                shownode.NextNode();
            }

            AddRankSetting( kfsetting );
            ranknode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    void KFRankClientConfig::AddRankSetting( KFRankSetting* kfsetting )
    {
        _kf_rank_setting.Insert( kfsetting->_rank_id, kfsetting );
        RankDataType key( kfsetting->_parent_data, kfsetting->_rank_data );
        _kf_rank_data_list[ key ].push_back( kfsetting );
    }

    void KFRankClientConfig::AddUpdateSetting( const std::string& parentname, const std::string& dataname, KFRankSetting* kfsetting )
    {
        RankDataType key( parentname, dataname );
        _kf_rank_update_list[ key ].push_back( kfsetting );

    }


    const KFRankSetting* KFRankClientConfig::FindRankSetting( uint32 rankid ) const
    {
        return _kf_rank_setting.Find( rankid );
    }

    std::vector< KFRankSetting* >& KFRankClientConfig::FindRankSetting( const std::string& parentdata, const std::string& rankdata )
    {
        RankDataType key( parentdata, rankdata );

        auto iter = _kf_rank_update_list.find( key );
        if ( iter == _kf_rank_update_list.end() )
        {
            static std::vector< KFRankSetting* > _empty_list;
            return _empty_list;
        }

        return iter->second;
    }


}