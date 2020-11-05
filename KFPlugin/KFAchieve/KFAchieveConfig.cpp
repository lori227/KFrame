#include "KFAchieveConfig.hpp"
#include "KFXmlReader/KFReadSetting.h"

namespace KFrame
{
    void KFAchieveConfig::ReadSetting( KFNode& xmlnode, KFAchieveSetting* kfsetting )
    {
        auto strcompletecondition = xmlnode.GetString( "CompleteCondition" );
        kfsetting->_complete_condition_type = KFReadSetting::ParseConditionList( strcompletecondition, kfsetting->_complete_condition );

        kfsetting->_rewards._str_parse = xmlnode.GetString( "Reward" );
    }

    void KFAchieveConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_rewards, __FUNC_LINE__ );
        }
    }
}