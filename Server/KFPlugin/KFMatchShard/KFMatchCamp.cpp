#include "KFMatchCamp.h"
#include "KFMatchQueue.h"

namespace KFrame
{
    KFMatchCamp::KFMatchCamp()
    {
        _camp_id = 0;

        // 启动加入定时器
        _enter_timer.StartTimer( 1, 5000 );
    }

    KFMatchCamp::~KFMatchCamp()
    {

    }

    uint32 KFMatchCamp::PlayerCount()
    {
        return _kf_player_list.Size();
    }

    bool KFMatchCamp::HavePlayer( uint32 playerid )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        return kfplayer != nullptr;
    }

    void KFMatchCamp::LoadFrom( const KFMsg::PBMatchGroup* pbgroup )
    {
        for ( auto i = 0; i < pbgroup->pbplayer_size(); ++i )
        {
            auto pbplayer = &pbgroup->pbplayer( i );
            auto kfplayer = KFMatchPlayer::Create( pbplayer );

            kfplayer->SetCampID( _camp_id );
            _kf_player_list.Insert( kfplayer->GetID(), kfplayer );
        }
    }

    void KFMatchCamp::SaveTo( KFMsg::PBBattleCamp* pbcamp )
    {
        pbcamp->set_campid( _camp_id );

        auto kfplayer = _kf_player_list.First();
        while ( kfplayer != nullptr )
        {
            kfplayer->SaveTo( pbcamp->add_pbplayer() );
            kfplayer = _kf_player_list.Next();
        }
    }

    void KFMatchCamp::AddGroup( KFMatchGroup* kfgroup )
    {
        auto kfplayer = kfgroup->_kf_player_list.First();
        while ( kfplayer != nullptr )
        {
            kfplayer->SetCampID( _camp_id );
            _kf_player_list.Insert( kfplayer->GetID(), kfplayer );

            kfplayer = kfgroup->_kf_player_list.Next();
        }

        kfgroup->_kf_player_list.Clear( false );

        __LOG_DEBUG__( KFLogEnum::System, "camp[{}] match group[{}]!", _camp_id, kfgroup->_group_id );
    }

    void KFMatchCamp::EnterBattleRoomReq( KFMatchRoom* kfroom )
    {
        if ( !_enter_timer.DoneTimer( KFGlobal::Instance()->_game_time, true ) )
        {
            return;
        }

        KFMsg::S2SAddCampToBattleShardReq req;
        req.set_roomid( kfroom->_battle_room_id );
        SaveTo( req.mutable_pbcamp() );
        kfroom->SendMessageToBattle( KFMsg::S2S_ADD_CAMP_TO_BATTLE_SHARD_REQ, &req );

        __LOG_DEBUG__( KFLogEnum::System, "camp[{}] enter room[{}] req!", _camp_id, kfroom->_battle_room_id );
    }

    bool KFMatchCamp::EnterBattleRoomAck()
    {
        _enter_timer.StopTimer();
        return true;
    }

    bool KFMatchCamp::LeaveBattleRoom( uint32 plyerid )
    {
        return _kf_player_list.Remove( plyerid );
    }

    bool KFMatchCamp::CancelMatchReq( uint32 playerid )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        kfplayer->CancelMatchReq();
        return true;
    }

    void KFMatchCamp::RunCancelMatch( KFMatchRoom* kfroom )
    {
        auto kfplayer = _kf_player_list.First();
        while ( kfplayer != nullptr )
        {
            kfplayer->RunCancelMatch( kfroom );
            kfplayer = _kf_player_list.Next();
        }
    }

    uint32 KFMatchCamp::CancelMatch( uint32 playerid, bool isroomopen )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        if ( kfplayer == nullptr )
        {
            return _invalid_int;
        }

        // 房间开启了, 只删除取消匹配的人
        if ( isroomopen )
        {
            _kf_player_list.Remove( playerid );
            return 1;
        }

        // 房间还没有开启,把队伍分开 重新组队
        auto groupid = kfplayer->GetGroupID();

        std::set< uint32 > removelist;
        auto kftemp = _kf_player_list.First();
        while ( kftemp != nullptr )
        {
            if ( kftemp->GetGroupID() == groupid )
            {
                removelist.insert( kftemp->GetID() );
            }

            kftemp = _kf_player_list.Next();
        }

        // 删除
        for ( auto id : removelist )
        {
            _kf_player_list.Remove( id );
        }

        return static_cast< uint32 >( removelist.size() );
    }

    bool KFMatchCamp::QueryMatchGroup( uint32 playerid, uint32 serverid, KFMatchRoom* kfroom )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        kfplayer->QueryMatchRoom( serverid, kfroom );
        return true;
    }

}