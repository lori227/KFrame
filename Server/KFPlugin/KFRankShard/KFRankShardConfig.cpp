#include "KFRankShardConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFRankShardConfig::KFRankShardConfig()
    {

    }

    KFRankShardConfig::~KFRankShardConfig()
    {

    }

    bool KFRankShardConfig::LoadConfig()
    {
        _kf_rank_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();

        auto ranksnode = config.FindNode( "Ranks" );
        auto ranknode = ranksnode.FindNode( "Rank" );
        while ( ranknode.IsValid() )
        {
            auto kfsetting = __KF_CREATE__( KFRankSetting );

            kfsetting->_rank_id = ranknode.GetUInt32( "Id" );
            kfsetting->_zone_type = ranknode.GetUInt32( "ZoneType" );
            kfsetting->_parent_data = ranknode.GetString( "ParentData" );
            kfsetting->_rank_data = ranknode.GetString( "RankData" );
            kfsetting->_max_count = ranknode.GetUInt32( "MaxCount" );
            kfsetting->_refresh_type = ranknode.GetUInt32( "RefreshType" );
            kfsetting->_refresh_time = ranknode.GetUInt32( "RefreshTime" );
            kfsetting->_refresh_hour = ranknode.GetUInt32( "RefreshHour" );
            kfsetting->_refresh_minute = ranknode.GetUInt32( "RefreshMinute" );
            kfsetting->_is_reset_data = ranknode.GetBoolen( "Reset" );

            AddRankSetting( kfsetting );
            ranknode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    void KFRankShardConfig::AddRankSetting( KFRankSetting* kfsetting )
    {
        _kf_rank_setting.Insert( kfsetting->_rank_id, kfsetting );
    }

    const KFRankSetting* KFRankShardConfig::FindRankSetting( uint32 rankid ) const
    {
        return _kf_rank_setting.Find( rankid );
    }
}