#include "KFBattleCamp.h"

namespace KFrame
{
    KFBattleCamp::KFBattleCamp()
    {
        _camp_id = 0;
    }

    KFBattleCamp::~KFBattleCamp()
    {

    }

    uint32 KFBattleCamp::PlayerCount()
    {
        return _kf_player_list.Size();
    }

    bool KFBattleCamp::HavePlayer( uint64 playerid )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        return kfplayer != nullptr;
    }

    void KFBattleCamp::LoadFrom( const KFMsg::PBBattleCamp* pbcamp )
    {
        _camp_id = pbcamp->campid();

        for ( auto i = 0; i < pbcamp->pbplayer_size(); ++i )
        {
            auto& pbplayer = pbcamp->pbplayer( i );
            auto kfplayer = KFBattlePlayer::Create( pbplayer );
            _kf_player_list.Insert( kfplayer->GetID(), kfplayer );

            __LOG_DEBUG__( "camp[{}] add player[{}]!", _camp_id, kfplayer->GetID() );
        }
    }

    void KFBattleCamp::RunEnterBattleRoom( KFBattleRoom* kfroom )
    {
        for ( auto& iter : _kf_player_list._objects )
        {
            auto kfplayer = iter.second;
            kfplayer->RunEnterRoom( kfroom );
        }
    }

    void KFBattleCamp::FinishLeaveBattleRoom( KFBattleRoom* kfroom )
    {
        for ( auto& iter : _kf_player_list._objects )
        {
            auto kfplayer = iter.second;
            kfplayer->FinishLeaveRoom( kfroom );
        }
    }

    void KFBattleCamp::ResetBattleRoomStatus()
    {
        for ( auto& iter : _kf_player_list._objects )
        {
            auto kfplayer = iter.second;
            kfplayer->ResetRoomStatus();
        }
    }

    void KFBattleCamp::SendMessageToCamp( uint32 msgid, google::protobuf::Message* message )
    {
        for ( auto& iter : _kf_player_list._objects )
        {
            auto kfplayer = iter.second;
            kfplayer->SendToClient( msgid, message );
        }
    }
}