#include "KFAchieveConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFAchieveConfig::ReadSetting( KFNode& xmlnode, KFAchieveSetting* kfsetting )
    {
        auto strcompletecondition = xmlnode.GetString( "CompleteCondition" );
        kfsetting->_complete_condition_type = KFReadSetting::ParseConditionList( strcompletecondition, kfsetting->_complete_condition );

        auto strreward = xmlnode.GetString( "Reward" );
        kfsetting->_rewards.Parse( strreward, __FUNC_LINE__ );
    }
}