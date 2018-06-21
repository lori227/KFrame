#include "KFBattlePlayer.h"
#include "KFBattleRoom.h"
#include "KFBattleConfig.h"
#include "KFClusterShard/KFClusterShardInterface.h"

namespace KFrame
{
	KFBattlePlayer::KFBattlePlayer()
	{
		_notice_count = 0;
		_status = KFPlayerStatus::StatusPlayerIdle;
	}

	KFBattlePlayer::~KFBattlePlayer()
	{
	
	}
	
	KFBattlePlayer* KFBattlePlayer::Create( const KFMsg::PBBattlePlayer& pbplayer )
	{
		auto kfplayer = __KF_CREATE_BATCH__( KFBattlePlayer, 100 );
		kfplayer->LoadFrom( pbplayer );

		return kfplayer;
	}

	void KFBattlePlayer::Release( KFBattlePlayer* kfplayer )
	{
		__KF_DESTROY__( KFBattlePlayer, kfplayer );
	}

	uint32 KFBattlePlayer::GetID()
	{
		return _pb_player.playerid();
	}

	void KFBattlePlayer::LoadFrom( const KFMsg::PBBattlePlayer& pbplayer )
	{
		_pb_player.CopyFrom( pbplayer );

		auto temp = __KF_STRING__( KFUtility::Make64Guid( pbplayer.playerid() ) );
		_token = KFCrypto::Md5Encode( temp.c_str() );

		_status = KFPlayerStatus::StatusEnterRoom;
	}

	void KFBattlePlayer::SaveTo( KFMsg::PBBattlePlayer* pbplayer )
	{
		pbplayer->CopyFrom( _pb_player );
	}

	bool KFBattlePlayer::SendMessageToClient( uint32 msgid, google::protobuf::Message* message )
	{
		return _kf_cluster_shard->SendMessageToClient( _pb_player.serverid(), msgid, message );
	}

	void KFBattlePlayer::RunEnterRoom( KFBattleRoom* kfroom )
	{
		switch ( _status )
		{
		case KFPlayerStatus::StatusEnterRoom:
		{
			// 玩家进去战场
			PlayerEnterBattleRoom( kfroom );
		}break;
		case KFPlayerStatus::StatusNoticeRoom:
		{
			// 通知玩家战场信息
			PlayerNoticeBattleRoom( kfroom );
		}break;
		default:
			break;
		}
	}

