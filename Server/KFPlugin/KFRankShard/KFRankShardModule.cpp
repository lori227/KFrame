#include "KFRankShardModule.h"


namespace KFrame
{
#define __USE_WORKER__
#define __RANK_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_rank )
#define __PUBLIC_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_public )

	
#ifdef __USE_WORKER__
#define __REGISTER_RANK_MESSAGE__ __REGISTER_WORKER_MESSAGE__
#define __UNREGISTER_RANK_MESSAGE__ __UNREGISTER_WORKER_MESSAGE__
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_worker->SendMessageToClient( kfguid, msgid, message )
#else
#define __REGISTER_RANK_MESSAGE__ __REGISTER_MESSAGE__
#define __UNREGISTER_RANK_MESSAGE__ __UNREGISTER_MESSAGE__
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendMessageToClient( kfguid, msgid, message )
#endif

	KFRankShardModule::KFRankShardModule()
	{

	}

	KFRankShardModule::~KFRankShardModule()
	{

	}

	void KFRankShardModule::InitModule()
	{
	}

	void KFRankShardModule::BeforeRun()
	{
		__REGISTER_RANK_MESSAGE__( KFMsg::S2S_JOIN_RANK_LIST_REQ, &KFRankShardModule::HandleJoinRankListReq );
		__REGISTER_RANK_MESSAGE__( KFMsg::S2S_QUERY_WHOLE_RANK_LIST_REQ, &KFRankShardModule::HandleQueryWholeRanklistReq );
		__REGISTER_RANK_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &KFRankShardModule::HandleQueryFriendRanklistReq );
	}
	
	void KFRankShardModule::BeforeShut()
	{
		__UNREGISTER_RANK_MESSAGE__( KFMsg::S2S_JOIN_RANK_LIST_REQ );
		__UNREGISTER_RANK_MESSAGE__( KFMsg::S2S_QUERY_WHOLE_RANK_LIST_REQ );
		__UNREGISTER_RANK_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ );
	}

	void KFRankShardModule::OnceRun()
	{
		
	}

	// 处理打榜请求
	__KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleJoinRankListReq )
	{
		__PROTO_PARSE__( KFMsg::S2SJoinRankListReq );
		auto redisdriver = __RANK_REDIS_DRIVER__;
		auto scoretype = kfmsg.scoretype();
		std::string mainkey = GetRankMainKey( scoretype );
		if (mainkey.empty() )
		{
			return;
		}

		std::string total_score( KFField::_total_score );
		std::string totalkey = GetRankMainKey( total_score );
		if ( totalkey.empty() )
		{
			return;
		}

		auto pbsocres = &kfmsg.scores();
		AppendCommand( redisdriver, kfmsg.playerid(), mainkey, pbsocres );

		// 总榜
		auto pbtotalsocres = &kfmsg.totalscores();
		AppendCommand( redisdriver, kfmsg.playerid(), totalkey, pbtotalsocres );

		auto ok = redisdriver->PipelineExecute();
		if ( !ok )
		{
			KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player[%u] join rank[%s] failed!",
				__FUNCTION_LINE__, kfmsg.playerid(), scoretype.c_str() );
		}
	}


	std::string KFRankShardModule::GetRankMainKey( std::string& ranktype )
	{
		std::string mainkey = "";
		if ( ranktype == KFField::_single_score )
		{
			mainkey =  KFField::_single;
		}
		else if ( ranktype == KFField::_double_score )
		{
			mainkey = KFField::_double;
		}
		else if ( ranktype == KFField::_four_score )
		{
			mainkey = KFField::_four;
		}
		else if ( ranktype == KFField::_total_score )
		{
			mainkey = KFField::_total;
		}
		return mainkey;
	}


	std::string KFRankShardModule::MakeRankKey( std::string& mainkey, const std::string& scoretype )
	{
		std::string key = "";
		if ( scoretype == KFField::_eval_score )
		{
			key = mainkey + "_" + KFField::_eval_rank;
		}
		else if ( scoretype == KFField::_win_score )
		{
			key = mainkey + "_" + KFField::_win_rank;
		}
		else if ( scoretype == KFField::_kill_score )
		{
			key = mainkey + "_" + KFField::_kill_rank;
		}
		return key;
	}

	void KFRankShardModule::AppendCommand( KFRedisDriver*redisdriver, uint32 playerid, std::string& mainkey, const KFMsg::PBScoreData* scores )
	{
		for ( auto i = 0; i < scores->score_size(); ++i )
		{
			auto score = &scores->score( i );
			std::string rankkey = MakeRankKey( mainkey, score->name() );
			redisdriver->AppendCommand( "zadd %s %u %u", rankkey.c_str(), score->value(), playerid );
		}
	}


	__KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryWholeRanklistReq )
	{
		__PROTO_PARSE__( KFMsg::S2SQueryWholeRankListReq );
		auto redisdriver = __RANK_REDIS_DRIVER__;
		auto publicrediver = __PUBLIC_REDIS_DRIVER__;
		auto ranktype = _kf_option->GetString( "ranktype", kfmsg.matchid() );
		auto mainkey = GetRankMainKey( ranktype );
		if ( mainkey.empty() )
		{
			return KFLogger::LogLogic( KFLogger::Error, "[%s:%u] Make wholerank key err rankid：%u ranktype:%s!",
				__FUNCTION_LINE__, kfmsg.matchid(), ranktype.c_str() );
		}

		auto rankkey = mainkey + "_" + kfmsg.ranktype();
		//auto rankkey = MakeRankKey( mainkey, kfmsg.ranktype() );
		MapString ranklist;
		redisdriver->MapExecute( ranklist,"zrevrange %s %u %u WITHSCORES", rankkey.c_str(), kfmsg.begin(), kfmsg.end() );
		if ( ranklist.empty() )
		{
			return;
		}

		// redis cmds
		VectorString fileds;
		VectorString playerids;
		fileds.push_back( KFField::_name );
		fileds.push_back( KFField::_icon );

		auto queryplayercmd = "hmget %s:%s %s %s";
		for ( auto& iter : ranklist )
		{
			playerids.push_back( iter.first );
		}

		VectorString playerinfos;
		if ( !QueryRoleInfoBatch( playerinfos, playerids, fileds ) )
		{
			return KFLogger::LogSystem( KFLogger::Error, "[%s:%u] redis exec err!", __FUNCTION_LINE__ );
		}

		auto cursor = 0;
		KFMsg::S2SQueryRankListAck ack;
		ack.set_matchid( kfmsg.matchid() );
		ack.set_ranktype( kfmsg.ranktype() );
		ack.set_playerid( kfmsg.playerid() );

		auto pbrankdata = ack.mutable_rankdatas();
		for ( auto& iter : ranklist )
		{
			auto rankdata =  pbrankdata->add_rankdata();
			{	
				auto pbstring = rankdata->add_pbstring();
				pbstring->set_name( KFField::_id );
				pbstring->set_value( iter.first );
			}
			{
				auto pbstring = rankdata->add_pbstring();
				pbstring->set_name( KFField::_score );
				pbstring->set_value( iter.second );
			}
			// 拼接角色信息
			FormatRoleInfoToPB( rankdata , playerinfos, cursor, fileds );
		}
		__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_RANK_LIST_ACK ,&ack );
	}

	__KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryFriendRanklistReq )
	{
		__PROTO_PARSE__( KFMsg::S2SQueryFriendRankListReq );
		auto redisdriver = __RANK_REDIS_DRIVER__;
		auto publicrediver = __PUBLIC_REDIS_DRIVER__;
		auto ranktype = _kf_option->GetString( "ranktype", kfmsg.matchid() );
		auto mainkey = GetRankMainKey( ranktype );
		if ( mainkey.empty() )
		{
			return;
		}
		auto rankkey = mainkey + "_" + kfmsg.ranktype();
		//auto rankkey = MakeRankKey( mainkey, kfmsg.ranktype() );
		// 查询
		ListString queryrankcmds;
		auto queryrankcmd = "zscore %s %u";

		char strsql[ 256 ];

		auto playerids = &kfmsg.playerids();
		VectorString playeridvec;

		for ( auto i = 0; i < playerids->playerid_size(); ++i )
		{
			if ( _invalid_int == playerids->playerid( i ) )
			{
				continue;
			}
			memset( strsql, 0, 256 );
			snprintf( strsql, 256, queryrankcmd, rankkey.c_str(), playerids->playerid( i ) );
			queryrankcmds.push_back( strsql );
			playeridvec.push_back( __KF_STRING__( playerids->playerid( i ) ) );
		}
		VectorString rankscores;
		auto ok =  publicrediver->PipelineExecute( queryrankcmds, rankscores );
		if ( !ok || rankscores.size() != queryrankcmds.size() )
		{
			return KFLogger::LogSystem( KFLogger::Error, "[%s:%u] redis exec err ranksize:%u,cmdsize:%u!",
				__FUNCTION_LINE__, queryrankcmds.size(), rankscores.size() );
			
		}

		VectorString fileds;
		fileds.push_back( KFField::_name );
		fileds.push_back( KFField::_icon );
		VectorString playerinfos;
		if ( !this->QueryRoleInfoBatch( playerinfos, playeridvec, fileds ) )
		{
			return  KFLogger::LogSystem( KFLogger::Error, "[%s:%u] redis exec err!", __FUNCTION_LINE__ );
		}


		auto cursor = 0;
		KFMsg::S2SQueryRankListAck ack;
		ack.set_matchid( kfmsg.matchid() );
		ack.set_ranktype( kfmsg.ranktype() );
		ack.set_playerid( kfmsg.playerid() );
		ack.set_isfriendrank( true );
		auto pbrankdata = ack.mutable_rankdatas();
		for ( auto i = 0; i < playerids->playerid_size(); ++i )
		{
			if ( _invalid_int == playerids->playerid( i ) )
			{
				continue;
			}

			auto rankdata = pbrankdata->add_rankdata();
			{
				auto pbstring = rankdata->add_pbstring();
				pbstring->set_name( KFField::_id );
				pbstring->set_value( __KF_STRING__( playerids->playerid( i ) ) );
			}
			{
				auto pbstring = rankdata->add_pbstring();
				pbstring->set_name( KFField::_score );
				pbstring->set_value( rankscores[i] );
			}
			// 拼接角色信息
			FormatRoleInfoToPB( rankdata, playerinfos, cursor, fileds );
		}
		__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_RANK_LIST_ACK, &ack );
	}


	bool KFRankShardModule::QueryRoleInfoBatch( VectorString& playerinfos, VectorString& playerids, VectorString& fileds )
	{
		auto publicrediver = __PUBLIC_REDIS_DRIVER__;
		ListString cmds;
		char* queryplayercmd = "hmget %s:%s %s %s";
		char strsql[ 256 ];
		
		for ( auto& iter : playerids )
		{
			memset( strsql, 0, 256 );
			snprintf( strsql, 256, queryplayercmd, KFField::_public.c_str(), iter.c_str(), fileds[ 0 ].c_str(), fileds[ 1 ].c_str() );
			cmds.push_back( strsql );
		}
		return publicrediver->PipelineExecute( cmds, playerinfos );
	}

	void KFRankShardModule::FormatRoleInfoToPB( KFMsg::PBStrings* pbrank, VectorString& playerinfos, int& cursor, VectorString& fileds )
	{
		if ( cursor >= playerinfos.size() )
		{
			return KFLogger::LogSystem( KFLogger::Error, "[%s:%u] AddRoleInfo to RankData Error: cursor:%u size:%u",
				__FUNCTION_LINE__, cursor, playerinfos.size() );
		}

		{
			auto pbstring = pbrank->add_pbstring();
			pbstring->set_name( fileds[ 0 ] );
			pbstring->set_value( playerinfos[ cursor ] );
		}
		{
			auto pbstring = pbrank->add_pbstring();
			pbstring->set_name( fileds[ 1 ] );
			pbstring->set_value( playerinfos[ cursor + 1 ] );

		}
		cursor += fileds.size();
	}
}