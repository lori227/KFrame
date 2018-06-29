#include "KFRankClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
	KFRankClientModule::KFRankClientModule()
	{

	}

	KFRankClientModule::~KFRankClientModule()
	{

	}

	void KFRankClientModule::InitModule()
	{
	}

	void KFRankClientModule::BeforeRun()
	{
		__REGISTER_MESSAGE__( KFMsg::MSG_QUERY_WHOLE_RANK_LIST_REQ, &KFRankClientModule::HandleQueryWholeRankListReq );
		__REGISTER_MESSAGE__( KFMsg::MSG_QUERY_FRIEND_RANK_LIST_REQ, &KFRankClientModule::HandleQueryFriendRankListReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_ACK, &KFRankClientModule::HandleQueryRankListAck );

	}

	void KFRankClientModule::BeforeShut()
	{
		__UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_WHOLE_RANK_LIST_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_FRIEND_RANK_LIST_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_ACK );
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	bool KFRankClientModule::SendMessageToRank( uint64 zoneid, uint32 msgid, ::google::protobuf::Message* message )
	{
		KFMsg::S2STransmitToRankShardReq req;
		req.set_zoneid( zoneid );
		req.set_msgid( msgid );
		req.set_msgdata( message->SerializeAsString() );
		return _kf_cluster->SendMessageToShard( KFField::_rank, KFMsg::S2S_TRANSMIT_TO_RANK_SHARD_REQ, &req );
	}

	bool KFRankClientModule::SendMessageToRankShard( uint32 msgid, ::google::protobuf::Message* message )
	{
		return _kf_cluster->SendMessageToShard( KFField::_rank, msgid, message );
	}

	__KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryWholeRankListReq )
	{
		__CLIENT_PROTO_PARSE__( KFMsg::MsgQueryWholeRankListReq );
		if ( KFMsg::RankListEnum_MIN > kfmsg.matchid() || KFMsg::RankListEnum_MAX < kfmsg.matchid() )
		{
			return;
		}
		
		if ( kfmsg.ranktype() != KFField::_eval_rank && kfmsg.ranktype() != KFField::_win_rank 
			&& kfmsg.ranktype() != KFField::_kill_rank )
		{
			return;
		}
		KFMsg::S2SQueryWholeRankListReq req;
		req.set_matchid( kfmsg.matchid() );
		req.set_playerid( playerid );
		req.set_ranktype( kfmsg.ranktype() );
		req.set_begin( 0 );
		req.set_end( 99 );

		auto zoneid = _kf_zone->GetPlayerZoneId( playerid );
		SendMessageToRank( zoneid, KFMsg::S2S_QUERY_WHOLE_RANK_LIST_REQ, &req );
	}

	__KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryFriendRankListReq )
	{
		__CLIENT_PROTO_PARSE__( KFMsg::MsgQueryFriendRankListReq );
		if ( KFMsg::RankListEnum_MIN > kfmsg.matchid() || KFMsg::RankListEnum_MAX < kfmsg.matchid() )
		{
			return;
		}

		if ( kfmsg.ranktype() != KFField::_eval_rank && kfmsg.ranktype() != KFField::_win_rank && kfmsg.ranktype() != KFField::_kill_rank )
		{
			return;
		}

		auto kfobject = player->GetData();
		auto kffriends = kfobject->FindData( KFField::_friend );
		if ( nullptr == kffriends )
		{
			return;
		}

		KFMsg::S2SQueryFriendRankListReq req;
		req.set_matchid( kfmsg.matchid() );
		req.set_playerid( playerid );
		req.set_ranktype( kfmsg.ranktype() );

		auto kffriend = kffriends->FirstData();
		while ( nullptr != kffriend )
		{
			auto friendid = kffriend->GetValue<uint32>( KFField::_id );
			if ( _invalid_int != friendid )
			{
				auto playerids = req.mutable_playerids();
				playerids->add_playerid( friendid );
			}
			kffriend = kffriends->NextData();
		}
		auto playerids = req.mutable_playerids();
		playerids->add_playerid( playerid );

		auto zoneid = _kf_zone->GetPlayerZoneId( playerid );
		SendMessageToRank( zoneid, KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &req );
	}

	__KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryRankListAck )
	{
		__SERVER_PROTO_PARSE__( KFMsg::S2SQueryRankListAck );
		KFMsg::MsgQueryRankListAck ack;
		ack.set_matchid( kfmsg.matchid() );
		ack.set_ranktype( kfmsg.ranktype() );
		auto pbrankdatas = &kfmsg.rankdatas();
		SerialzeToClient( pbrankdatas , kfmsg.ranktype() );
		auto rankdatas = ack.mutable_rankdatas();
		rankdatas->CopyFrom( kfmsg.rankdatas() );
		if ( kfmsg.isfriendrank() )
		{
			ack.set_isfriendrank( true );
		}

		_kf_player->SendMessageToClient( player, KFMsg::MSG_QUERY_RANK_LIST_ACK, &ack );
	}

	

	void KFRankClientModule::JoinRankList( uint64 matchid, uint32 playerid, KFEntity* player, const std::string& scoretype )
	{
		// 1次join更新6个榜单
		if ( _invalid_int == playerid || nullptr == player || scoretype.empty() )
		{
			return KFLogger::LogLogic( KFLogger::Error, "%s join rank err matchid:%u playerid:%u scoretype:%s!",
				__FUNCTION__, matchid, playerid, scoretype.c_str() );
		}
	
		KFMsg::S2SJoinRankListReq req;
		req.set_playerid( playerid );
		req.set_scoretype( scoretype );
		auto kfobject = player->GetData();
		auto kfscores = kfobject->FindData( scoretype );
		auto kftotalscores = kfobject->FindData( KFField::_total_score );
		if ( nullptr == kfscores || nullptr == kftotalscores )
		{
			return;
		}
		auto pbsocre = req.mutable_scores();
		FormateRankData( pbsocre , kfscores );
		auto pbtotalscore = req.mutable_totalscores();
		FormateRankData( pbtotalscore, kftotalscores );

		auto zoneid = _kf_zone->GetPlayerZoneId( playerid );
		SendMessageToRank( zoneid, KFMsg::S2S_JOIN_RANK_LIST_REQ, &req );
	}

	void KFRankClientModule::CalcRankData( KFData* kfdata, uint64& evalscore, uint64& winscore, uint64& killscore )
	{
		evalscore = _invalid_int;
		winscore = _invalid_int;
		killscore = _invalid_int;
		if ( nullptr == kfdata )
		{
			return;
		}
		auto playsocre = kfdata->GetValue< uint32 >( KFField::_score );
		auto playcount = kfdata->GetValue< uint32 >( KFField::_count );
		if ( _invalid_int == playcount )
		{
			return;
		}

		auto playtopfive = kfdata->GetValue< uint32 >( KFField::_top_five );
		auto playvictory = kfdata->GetValue< uint32 >( KFField::_victory );
		uint32 playwinrate = playvictory * 100 / playcount;
		
		// 评分榜积分
		evalscore = playsocre * 10000000000 + playwinrate * 10000000 + playtopfive;

		// 胜利榜积分
		winscore = playvictory * 10000000000 + playwinrate * 10000000 + playtopfive;

		auto playkill = kfdata->GetValue< uint32 >( KFField::_kill );
		auto playdie = (kfdata->GetValue< uint32 >( KFField::_die ) > 0 ? kfdata->GetValue< uint32 >( KFField::_die ) : 1);
		uint32 playevagekill = playkill / playcount;
		uint32 kd = playkill * 10 / playdie;
		// 击伤榜积分
		killscore = playkill * 10000000000 + playevagekill * 10000000 + kd;
	}

	void KFRankClientModule::FormateRankData( KFMsg::PBScoreData* pbscores, KFData* kfdata ) 
	{
		uint64 evalscore = 0u, winscore = 0u, killscore = 0u;
		CalcRankData( kfdata, evalscore, winscore, killscore );
		{
			auto pbdata = pbscores->add_score();
			pbdata->set_name( KFField::_eval_score );
			pbdata->set_value( evalscore );
		}
		{
			auto pbdata = pbscores->add_score();
			pbdata->set_name( KFField::_win_score );
			pbdata->set_value( winscore );
		}
		{
			auto pbdata = pbscores->add_score();
			pbdata->set_name( KFField::_kill_score );
			pbdata->set_value( killscore );
		}
	}

	void KFRankClientModule::SerialzeToClient( const KFMsg::PBRankData* pbrankdatas, const std::string& ranktype )
	{
		for ( auto i = 0; i < pbrankdatas->rankdata_size(); ++i )
		{
			auto rankinfo = &pbrankdatas->rankdata( i );
			if ( nullptr == rankinfo )
			{
				continue;
			}

			ParseScoreToPb( rankinfo, ranktype );
		}
		
	}

	void KFRankClientModule::ParseScoreToPb( const KFMsg::PBStrings* pbstrings, const std::string& ranktype )
	{
		uint64 score = 0;
		for ( auto i = 0; i < pbstrings->pbstring_size(); ++i )
		{
			auto pbstring = &pbstrings->pbstring( i );
			if ( nullptr != pbstring && pbstring->name() == KFField::_score )
			{
				score = KFUtility::ToValue<uint64>( pbstring->value() );
				break;
			}
		}

		if ( score <= _invalid_int )
		{
			return;
		}

		if ( ranktype == KFField::_eval_rank )
		{
			SerialzeEvalRank( score, pbstrings );
		}

		else if ( ranktype == KFField::_win_rank )
		{
			SerialzeWinRank( score, pbstrings );
		}
		else if ( ranktype == KFField::_kill_rank )
		{
			SerialzeKillRank( score, pbstrings );
		}

	}


	void KFRankClientModule::SerialzeEvalRank( uint64 score, const KFMsg::PBStrings* pbstrings )
	{
		auto rankinfos = const_cast< KFMsg::PBStrings* >( pbstrings );
		uint64 evlascore = 0;
		{
			auto rankinfo = rankinfos->add_pbstring();
			rankinfo->set_name( KFField::_eval_score );
			evlascore = score / 10000000000;
			rankinfo->set_value( __KF_STRING__( evlascore ) );
		}

		uint64 winrate = 0;
		{
			auto rankinfo = rankinfos->add_pbstring();
			rankinfo->set_name( KFField::_win_rate );
			winrate = ( score - evlascore * 10000000000 )/ 10000000;
			rankinfo->set_value( __KF_STRING__( winrate ) );
		}

		{
			auto rankinfo = rankinfos->add_pbstring();
			rankinfo->set_name( KFField::_top_five );
			auto topfive = score - evlascore * 10000000000 - winrate * 10000000;
			rankinfo->set_value( __KF_STRING__( topfive ) );
		}
	}

	void KFRankClientModule::SerialzeWinRank( uint64 score, const KFMsg::PBStrings* pbstrings )
	{
		auto rankinfos = const_cast< KFMsg::PBStrings* >( pbstrings );
		uint64 victory = 0;
		{
			auto rankinfo = rankinfos->add_pbstring();
			rankinfo->set_name( KFField::_victory );
			victory = score / 10000000000;
			rankinfo->set_value( __KF_STRING__( victory ) );
		}

		uint64 winrate = 0;
		{
			auto rankinfo = rankinfos->add_pbstring();
			rankinfo->set_name( KFField::_win_rate );
			winrate = ( score - victory * 10000000000 ) / 10000000;
			rankinfo->set_value( __KF_STRING__( winrate ) );
		}

		{
			auto rankinfo = rankinfos->add_pbstring();
			rankinfo->set_name( KFField::_top_five );
			auto topfive = score - victory * 10000000000 - winrate * 10000000;
			rankinfo->set_value( __KF_STRING__( topfive ) );
		}
	}

	void KFRankClientModule::SerialzeKillRank( uint64 score, const KFMsg::PBStrings* pbstrings )
	{
		auto rankinfos = const_cast< KFMsg::PBStrings* >( pbstrings );
		uint64 kill = 0;
		{
			auto rankinfo = rankinfos->add_pbstring();
			rankinfo->set_name( KFField::_kill );
			kill = score / 10000000000;
			rankinfo->set_value( __KF_STRING__( kill ) );
		}

		uint64 evagekill = 0;
		{
			auto rankinfo = rankinfos->add_pbstring();
			rankinfo->set_name( KFField::_evage_kill );
			evagekill = ( score - kill * 10000000000 ) / 10000000;
			rankinfo->set_value( __KF_STRING__( evagekill ) );
		}

		{
			auto rankinfo = rankinfos->add_pbstring();
			rankinfo->set_name( KFField::_kd_score );
			auto kdscore = score - kill * 10000000000 - evagekill * 10000000;
			rankinfo->set_value( __KF_STRING__( kdscore ) );
		}


	}
}