	void KFBattlePlayer::PlayerEnterBattleRoom( KFBattleRoom* kfroom )
	{
		KFMsg::S2SPlayerEnterBattleRoomReq req;
		SaveTo( req.mutable_player() );
		req.set_roomid( kfroom->_battle_room_id );
		req.set_token( _token );
		auto ok = kfroom->SendMessageToBattle( KFMsg::S2S_PLAYER_ENTER_BATTLE_ROOM_REQ, &req );
		if ( ok )
		{
			KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%u] enter battle room[%s] req!",
				__FUNCTION__, _pb_player.campid(), _pb_player.playerid(), kfroom->_str_room_id.c_str() );
		}
	}

	void KFBattlePlayer::ConfirmEnterBattleRoom()
	{
		_notice_count = 0;
		_status = KFPlayerStatus::StatusNoticeRoom;
	}

	void KFBattlePlayer::PlayerNoticeBattleRoom( KFBattleRoom* kfroom )
	{
		// 发送消息
		KFMsg::S2SNoticeMatchRoomReq req;
		req.set_matchid( kfroom->_match_id );
		req.set_battleshardid( KFGlobal::Instance()->_app_id );
		req.set_campid( _pb_player.campid() );
		req.set_playerid( _pb_player.playerid() );
		req.set_serverid( kfroom->_battle_server._server_id );
		req.set_ip( kfroom->_battle_server._ip );
		req.set_port( kfroom->_battle_server._port );
		req.set_roomid( kfroom->_battle_room_id );
		req.set_token( _token );
		auto ok = SendMessageToClient( KFMsg::S2S_NOTICE_MATCH_ROOM_REQ, &req );
		if ( ok )
		{
			++_notice_count;
			if ( _notice_count >= 10 )
			{
				_status = KFPlayerStatus::StatusDisconnetRoom;
			}

			KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%u:%u] notice battle room[%s] req[%u]!",
				__FUNCTION__, _pb_player.campid(), _pb_player.serverid(), _pb_player.playerid(),
				kfroom->_str_room_id.c_str(), _notice_count );
		}
	}

	void KFBattlePlayer::ConfirmNoticeBattleRoom()
	{
		// 确认客户端已经收到战场信息
		_status = KFPlayerStatus::StatusLoginRoom;
	}

	bool KFBattlePlayer::LoginBattleRoomReq( KFBattleRoom* kfroom )
	{
		_status = KFPlayerStatus::StatusPlayingRoom;

		KFMsg::S2SPlayerLoginBattleRoomAck ack;
		ack.set_roomid( kfroom->_battle_room_id );
		ack.set_campid( _pb_player.campid() );
		ack.set_playerid( _pb_player.playerid() );
		auto ok = kfroom->SendMessageToBattle( KFMsg::S2S_PLAYER_LOGIN_BATTLE_ROOM_ACK, &ack );
		if ( !ok )
		{
			KFLogger::LogLogic( KFLogger::Info, "[%s] player[%u] login battle room[%s] failed!",
				__FUNCTION__, _pb_player.playerid(), kfroom->_str_room_id.c_str() );
		}

		return ok;
	}

	void KFBattlePlayer::LeaveBattleRoomReq( KFBattleRoom* kfroom )
	{
		if ( kfroom->IsBattleRoomStart() )
		{
			// 已经开始, 设置离开状态
			_status = KFPlayerStatus::StatusLeaveRoom;
		}
		else
		{
			// 发送消息到Match服务器
			KFMsg::S2SPlayerLeaveRoomToMatchShardReq req;
			req.set_matchid( kfroom->_match_id );
			req.set_roomid( kfroom->_battle_room_id );
			req.set_campid( _pb_player.campid() );
			req.set_playerid( _pb_player.playerid() );
			kfroom->SendMessageToMatch( KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_MATCH_SHARD_REQ, &req );
		}

		// 发送到游戏服务器,
		SendLeaveRoomToPlayer();
	}

	bool KFBattlePlayer::QueryBattleRoom( uint32 serverid )
	{
		// 设置游戏服务器id
		_pb_player.set_serverid( serverid );

		// 已经离开了
		if ( _status != KFPlayerStatus::StatusLeaveRoom )
		{
			_notice_count = 0;
			_status = KFPlayerStatus::StatusNoticeRoom;
		}
		else
		{
			KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] online status[%u] failed!",
				__FUNCTION__, _pb_player.playerid(), _status );
		}

		return true;
	}

	void KFBattlePlayer::OnlineBattleRoom( uint32 serverid )
	{
		_pb_player.set_serverid( serverid );
		if ( _status == KFPlayerStatus::StatusDisconnetRoom )
		{
			_notice_count = 0;
			_status = KFPlayerStatus::StatusNoticeRoom;
			KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] online ok!",
				__FUNCTION__, _pb_player.playerid() );
		}
	}

	void KFBattlePlayer::FinishLeaveRoom()
	{
		if ( _status == KFPlayerStatus::StatusLeaveRoom )
		{
			return;
		}

		_status = KFPlayerStatus::StatusLeaveRoom;
		SendLeaveRoomToPlayer();
	}

	void KFBattlePlayer::SendLeaveRoomToPlayer()
	{
		KFMsg::S2SLeaveBattleRoomToClientAck ack;
		ack.set_playerid( GetID() );
		SendMessageToClient( KFMsg::S2S_LEAVE_BATTLE_ROOM_TO_CLIENT_ACK, &ack );
	}

	bool KFBattlePlayer::BattleScoreBalance( KFBattleRoom* kfroom, KFMsg::PBBattleScore* pbscore )
	{
		// 设置matchid
		pbscore->set_matchid( kfroom->_match_id );

		// 计算评分
		ScoreBalance( pbscore );
		
		// 计算奖励
		auto reward = _kf_battle_config->FindBattleReward( pbscore->matchid(), pbscore->score() );

		{
			KFMsg::S2SBattleRoomScoreBalanceAck ack;
			ack.set_playerid( pbscore->playerid() );
			ack.set_roomid( kfroom->_battle_room_id );
			ack.set_reward( reward );
			auto ok = kfroom->SendMessageToBattle( KFMsg::S2S_BATTLE_ROOM_SCORE_BALANCE_ACK, &ack );
			if ( !ok )
			{
				KFLogger::LogLogic( KFLogger::Error, "[%s] player[%u] balance score send failed!",
					__FUNCTION__, _pb_player.playerid() );
			}
		}

		{
			// 发送到玩家
			KFMsg::S2SPlayerBattleScoreReq req;
			req.set_playerid( _pb_player.playerid() );
			req.set_roomid( kfroom->_battle_room_id );
			req.mutable_pbscore()->CopyFrom( *pbscore );
			SendMessageToClient( KFMsg::S2S_PLAYER_BATTLE_SCORE_REQ, &req );
		}

		return true;
	}

	void KFBattlePlayer::ScoreBalance( KFMsg::PBBattleScore * pbscore )
	{
		// 各项数据
		double battlescore = 0.0f;
		for ( auto i = 0; i < pbscore->pbdata_size(); ++i )
		{
			auto pbdata = &pbscore->pbdata( i );
			auto scoreparam = _kf_battle_config->GetScoreParam( pbdata->name() );
			battlescore += pbdata->value() * scoreparam;
		}

		// 排名分
		auto rankingparam = _kf_battle_config->GetScoreParam( KFField::_ranking );
		battlescore += rankingparam / pbscore->ranking();

		// 吃鸡奖励分
		if ( pbscore->ranking() == 1 )
		{
			auto toponeparam = _kf_battle_config->GetScoreParam( KFField::_top_one );
			battlescore += toponeparam;
		}

		if (battlescore < 0.0f)
		{
			battlescore = 0;
		}
		pbscore->set_score( battlescore );
	}
}