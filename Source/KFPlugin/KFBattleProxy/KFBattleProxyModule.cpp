#include "KFBattleProxyModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
	KFBattleProxyModule::KFBattleProxyModule()
	{
	}

	KFBattleProxyModule::~KFBattleProxyModule()
	{
	}

	void KFBattleProxyModule::InitModule()
	{
		///////////////////////////////////////////////////////////////////////////////
	}

	void KFBattleProxyModule::BeforeRun()
	{
		_kf_tcp_server->RegisterLostFunction( this, &KFBattleProxyModule::OnServerLostHandle );
		////////////////////////////////////////////////////////////////////////////////
		__REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_BATTLE_SERVER_REQ, &KFBattleProxyModule::HandleRegisterBattleServerReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_BATTLE_PROXY_REQ, &KFBattleProxyModule::HandleCreateRoomToBattleProxyReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_QUERY_BATTLE_ROOM_REQ, &KFBattleProxyModule::HandleQueryBattleRoomReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_ROOM_START_REQ, &KFBattleProxyModule::HandleTellBattleRoomStartReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_BATTLE_ROOM_ACK, &KFBattleProxyModule::HandlePlayerEnterBattleRoomAck );
		__REGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_ROOM_FINISH_REQ, &KFBattleProxyModule::HandleTellBattleRoomFinishReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_BATTLE_ROOM_REQ, &KFBattleProxyModule::HandlePlayerLeaveBattleRoomReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LOGIN_BATTLE_ROOM_REQ, &KFBattleProxyModule::HandlePlayerLoginBattleRoomReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_BATTLE_ROOM_SCORE_BALANCE_REQ, &KFBattleProxyModule::HandleBattleRoomScoreBalanceReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_OPEN_BATTLE_ROOM_ACK, &KFBattleProxyModule::HandleOpenBattleRoomAck );
	}

	void KFBattleProxyModule::BeforeShut()
	{
		_kf_tcp_server->UnRegisterLostFunction( this );
		//////////////////////////////////////////////////////////////////////////////
		__UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_BATTLE_SERVER_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_BATTLE_PROXY_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_BATTLE_ROOM_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_ROOM_START_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_BATTLE_ROOM_ACK );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_ROOM_FINISH_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_BATTLE_ROOM_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LOGIN_BATTLE_ROOM_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_BATTLE_ROOM_SCORE_BALANCE_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_OPEN_BATTLE_ROOM_ACK );
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////	
	__KF_SERVER_LOST_FUNCTION__( KFBattleProxyModule::OnServerLostHandle )
	{
		KFLogger::LogLogic( KFLogger::Error, "[%s] client[%s:%s:%u] lost!",
			__FUNCTION__, handlename.c_str(), handletype.c_str(), handleid );

		if ( handlename == KFField::_battle && handletype == KFField::_client )
		{
			OnServerLostBattleServer( handleid );
		}
	}

	void KFBattleProxyModule::OnServerLostBattleServer( uint32 serverid )
	{
		KFLogger::LogLogic( KFLogger::Debug, "[%s] battle[%u] lost req!",
			__FUNCTION__, serverid );

		auto shardid = _kf_cluster_proxy->SelectClusterShard( serverid, false );
		if ( shardid == _invalid_int )
		{
			return KFLogger::LogLogic( KFLogger::Error, "[%s] battle[%u] can't select shard!",
				__FUNCTION__, serverid );
		}

		KFMsg::S2SDisconnectServerToBattleShardReq req;
		req.set_serverid( serverid );
		auto ok = _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_DISCONNECT_SERVER_TO_BATTLE_SHARD_REQ, &req );
		if ( ok )
		{
			KFLogger::LogLogic( KFLogger::Debug, "[%s] battle[%u] lost ok!",
				__FUNCTION__, serverid );
		}
		else
		{
			KFLogger::LogLogic( KFLogger::Error, "[%s] battle[%u] lost failed!",
				__FUNCTION__, serverid );
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleRegisterBattleServerReq )
	{
		__PROTO_PARSE__( KFMsg::S2SRegisterBattleServerReq );

		auto strroomid = __KF_STRING__( kfmsg.roomid() );
		KFLogger::LogLogic( KFLogger::Info, "[%s] register battle[%u|%s:%u|%s:%u] req!",
			__FUNCTION__, kfmsg.serverid(), kfmsg.ip().c_str(), kfmsg.port(), strroomid.c_str(), kfmsg.battleshardid() );

		auto shardid = kfmsg.battleshardid();
		if ( shardid == _invalid_int )
		{
			shardid = _kf_cluster_proxy->SelectClusterShard( kfmsg.serverid(), false );
			if ( shardid == _invalid_int )
			{
				return KFLogger::LogLogic( KFLogger::Error, "[%s] battle[%u|%s:%u] can't select shard!",
					__FUNCTION__, kfmsg.serverid(), kfmsg.ip().c_str(), kfmsg.port() );
			}
		}

		{
			KFMsg::S2SRegisterServerToBattleShardReq req;
			req.set_ip( kfmsg.ip() );
			req.set_port( kfmsg.port() );
			req.set_roomid( kfmsg.roomid() );
			req.set_serverid( kfmsg.serverid() );
			_kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_REGISTER_SERVER_TO_BATTLE_SHARD_REQ, &req );
		}

		{
			// 广播给所有服务器, 有战斗服务器注册了
			KFMsg::S2STellBattleRegisterToShardReq req;
			req.set_serverid( kfmsg.serverid() );
			req.set_roomid( kfmsg.roomid() );
			req.set_ip( kfmsg.ip() );
			req.set_port( kfmsg.port() );
			_kf_cluster_proxy->SendMessageToShard( KFMsg::S2S_TELL_BATTLE_REGISTER_TO_SHARD_REQ, &req );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleCreateRoomToBattleProxyReq )
	{
		__PROTO_PARSE__( KFMsg::S2SCreateRoomToBattleProxyReq );
		auto strroomid = __KF_STRING__( kfmsg.roomid() );
		KFLogger::LogLogic( KFLogger::Info, "[%s] create room[%s] req!",
			__FUNCTION__, strroomid.c_str() );

		auto shardid = _kf_cluster_proxy->FindObjectShard( kfmsg.roomid() );
		if ( shardid == _invalid_int )
		{
			shardid = _kf_cluster_proxy->SelectClusterShard( kfmsg.roomid(), false );
			if ( shardid == _invalid_int )
			{
				return KFLogger::LogLogic( KFLogger::Error, "[%s] [%s] can't select shard!",
					__FUNCTION__, strroomid.c_str() );
			}

			_kf_cluster_proxy->AddObjectShard( kfmsg.roomid(), shardid );
		}

		KFMsg::S2SCreateRoomToBattleShardReq req;
		req.set_matchid( kfmsg.matchid() );
		req.set_roomid( kfmsg.roomid() );
		req.set_matchshardid( __KF_HEAD_ID__( kfguid ) );
		req.set_maxplayercount( kfmsg.maxplayercount() );
		auto ok = _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_CREATE_ROOM_TO_BATTLE_SHARD_REQ, &req );
		if ( ok )
		{
			KFLogger::LogLogic( KFLogger::Info, "[%s] create room[%u:%s] ok!",
				__FUNCTION__, shardid, strroomid.c_str() );
		}
		else
		{
			KFLogger::LogLogic( KFLogger::Info, "[%s] create room[%u:%s] failed!",
				__FUNCTION__, shardid, strroomid.c_str() );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleQueryBattleRoomReq )
	{
		__PROTO_PARSE__( KFMsg::S2SQueryBattleRoomReq );

		auto strroomid = __KF_STRING__( kfmsg.roomid() );
		auto shardid = _kf_cluster_proxy->FindObjectShard( kfmsg.roomid() );

		KFMsg::S2SQueryRoomToBattleShardReq req;
		req.set_roomid( kfmsg.roomid() );
		req.set_playerid( kfmsg.playerid() );
		req.set_serverid( kfmsg.serverid() );
		auto ok = _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_QUERY_ROOM_TO_BATTLE_SHARD_REQ, &req );
		if ( ok )
		{
			KFLogger::LogLogic( KFLogger::Error, "[%s] player[%u] query room[%s] ok!",
				__FUNCTION__, kfmsg.playerid(), strroomid.c_str() );
		}
		else
		{
			KFLogger::LogLogic( KFLogger::Error, "[%s] player[%u] query room[%s] failed!",
				__FUNCTION__, kfmsg.playerid(), strroomid.c_str() );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleOpenBattleRoomAck )
	{
		__PROTO_PARSE__( KFMsg::S2SOpenBattleRoomAck );

		KFMsg::S2SOpenBattleRoomToShardAck ack;
		ack.set_roomid( kfmsg.roomid() );
		ack.set_matchid( kfmsg.matchid() );
		ack.set_result( kfmsg.result() );
		auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_OPEN_BATTLE_ROOM_TO_SHARD_ACK, &ack );
		if ( !ok )
		{
			auto strroomid = __KF_STRING__( kfmsg.roomid() );
			KFLogger::LogLogic( KFLogger::Error, "[%s] open room[%u:%s] failed!",
				__FUNCTION__, kfmsg.battleshardid(), strroomid.c_str() );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandlePlayerEnterBattleRoomAck )
	{
		__PROTO_PARSE__( KFMsg::S2SPlayerEnterBattleRoomAck );

		KFMsg::S2SPlayerEnterRoomToBattleShardAck ack;
		ack.set_roomid( kfmsg.roomid() );
		ack.set_campid( kfmsg.campid() );
		ack.set_playerid( kfmsg.playerid() );
		auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_PLAYER_ENTER_ROOM_TO_BATTLE_SHARD_ACK, &ack );
		if ( !ok )
		{
			auto strroomid = __KF_STRING__( kfmsg.roomid() );
			KFLogger::LogLogic( KFLogger::Error, "[%s] enter room[%u:%s] failed!",
				__FUNCTION__, kfmsg.battleshardid(), strroomid.c_str() );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleTellBattleRoomStartReq )
	{
		__PROTO_PARSE__( KFMsg::S2STellBattleRoomStartReq );

		KFMsg::S2STellRoomStartToBattleShardReq req;
		req.set_roomid( kfmsg.roomid() );
		auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_TELL_ROOM_START_TO_BATTLE_SHARD_REQ, &req );
		if ( !ok )
		{
			auto strroomid = __KF_STRING__( kfmsg.roomid() );
			KFLogger::LogLogic( KFLogger::Error, "[%s] start room[%u:%s] failed!",
				__FUNCTION__, kfmsg.battleshardid(), strroomid.c_str() );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleTellBattleRoomFinishReq )
	{
		__PROTO_PARSE__( KFMsg::S2STellBattleRoomFinishReq );

		KFMsg::S2STellRoomFinishToBattleShardReq req;
		req.set_roomid( kfmsg.roomid() );
		req.set_serverid( kfmsg.serverid() );
		req.set_ip( kfmsg.ip() );
		auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_TELL_ROOM_FINISH_TO_BATTLE_SAHRD_REQ, &req );
		if ( !ok )
		{
			auto strroomid = __KF_STRING__( kfmsg.roomid() );
			KFLogger::LogLogic( KFLogger::Error, "[%s] finish room[%u:%s] failed!",
				__FUNCTION__, kfmsg.battleshardid(), strroomid.c_str() );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandlePlayerLoginBattleRoomReq )
	{
		__PROTO_PARSE__( KFMsg::S2SPlayerLoginBattleRoomReq );

		KFMsg::S2SPlayerLoginRoomToBattleShardReq req;
		req.set_roomid( kfmsg.roomid() );
		req.set_campid( kfmsg.campid() );
		req.set_playerid( kfmsg.playerid() );
		auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_PLAYER_LOGIN_ROOM_TO_BATTLE_SHARD_REQ, &req );
		if ( !ok )
		{
			auto strroomid = __KF_STRING__( kfmsg.roomid() );
			KFLogger::LogLogic( KFLogger::Error, "[%s] login room[%u:%s] failed!",
				__FUNCTION__, kfmsg.battleshardid(), strroomid.c_str() );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandlePlayerLeaveBattleRoomReq )
	{
		__PROTO_PARSE__( KFMsg::S2SPlayerLeaveBattleRoomReq );

		KFMsg::S2SPlayerLeaveRoomToBattleShardReq req;
		req.set_roomid( kfmsg.roomid() );
		req.set_campid( kfmsg.campid() );
		req.set_playerid( kfmsg.playerid() );
		auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_BATTLE_SHARD_REQ, &req );
		if ( !ok )
		{
			auto strroomid = __KF_STRING__( kfmsg.roomid() );
			KFLogger::LogLogic( KFLogger::Error, "[%s] leave room[%u:%s] failed!",
				__FUNCTION__, kfmsg.battleshardid(), strroomid.c_str() );
		}
	}


	__KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleBattleRoomScoreBalanceReq )
	{
		__PROTO_PARSE__( KFMsg::S2SBattleRoomScoreBalanceReq );

		KFMsg::S2SBattleScoreBalanceToShardReq req;
		req.set_roomid( kfmsg.roomid() );
		req.mutable_pbscore()->CopyFrom( kfmsg.pbscore() );
		auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_BATTLE_SCORE_BALANCE_TO_SHARD_REQ, &req );
		if ( !ok )
		{
			auto strroomid = __KF_STRING__( kfmsg.roomid() );
			KFLogger::LogLogic( KFLogger::Error, "[%s] balance room[%u:%s] failed!",
				__FUNCTION__, kfmsg.battleshardid(), strroomid.c_str() );
		}
	}
	
}