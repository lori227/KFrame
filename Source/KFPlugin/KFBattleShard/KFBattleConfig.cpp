#include "KFBattleConfig.h"

namespace KFrame
{
	KFBattleRewardSrtting::KFBattleRewardSrtting()
	{
		_match_id = 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	KFBattleConfig::KFBattleConfig()
	{
		_wait_enter_time = 90000;
		_min_open_room_camp_count = 2;
		_room_valid_time = 3600000;
		_reward_file = "battlereward.config";
	}

	KFBattleConfig::~KFBattleConfig()
	{
		
	}
	
	bool KFBattleConfig::LoadConfig( const char* file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();

			//////////////////////////////////////////////////////////////////
			auto xmlnode = config.FindNode( "Setting" );
			_min_open_room_camp_count = xmlnode.GetUInt32( "MinOpenRoomCount" );
			_room_valid_time = xmlnode.GetUInt32( "RoomValidTime" ) * TimeEnum::OneMinuteMicSecond;
			_wait_enter_time = xmlnode.GetUInt32( "WaitEnterTime" ) * TimeEnum::OneMinuteMicSecond;
			_reward_file = xmlnode.GetString( "RewardFile" );

			//////////////////////////////////////////////////////////////////
			auto paramsnode = config.FindNode( "Params" );
			auto itemnode = paramsnode.FindNode( "Item" );
			while ( itemnode.IsValid() )
			{
				auto paramname = itemnode.GetString( "Name" );
				auto paramvalue = itemnode.GetDouble( "Value" );
				_score_param[ paramname ] = paramvalue;

				itemnode.NextNode();
			}
		}
		catch ( ... )
		{
			return false;
		}

		LoadRewardConfig( _reward_file.c_str() );
		return true;
	}

	bool KFBattleConfig::LoadRewardConfig( const char * file )
	{
		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();

			////////////////////////////////////////////////////////////////////
			auto xmlnode = config.FindNode( "Setting" );
			while ( xmlnode.IsValid() )
			{
				auto matchid = xmlnode.GetUInt32( "MatchId" );
				auto rewardkey = xmlnode.GetUInt32( "RewardKey" );
				auto rewardvalue = xmlnode.GetString( "RewardValue" );
				AddBattleReward( matchid, rewardkey, rewardvalue );

				xmlnode.NextNode();
			}
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}

	void KFBattleConfig::AddBattleReward( uint32 matchid, uint32 rewardkey, std::string & rewardvalue )
	{
		auto rewarditer = _battle_reward.find( matchid );
		if ( rewarditer == _battle_reward.end() )
		{
			KFBattleRewardSrtting kfsetting;
			kfsetting._match_id = matchid;
			kfsetting._score_reward.insert( std::make_pair( rewardkey, rewardvalue ) );
			_battle_reward.insert( std::make_pair( matchid, kfsetting ) );
		}
		else
		{
			rewarditer->second._score_reward.insert( std::make_pair( rewardkey, rewardvalue ) );
		}
	}

	std::string KFBattleConfig::FindBattleReward( uint32 matchid, uint32 score )
	{
		auto matchiter = _battle_reward.find( matchid );
		if ( matchiter == _battle_reward.end() )
		{
			return "";
		}
		auto scorereward = matchiter->second;
		auto scoreiter = scorereward._score_reward.lower_bound( score );
		if ( scoreiter == scorereward._score_reward.end() )
		{
			scoreiter = scorereward._score_reward.begin();
		}

		return scoreiter->second;
	}

	double KFBattleConfig::GetScoreParam( const std::string& paramname )
	{
		auto scoreiter = _score_param.find( paramname );
		if ( scoreiter != _score_param.end() )
		{
			return scoreiter->second;
		}
		return 0.0;
	}
}