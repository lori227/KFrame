#include "KFRelationConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFRelationConfig::KFRelationConfig()
    {
        _max_friend_count = 100;
        _single_team_friend_liness = 5;
        _max_team_friend_liness = 80;
        _single_win_friend_liness = 5;
        _max_win_friend_liness = 20;
        _jing_jiu_friend_liness = 2;
        _max_jing_jiu_friend_liness = 10;
        _friend_liness_reset_time = 5;
    }

    KFRelationConfig::~KFRelationConfig()
    {

    }

    bool KFRelationConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            //////////////////////////////////////////////////////////////////
            auto setting = config.FindNode( "Setting" );
            _max_friend_count = setting.GetUInt32( "FriendMaxCount" );
            _max_invite_count = setting.GetUInt32( "InviteMaxCount" );
            _invite_keep_time = setting.GetUInt32( "InviteKeepTime" ) * TimeEnum::OneDaySecond;
            _single_team_friend_liness = setting.GetUInt32( "SingleTeamFriendLiness" );
            _max_team_friend_liness = setting.GetUInt32( "MaxTeamFriendLiness" );
            _single_win_friend_liness = setting.GetUInt32( "SingleWinFriendLiness" );
            _max_win_friend_liness = setting.GetUInt32( "MaxWinFriendLiness" );
            _jing_jiu_friend_liness = setting.GetUInt32( "JingJiuFriendLiness" );
            _max_jing_jiu_friend_liness = setting.GetUInt32( "MaxJingJiuFriendLiness" );
            _friend_liness_reset_time = setting.GetUInt32( "ResetFriendLinessTime" );
            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}