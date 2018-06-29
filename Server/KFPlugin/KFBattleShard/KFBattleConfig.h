#ifndef __KF_BATTLE_CONFIG_H__
#define __KF_BATTLE_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

class KFAgent;

namespace KFrame
{
    class KFBattleRewardSrtting
    {
    public:
        KFBattleRewardSrtting();

    public:
        uint32 _match_id;
        std::map<uint32, std::string> _score_reward;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFBattleConfig : public KFConfig, public KFSingleton< KFBattleConfig >
    {
    public:
        KFBattleConfig();
        ~KFBattleConfig();

        // 加载配置
        bool LoadConfig( const char* file );

        // 加载奖励配置
        bool LoadRewardConfig( const char* file );

        // 添加结算奖励
        void AddBattleReward( uint32 matchid, uint32 rewardkey, std::string& rewardvalue );

        // 查找结算奖励
        std::string FindBattleReward( uint32 matchid, uint32 score );

        // 获取评分计算系数
        double GetScoreParam( const std::string& paramname );
    public:
        // 申请分配战场的人数
        uint32 _min_open_room_camp_count;

        // 等待进入时间
        uint32 _wait_enter_time;

        // 等待确认时间
        //uint32 _wait_confirm_time;

        // 战场的有效时间
        uint64 _room_valid_time;

        // 奖励配置的文件名
        std::string _reward_file;

        // 评分计算参数
        std::map< std::string, double > _score_param;

        // 战斗结算奖励配置
        std::map< uint32, KFBattleRewardSrtting > _battle_reward;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_battle_config = KFBattleConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif