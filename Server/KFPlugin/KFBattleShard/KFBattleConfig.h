#ifndef __KF_BATTLE_CONFIG_H__
#define __KF_BATTLE_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

class KFAgent;

namespace KFrame
{
    class KFRewardSetting
    {
    public:
        KFRewardSetting()
        {
            _score = 0;
            _match_id = 0;
        }

    public:
        uint32 _score;
        uint32 _match_id;
        std::string _reward;
    };

    class KFBattleRewardSetting
    {
    public:
        std::map< uint32, KFRewardSetting > _reward_list;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFBattleConfig : public KFConfig, public KFSingleton< KFBattleConfig >
    {
    public:
        KFBattleConfig();
        ~KFBattleConfig();

        // 加载配置
        bool LoadConfig();

        // 查找结算奖励
        const KFRewardSetting* FindBattleReward( uint32 matchid, uint32 score );

    protected:
        // 添加结算奖励
        void AddBattleReward( uint32 matchid, uint32 rewardkey, std::string& rewardvalue );

    public:

        // 战斗结算奖励配置
        std::map< uint32, KFBattleRewardSetting > _battle_reward_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_battle_config = KFBattleConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif