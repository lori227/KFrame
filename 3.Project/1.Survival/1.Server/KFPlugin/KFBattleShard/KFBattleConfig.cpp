#include "KFBattleConfig.h"

namespace KFrame
{
    bool KFBattleConfig::LoadConfig()
    {
        _battle_reward_list.clear();

        ////////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto matchid = xmlnode.GetUInt32( "MatchId" );
            auto rewardkey = xmlnode.GetUInt32( "RewardKey" );
            auto rewardvalue = xmlnode.GetString( "RewardValue" );
            AddBattleReward( matchid, rewardkey, rewardvalue );

            xmlnode.NextNode();
        }
        ////////////////////////////////////////////////////////////////////

        return true;
    }

    void KFBattleConfig::AddBattleReward( uint32 matchid, uint32 rewardkey, std::string& rewardvalue )
    {
        auto iter = _battle_reward_list.find( matchid );
        if ( iter == _battle_reward_list.end() )
        {
            iter = _battle_reward_list.insert( std::make_pair( matchid, KFBattleRewardSetting() ) ).first;
        }

        KFRewardSetting kfsetting;
        kfsetting._match_id = matchid;
        kfsetting._score = rewardkey;
        kfsetting._reward = rewardvalue;
        iter->second._reward_list.insert( std::make_pair( rewardkey, kfsetting ) );
    }

    const KFRewardSetting* KFBattleConfig::FindBattleReward( uint32 matchid, uint32 score )
    {
        auto matchiter = _battle_reward_list.find( matchid );
        if ( matchiter == _battle_reward_list.end() )
        {
            return nullptr;
        }

        auto& scorereward = matchiter->second;
        auto scoreiter = scorereward._reward_list.lower_bound( score );
        if ( scoreiter == scorereward._reward_list.end() )
        {
            return nullptr;
        }

        return &scoreiter->second;
    }
}