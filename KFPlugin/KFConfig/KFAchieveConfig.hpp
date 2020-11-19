#ifndef	__KF_ACHIEVE_CONFIG_H__
#define	__KF_ACHIEVE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFAchieveSetting : public KFIntSetting
	{
	public:
		// 完成条件
		DynamicConditionGroupPtr _complete_condition;

		// 奖励
		KFElements _reward;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFAchieveConfig : public KFConfigT< KFAchieveSetting >, public KFInstance< KFAchieveConfig >
	{
	public:
		KFAchieveConfig()
		{
			_key_name = "id";
			_file_name = "achieve";
		}

		~KFAchieveConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _settings._objects )
			{
				auto kfsetting = iter.second;

				KFGlobal::Instance()->ParseElement( kfsetting->_reward, _file_name.c_str(), kfsetting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFAchieveSetting* kfsetting )
		{
			kfsetting->_complete_condition = xmlnode.ReadDynamicConditionGroup( "completecondition", true );
			kfsetting->_reward._str_parse = xmlnode.ReadString( "reward", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
