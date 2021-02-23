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
	class KFAchieveConfig : public KFConfigT<KFAchieveSetting>, public KFInstance<KFAchieveConfig>
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
			for ( auto& iter : _setting_list._objects )
			{
				auto setting = iter.second;

				KFGlobal::Instance()->ParseElement( setting->_reward, _file_name.c_str(), setting->_row );
			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFAchieveSetting> setting )
		{
			setting->_complete_condition = xml_node.ReadDynamicConditionGroup( "completecondition", true );
			setting->_reward._str_parse = xml_node.ReadString( "reward", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
