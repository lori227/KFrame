#ifndef __KF_ACHIEVE_CONFIG_H__
#define __KF_ACHIEVE_CONFIG_H__

#include "KFConfig.h"
#include "KFElementConfig.h"

namespace KFrame
{
    class KFAchieveSetting : public KFIntSetting
    {
    public:
        // 完成条件
        KFConditionGroup _complete_condition_group;

        // 奖励
        KFElements _rewards;
    };
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    class KFAchieveConfig : public KFConfigT< KFAchieveSetting >, public KFInstance< KFAchieveConfig >
    {
    public:
        KFAchieveConfig()
        {
            _file_name = "achieve";
        }

        virtual void LoadAllComplete()
        {
            for ( auto& iter : _settings._objects )
            {
                auto kfsetting = iter.second;
                KFElementConfig::Instance()->ParseElement( kfsetting->_rewards, __FUNC_LINE__ );
            }
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFAchieveSetting* kfsetting )
        {
            kfsetting->_rewards._str_parse = xmlnode.ReadString( "Reward" );
            kfsetting->_complete_condition_group = xmlnode.ReadConditionGroup( "CompleteCondition" );
        }
    };
}

#endif