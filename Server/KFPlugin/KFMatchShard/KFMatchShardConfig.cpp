#include "KFMatchShardConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFMatchShardConfig::KFMatchShardConfig()
    {

    }

    KFMatchShardConfig::~KFMatchShardConfig()
    {

    }

    const KFMatchSetting* KFMatchShardConfig::FindMatchSetting( uint32 matchid ) const
    {
        return _kf_match_setting.Find( matchid );
    }

    bool KFMatchShardConfig::LoadConfig( const char* file )
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
                kfsetting->_max_player_count = xmlnode.GetUInt32( "MaxPlayerCount" );
                kfsetting->_max_room_wait_time = xmlnode.GetUInt32( "MaxRoomWaitTime" ) * KFTimeEnum::OneMinuteMicSecond;
                kfsetting->_wait_confirm_time = xmlnode.GetUInt32( "WaitConfirmTime" ) * KFTimeEnum::OneSecondMicSecond;

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