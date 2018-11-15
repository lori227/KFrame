#include "KFMatchPlayer.h"
#include "KFMatchRoom.h"
#include "KFClusterShard/KFClusterShardInterface.h"

namespace KFrame
{
    KFMatchPlayer::KFMatchPlayer()
    {
        _room_id = 0;
    }

    void KFMatchPlayer::LoadFrom( const KFMsg::PBBattlePlayer* pbplayer )
    {
        _pb_player.CopyFrom( *pbplayer );
    }

    void KFMatchPlayer::SaveTo( KFMsg::PBBattlePlayer* pbplayer )
    {
        pbplayer->CopyFrom( _pb_player );
    }

    uint32 KFMatchPlayer::GetID() const
    {
        return _pb_player.playerid();
    }

    uint32 KFMatchPlayer::GetCampID() const
    {
        return _pb_player.campid();
    }

    void KFMatchPlayer::SetCampID( uint32 campid )
    {
        _pb_player.set_campid( campid );
    }

    uint64 KFMatchPlayer::GetGroupID() const
    {
        return _pb_player.groupid();
    }

    void KFMatchPlayer::SetGroupID( uint64 gropuid )
    {
        _pb_player.set_groupid( gropuid );
    }

    uint64 KFMatchPlayer::GetRoomID() const
    {
        return _room_id;
    }

    void KFMatchPlayer::SetRoomID( uint64 roomid )
    {
        _room_id = roomid;
    }

    uint32 KFMatchPlayer::GetServerID() const
    {
        return _pb_player.serverid();
    }

    void KFMatchPlayer::SetServerID( uint32 serverid )
    {
        _pb_player.set_serverid( serverid );
    }

    bool KFMatchPlayer::SendToGame( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_cluster_shard->SendToPlayer( _pb_player.serverid(), _pb_player.playerid(), msgid, message );
    }
}