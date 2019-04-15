#include "KFRankMasterConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRankMasterConfig::LoadConfig()
    {
        _kf_rank_setting.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();

        auto ranksnode = config.FindNode( "Ranks" );
        auto ranknode = ranksnode.FindNode( "Rank" );
        while ( ranknode.IsValid() )
        {
            auto id = ranknode.GetUInt32( "Id" );
            auto kfsetting = _kf_rank_setting.Create( id );
            kfsetting->_rank_id = id;

            auto refreshnode = ranknode.FindNode( "Refresh" );
            kfsetting->_refresh_type = refreshnode.GetUInt32( "RefreshType" );
            kfsetting->_refresh_time = refreshnode.GetUInt32( "RefreshTime" );
            kfsetting->_refresh_hour = refreshnode.GetUInt32( "RefreshHour" );
            kfsetting->_refresh_minute = refreshnode.GetUInt32( "RefreshMinute" );

            ranknode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    const KFRankSetting* KFRankMasterConfig::FindRankSetting( uint32 rankid ) const
    {
        return _kf_rank_setting.Find( rankid );
    }
}