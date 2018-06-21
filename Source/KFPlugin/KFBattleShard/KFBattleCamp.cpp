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

	bool KFBattleCamp::HavePlayer( uint32 playerid )
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

			KFLogger::LogLogic( KFLogger::Debug, "[%s] camp[%u] add player[%u]!",
				__FUNCTION__, _camp_id, kfplayer->GetID() );
		}
	}

	void KFBattleCamp::RunEnterBattleRoom( KFBattleRoom* kfroom )
	{
		auto kfplayer = _kf_player_list.First();
		while ( kfplayer != nullptr )
		{
			kfplayer->RunEnterRoom( kfroom );
			kfplayer = _kf_player_list.Next();
		}
	}

	void KFBattleCamp::FinishLeaveBattleRoom()
	{
		auto kfplayer = _kf_player_list.First();
		while ( kfplayer != nullptr )
		{
			kfplayer->FinishLeaveRoom();
			kfplayer = _kf_player_list.Next();
		}
	}
}