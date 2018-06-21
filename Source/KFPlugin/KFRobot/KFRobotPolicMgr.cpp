#include "KFRobotPolicMgr.h"

namespace KFrame
{
	KFRobotPolicMgr::KFRobotPolicMgr()
	{
	}

	KFRobotPolicMgr::~KFRobotPolicMgr()
	{
		for ( auto& iter : _storeSettings )
		{
			__KF_DESTROY__( StoreSetting, iter );
		}
		_storeSettings.clear();

	}

	void KFRobotPolicMgr::Init()
	{
		//商城
		{
			pStoreSetting pStore = __KF_CREATE__( StoreSetting );
			pStore->buytype = "money";
			pStore->shopid = 10001;
			pStore->num = 1;
			//_storeSettings.push_back( pStore );
		}
		{
			pStoreSetting pStore = __KF_CREATE__( StoreSetting );
			pStore->buytype = "money";
			pStore->shopid = 10002;
			pStore->num = 1;
			_storeSettings.push_back( pStore );
		}

		{
			pStoreSetting pStore = __KF_CREATE__( StoreSetting );
			pStore->buytype = "diamond";
			pStore->shopid = 10014;
			pStore->num = 1;
			_storeSettings.push_back( pStore );
		}
		//活动
		{
			pActivitySetting pActivity = __KF_CREATE__( ActivitySetting );
			pActivity->type = 1;
			pActivity->id = 1;
			_activitySettings.push_back( pActivity );
		}

		_oddbase = _invalid_int;
		_evenbase = _invalid_int;
		_players.clear();
	}

	void KFRobotPolicMgr::PushSortRoleInfo( uint32 playerid )
	{
		if (_invalid_int == playerid % 2 )
		{
			if (_evenplayer.find( playerid ) == _evenplayer.end() )
			{
				_evenplayer.insert( t_uintmap ::value_type( playerid,_evenbase));
				_evenbase++;
			}
		}
		else
		{
			if ( _oddplayer.find( _oddbase ) == _oddplayer.end() )
			{
				_oddplayer.insert( t_uintmap::value_type( _oddbase, playerid ) );
				_oddbase++;
			}
		}
	}

	void KFRobotPolicMgr::AddRoleInfo( uint32 playerid, KFEntity* player )
	{
		if ( _players.end() == _players.find(playerid))
		{
			_players.insert(t_player_map::value_type( playerid , player ));
		}
	}

	void KFRobotPolicMgr::RemoveRole( uint32 playerid )
	{
		if ( _players.end() != _players.find( playerid ) )
		{
			_players.erase( playerid );
		}
	}

	KFEntity* KFRobotPolicMgr::FindRoleById( uint32 playerid )
	{
		if ( _players.end() == _players.find(playerid))
		{
			return nullptr;
		}
		return _players[ playerid ];
	}


	uint32 KFRobotPolicMgr::GetFetterRole( uint32 playerid )
	{
		if ( _invalid_int == playerid % 2 &&  _evenplayer.end() != _evenplayer.find(playerid) )
		{
			auto pose = _evenplayer[ playerid ];
			return _oddplayer[ pose ];
		}
		return _invalid_int;
	}

	bool KFRobotPolicMgr::GetStoreSetting( std::string& buytype, uint32& shopid, uint32& num, int cursor )
	{
		pStoreSetting p = _storeSettings[ cursor ];
		if (nullptr == p)
		{
			return false;
		}
		buytype = p->buytype;
		shopid = p->shopid;
		num = p->num;
		return true;
	}

	bool KFRobotPolicMgr::GetAllFetterRole( std::vector<uint32>& roleids, uint32 playerid )
	{
		roleids.clear();
		if ( _invalid_int == playerid % 2 )
		{
			for (auto& iter: _oddplayer )
			{
				roleids.push_back( iter.second);
			}
			return true;
		}
		return false;
	}

	void KFRobotPolicMgr::InitOddList( std::string& oddlist )
	{
		auto temp = oddlist;
		pNode p,s;
		p = _oddStateList;
		auto strfirstvalue = KFUtility::SplitString( temp, "," );
		auto firstvalue = 0;
		firstvalue = KFUtility::ToValue< uint32 >( strfirstvalue );
		p->_state = firstvalue;

		while ( !temp.empty() )
		{
			auto strvalue = KFUtility::SplitString( temp, "," );
			auto value = 0;
			value = KFUtility::ToValue< uint32 >( strvalue );
			s = new Node();
			s->_state = value;
			s->next = nullptr;
			p->next = s;
			s->prev = p;
			p = s;
		}
		p->next = _oddStateList;
		_oddStateList->prev = p;
	}

	void KFRobotPolicMgr::InitEvenList( std::string& evenlist )
	{
		auto temp = evenlist;
		pNode p, s;
		p = _evenStateList;
		auto strfirstvalue = KFUtility::SplitString( temp, "," );
		auto firstvalue = 0;
		firstvalue = KFUtility::ToValue< uint32 >( strfirstvalue );
		p->_state = firstvalue;

		while ( !temp.empty() )
		{
			auto strvalue = KFUtility::SplitString( temp, "," );
			auto value = 0;
			value = KFUtility::ToValue< uint32 >( strvalue );
			s = new Node();
			s->_state = value;
			s->next = nullptr;
			p->next = s;
			s->prev = p;
			p = s;
		}
		p->next = _evenStateList;
		_evenStateList->prev = p;
	}

	void KFRobotPolicMgr::InitStateList( std::string& oddlist, std::string& evenlist )
	{
		int oddsize = std::count(oddlist.begin(),oddlist.end(),',') + 1;
		int evensize = std::count( evenlist.begin(), evenlist.end(), ',' ) + 1;
		_oddStateList = __KF_CREATE__( Node );
		_oddStateList->next = nullptr;
		_oddStateList->prev = nullptr;
		InitOddList( oddlist );

		_evenStateList = __KF_CREATE__( Node );
		_evenStateList->next = nullptr;
		_evenStateList->prev = nullptr;
		InitEvenList( evenlist );
	}

	pNode KFRobotPolicMgr::GetStateListById( uint32 playerid )
	{
		return ( _invalid_int == playerid % 2 ? _evenStateList : _oddStateList );
	}


	void KFRobotPolicMgr::GetPlayerName( uint32 playerid , std::string& name )
	{
		if ( nullptr != _players[playerid])
		{
			auto kfobject = _players[ playerid ]->GetData();
			if (kfobject)
			{
				auto basicobj = kfobject->FindData(KFField::_basic);
				name = basicobj->GetValue<std::string>(KFField::_name);
			}
		}

	}
}

