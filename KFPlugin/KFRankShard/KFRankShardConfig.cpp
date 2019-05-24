#include "KFRankShardConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRankShardConfig::LoadConfig( const std::string& file )
    {
        _kf_rank_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();

        auto ranksnode = config.FindNode( "Ranks" );
        auto ranknode = ranksnode.FindNode( "Rank" );
        while ( ranknode.IsValid() )
        {
            auto id = ranknode.GetUInt32( "Id" );
            auto kfsetting = _kf_rank_setting.Create( id );
            kfsetting->_rank_id = id;

            //data
            kfsetting->_parent_data = ranknode.GetString( "ParentData" );
            kfsetting->_rank_data = ranknode.GetString( "RankData" );
            kfsetting->_zone_type = ranknode.GetUInt32( "ZoneType" );
            kfsetting->_max_count = ranknode.GetUInt32( "MaxCount" );

            // refresh
            auto refreshnode = ranknode.FindNode( "Refresh" );
            kfsetting->_is_reset_data = refreshnode.GetBoolen( "Reset" );
            kfsetting->_refresh_type = refreshnode.GetUInt32( "RefreshType" );
            kfsetting->_refresh_time = refreshnode.GetUInt32( "RefreshTime" );
            kfsetting->_refresh_hour = refreshnode.GetUInt32( "RefreshHour" );
            kfsetting->_refresh_minute = refreshnode.GetUInt32( "RefreshMinute" );
            ranknode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    const KFRankSetting* KFRankShardConfig::FindRankSetting( uint32 rankid ) const
    {
        return _kf_rank_setting.Find( rankid );
    }
}