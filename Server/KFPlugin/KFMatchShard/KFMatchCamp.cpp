#include "KFMatchCamp.h"
#include "KFMatchQueue.h"

namespace KFrame
{
    KFMatchCamp::KFMatchCamp()
    {
        _camp_id = _invalid_int;
        _player_count = _invalid_int;
        _battle_server_id = _invalid_int;

        // 启动加入定时器
        _enter_timer.StartTimer( 1, 5000 );
    }

    KFMatchCamp::~KFMatchCamp()
    {
        _group_list.Clear();
    }

    void KFMatchCamp::SaveTo( KFMsg::PBBattleCamp* pbcamp )
    {
        pbcamp->set_campid( _camp_id );

        for ( auto& giter : _group_list._objects )
        {
            auto kfgroup = giter.second;
            for ( auto& iter : kfgroup->_player_list )
            {
                auto kfplayer = iter.second;
                kfplayer->SaveTo( pbcamp->add_pbplayer() );
            }
        }
    }

    void KFMatchCamp::AddGroup( KFMatchGroup* kfgroup )
    {
        // 设置玩家的阵营id
        for ( auto& iter : kfgroup->_player_list )
        {
            auto kfplayer = iter.second;
            kfplayer->SetCampID( _camp_id );
        }

        _player_count += kfgroup->PlayerCount();
        _group_list.Insert( kfgroup->_group_id, kfgroup );
        __LOG_DEBUG__( "camp[{}] add group[{}] playercount[{}]!", _camp_id, kfgroup->_group_id, _player_count );
    }

    bool KFMatchCamp::RemoveGroup( uint64 groupid )
    {
        auto kfgroup = _group_list.Find( groupid );
        if ( kfgroup == nullptr )
        {
            return false;
        }

        _player_count -= __MIN__( _player_count, kfgroup->PlayerCount() );
        _group_list.Remove( groupid );
        return true;
    }

    bool KFMatchCamp::RemovePlayer( uint64 groupid, uint32 playerid )
    {
        auto kfgroup = _group_list.Find( groupid );
        if ( kfgroup == nullptr )
        {
            return false;
        }

        auto ok = kfgroup->RemovePlayer( playerid );
        if ( ok )
        {
            _player_count -= __MIN__( _player_count, 1 );
            if ( kfgroup->PlayerCount() == 0 )
            {
                _group_list.Remove( groupid );
            }
        }

        return ok;
    }

    uint32 KFMatchCamp::PlayerCount()
    {
        return _player_count;
    }

    void KFMatchCamp::EnterBattleRoomReq( KFMatchRoom* kfroom )
    {
        if ( !_enter_timer.DoneTimer( KFGlobal::Instance()->_game_time, true ) )
        {
            return;
        }

        KFMsg::S2SAddCampToBattleShardReq req;
        SaveTo( req.mutable_pbcamp() );
        req.set_matchid( kfroom->_match_id );
        req.set_roomid( kfroom->_room_id );
        req.set_matchshardid( KFGlobal::Instance()->_app_id );
        auto ok = kfroom->SendToBattle( KFMsg::S2S_ADD_CAMP_TO_BATTLE_SHARD_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( "camp[{}] enter room[{}] req!", _camp_id, kfroom->_room_id );
        }
        else
        {
            __LOG_ERROR__( "camp[{}] enter room[{}] failed!", _camp_id, kfroom->_room_id );
        }
    }

    void KFMatchCamp::EnterBattleRoomAck( KFMatchRoom* kfroom )
    {
        _enter_timer.StopTimer();
        __LOG_DEBUG__( "camp[{}] enter room[{}] ok!", _camp_id, kfroom->_room_id );
    }

    void KFMatchCamp::ResetEnterBattleRoom()
    {
        _enter_timer.StartTimer( 1, 5000 );
        __LOG_DEBUG__( "camp[{}] reset enter room!", _camp_id );
    }
}