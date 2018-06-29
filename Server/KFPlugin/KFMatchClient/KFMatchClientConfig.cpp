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

    bool KFMatchClientConfig::LoadConfig( const char* file )
    {
        _kf_match_setting.Clear();

        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto xmlnode = config.FindNode( "Setting" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFMatchSetting );

                kfsetting->_match_id = xmlnode.GetUInt32( "MatchId" );
                kfsetting->_min_group_player_count = xmlnode.GetUInt32( "MinGroupPlayerCount" );
                kfsetting->_max_group_player_count = xmlnode.GetUInt32( "MaxGroupPlayerCount" );
                _kf_match_setting.Insert( kfsetting->_match_id, kfsetting );

                xmlnode.NextNode();
            }
            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}