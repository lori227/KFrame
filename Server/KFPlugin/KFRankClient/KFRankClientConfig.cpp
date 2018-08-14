#include "KFRankClientConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFRankClientConfig::KFRankClientConfig()
    {

    }

    KFRankClientConfig::~KFRankClientConfig()
    {

    }

    bool KFRankClientConfig::LoadConfig()
    {
        _player_data.clear();
        _kf_rank_data_list.clear();
        _kf_rank_setting.Clear();

        try
        {
            KFXml kfxml( _file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto playernode = config.FindNode( "PlayerData" );
            auto datanode = playernode.FindNode( "Data" );
            while ( datanode.IsValid() )
            {
                auto dataname = datanode.GetString( "Name" );
                _player_data.push_back( dataname );

                datanode.NextNode();
            }

            playernode.NextNode();
            //////////////////////////////////////////////////////////////////
            auto ranksnode = config.FindNode( "Ranks" );
            auto ranknode = ranksnode.FindNode( "Rank" );
            while ( ranknode.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFRankSetting );

                kfsetting->_rank_id = ranknode.GetUInt32( "Id" );
                kfsetting->_zone_type = ranknode.GetUInt32( "ZoneType" );
                kfsetting->_shard_id = ranknode.GetUInt32( "ShardId" );
                kfsetting->_parent_data = ranknode.GetString( "ParentData" );
                kfsetting->_rank_data = ranknode.GetString( "RankData" );
                kfsetting->_max_count = ranknode.GetUInt32( "MaxCount" );
                kfsetting->_refresh_type = ranknode.GetUInt32( "RefreshType" );
                kfsetting->_refresh_time = ranknode.GetUInt32( "RefreshTime" );
                kfsetting->_refresh_hour = ranknode.GetUInt32( "RefreshHour" );
                kfsetting->_is_reset_data = ranknode.GetBoolen( "Reset" );

                auto shownode = ranknode.FindNode( "ShowData" );
                while ( shownode.IsValid() )
                {
                    auto dataname = shownode.GetString( "Name" );
                    kfsetting->_show_data.push_back( dataname );
                    shownode.NextNode();
                }

                AddRankSetting( kfsetting );
                ranknode.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    void KFRankClientConfig::AddRankSetting( KFRankSetting* kfsetting )
    {
        _kf_rank_setting.Insert( kfsetting->_rank_id, kfsetting );

        RankDataType key( kfsetting->_parent_data, kfsetting->_rank_data );
        _kf_rank_data_list[ key ].push_back( kfsetting );
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