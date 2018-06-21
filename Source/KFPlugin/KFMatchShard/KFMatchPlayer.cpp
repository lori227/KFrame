#include "KFMatchPlayer.h"
#include "KFMatchRoom.h"
#include "KFClusterShard/KFClusterShardInterface.h"

namespace KFrame
{
	KFMatchPlayer::KFMatchPlayer()
	{
	}

	KFMatchPlayer::~KFMatchPlayer()
	{
	}

	KFMatchPlayer* KFMatchPlayer::Create( const KFMsg::PBBattlePlayer* pbplayer )
	{
		auto kfplayer = __KF_CREATE_BATCH__( KFMatchPlayer, 100 );
		kfplayer->LoadFrom( pbplayer );
		return kfplayer;
	}

	void KFMatchPlayer::Release( KFMatchPlayer* kfplayer )
	{
		__KF_DESTROY__( KFMatchPlayer, kfplayer );
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

	uint32 KFMatchPlayer::GetGroupID() const
	{
		return _pb_player.groupid();
	}

	bool KFMatchPlayer::SendMessageToGame( uint32 msgid, google::protobuf::Message* message )
	{
		return _kf_cluster_shard->SendMessageToClient( _pb_player.serverid(), msgid, message );
	}

	void KFMatchPlayer::QueryMatchRoom( uint32 serverid, KFMatchRoom* kfroom )
	{
		_pb_player.set_serverid( serverid );

		if ( kfroom != nullptr )
		{
			KFMsg::S2SPlayerOnlineToBattleShardReq req;
			req.set_roomid( kfroom->_battle_room_id );
			req.set_campid( _pb_player.campid() );
			req.set_playerid( _pb_player.playerid() );
			req.set_serverid( serverid );
			auto ok = kfroom->SendMessageToBattle( KFMsg::S2S_PLAYER_ONLINE_TO_BATTLE_SHARD_REQ, &req );
			if ( !ok )
			{
				KFLogger::LogLogic( KFLogger::Info, "[%s] player[%u] online to room[%s] failed!",
					__FUNCTION__, GetID(), kfroom->_str_room_id.c_str() );
			}
		}
	}

	void KFMatchPlayer::CancelMatchReq()
	{
		_cancel_timer.StartTimer( 1, 5000 );
	}

	void KFMatchPlayer::RunCancelMatch( KFMatchRoom* kfroom )
	{
		if ( !_cancel_timer.DoneTimer( KFGlobal::Instance()->_game_time, true ) )
		{
			return;
		}

		// 发送消息到集群
		KFMsg::S2SCancelMatchToBattleShardReq req;
		req.set_roomid( kfroom->_battle_room_id );
		req.set_campid( _pb_player.campid() );
		req.set_playerid( _pb_player.playerid() );
		req.set_matchid( kfroom->_match_id );
		req.set_matchshardid( KFGlobal::Instance()->_app_id );
		kfroom->SendMessageToBattle( KFMsg::S2S_CANCEL_MATCH_TO_BATTLE_SHARD_REQ, &req );

		auto strroomid = __KF_STRING__( kfroom->_battle_room_id );
		KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] player[%u:%u] cancal match req!",
			__FUNCTION__, strroomid.c_str(), _pb_player.campid(), _pb_player.playerid() );
	}
}