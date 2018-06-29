#include "KFMatchGroup.h"
#include "KFMatchQueue.h"

namespace KFrame
{
    KFMatchGroup::KFMatchGroup()
    {
        _group_id = 0;
    }

    KFMatchGroup::~KFMatchGroup()
    {

    }

    uint32 KFMatchGroup::PlayerCount()
    {
        return _kf_player_list.Size();
    }

    bool KFMatchGroup::IsFull( const KFMatchSetting* kfsetting )
    {
        return PlayerCount() >= kfsetting->_max_group_player_count;
    }

    bool KFMatchGroup::HavePlayer( uint32 playerid )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        return kfplayer != nullptr;
    }

    void KFMatchGroup::LoadFrom( const KFMsg::PBMatchGroup* pbgroup )
    {
        _group_id = pbgroup->groupid();

        for ( auto i = 0; i < pbgroup->pbplayer_size(); ++i )
        {
            auto pbplayer = &pbgroup->pbplayer( i );
            auto kfplayer = KFMatchPlayer::Create( pbplayer );

            _kf_player_list.Insert( kfplayer->GetID(), kfplayer );
        }
    }

    bool KFMatchGroup::QueryMatchGroup( uint32 playerid, uint32 gameid )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        kfplayer->QueryMatchRoom( gameid, nullptr );
        return true;
    }
}