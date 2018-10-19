#include "KFMatchClientConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFMatchClientConfig::KFMatchClientConfig()
    {

    }

    KFMatchClientConfig::~KFMatchClientConfig()
    {

    }

    const KFMatchSetting* KFMatchClientConfig::FindMatchSetting( uint32 matchid ) const
    {
        return _kf_match_setting.Find( matchid );
    }

    bool KFMatchClientConfig::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto matchid = xmlnode.GetUInt32( "MatchId" );
            auto kfsetting = _kf_match_setting.Create( matchid );

            kfsetting->_match_id = matchid;
            kfsetting->_min_group_player_count = xmlnode.GetUInt32( "MinGroupPlayerCount" );
            kfsetting->_max_group_player_count = xmlnode.GetUInt32( "MaxGroupPlayerCount" );

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}