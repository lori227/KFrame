#include "KFRelationConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFRelationConfig::KFRelationConfig()
    {
        _max_friend_count = 0;
        _single_team_friend_liness = 0;
        _max_team_friend_liness = 0;
        _single_win_friend_liness = 0;
        _max_win_friend_liness = 0;
        _jing_jiu_friend_liness = 0;
        _friend_liness_reset_time = 0;
        _max_invite_count = 0;
        _invite_keep_time = 0;
        _max_jing_jiu_friend_liness = 0;
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
            _friend_liness_reset_time = setting.GetUInt32( "ResetFriendLinessTime" );
            _max_jing_jiu_friend_liness = setting.GetUInt32( "MaxJingJiuFriendLiness" );
            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    uint32 KFRelationConfig::getMaxFriendLine( uint32 operate )
    {
        switch ( operate )
        {
        case KFFriendEnum::Team:
            return _max_team_friend_liness;
            break;
        case KFFriendEnum::Win:
            return _max_win_friend_liness;
            break;
        case KFFriendEnum::Laud:
            return _max_jing_jiu_friend_liness;
            break;
        default:
            break;
        }

        return _invalid_int;
    }
}