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
        for ( auto iter : _player_list )
        {
            _kf_match_queue->RemoveMatchPlayer( iter.first );
        }
        _player_list.clear();
    }

    uint32 KFMatchGroup::PlayerCount()
    {
        return static_cast< uint32 >( _player_list.size() );
    }

    bool KFMatchGroup::IsFull( const KFMatchSetting* kfsetting )
    {
        return PlayerCount() >= kfsetting->_max_group_player_count;
    }

    bool KFMatchGroup::HavePlayer( uint32 playerid )
    {
        auto iter = _player_list.find( playerid );
        return iter != _player_list.end();
    }

    void KFMatchGroup::LoadFrom( const KFMsg::PBMatchGroup* pbgroup )
    {
        _group_id = pbgroup->groupid();

        for ( auto i = 0; i < pbgroup->pbplayer_size(); ++i )
        {
            auto pbplayer = &pbgroup->pbplayer( i );
            auto kfplayer = _kf_match_queue->CreateMatchPlayer( pbplayer );
            kfplayer->SetGroupID( _group_id );

            _player_list[ kfplayer->GetID() ] = kfplayer;

            __LOG_DEBUG__( KFLogEnum::Logic, "group[{}] add player[{}]!", _group_id, kfplayer->GetID() );
        }
    }

    bool KFMatchGroup::RemovePlayer( uint32 playerid )
    {
        _kf_match_queue->RemoveMatchPlayer( playerid );
        return _player_list.erase( playerid ) > 0u;
    }
}