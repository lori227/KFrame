#include "KFRankClientConfig.h"

namespace KFrame
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	KFRankClientConfig::KFRankClientConfig()
	{

	}

	KFRankClientConfig::~KFRankClientConfig()
	{
		
	}
	
	bool KFRankClientConfig::LoadConfig( const char* file )
	{
		_kf_rank_setting.Clear();

		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();
			//////////////////////////////////////////////////////////////////
			auto xmlnode = config.FindNode( "Setting" );
			while ( xmlnode.IsValid() )
			{
				auto kfsetting = __KF_CREATE__( KFRankSetting );

				kfsetting->_match_id = xmlnode.GetUInt32( "MatchId" );
				kfsetting->_rank_id = xmlnode.GetUInt32( "RankId" );
				_kf_rank_setting.Insert( kfsetting->_match_id, kfsetting );

				xmlnode.NextNode();
			}
			//////////////////////////////////////////////////////////////////
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}

	const KFRankSetting* KFRankClientConfig::FindRankSetting( uint32 matchid ) const
	{
		return _kf_rank_setting.Find( matchid );
	}
}