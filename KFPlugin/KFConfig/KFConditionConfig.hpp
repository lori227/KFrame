#ifndef	__KF_CONDITION_CONFIG_H__
#define	__KF_CONDITION_CONFIG_H__

#include "KFConfig.h"
#include "KFConditionDefineConfig.hpp"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFConditionSetting : public KFIntSetting
	{
	public:
		// 完成条件定义
		KFConditionDefineOption _condition;

		// 条件完成数值
		uint32 _done_type = 0u;

		// 计数累计限制
		uint32 _done_value = 0u;

		// 清除计数条件定义
		KFConditionDefineOption _clean;

		// 动态限制条件
		DynamicConditionGroupPtr _dynamic_condition_limit;

		// 限制条件
		StaticConditionListPtr _static_condition_limit;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFConditionConfig : public KFConfigT< KFConditionSetting >, public KFInstance< KFConditionConfig >
	{
	public:
		KFConditionConfig()
		{
			_key_name = "id";
			_file_name = "condition";
		}

		~KFConditionConfig() = default;

		virtual void LoadAllComplete()
		{
			for ( auto& iter : _settings._objects )
			{
				auto kfsetting = iter.second;

				kfsetting->_condition._condition_define = KFConditionDefineConfig::Instance()->FindSetting( kfsetting->_condition._str_condition );
				if ( !kfsetting->_condition._str_condition.empty() && kfsetting->_condition._condition_define == nullptr )
				{
					__LOG_ERROR__( "condition=[{}] can't find define=[{}]", kfsetting->_id, kfsetting->_condition._str_condition );
				}

				kfsetting->_clean._condition_define = KFConditionDefineConfig::Instance()->FindSetting( kfsetting->_clean._str_condition );
				if ( !kfsetting->_clean._str_condition.empty() && kfsetting->_clean._condition_define == nullptr )
				{
					__LOG_ERROR__( "condition=[{}] can't find define=[{}]", kfsetting->_id, kfsetting->_clean._str_condition );
				}

			}
		}

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFConditionSetting* kfsetting )
		{
			kfsetting->_condition._str_condition = xmlnode.ReadString( "condition", true );
			kfsetting->_done_type = xmlnode.ReadUInt32( "donetype", true );
			kfsetting->_done_value = xmlnode.ReadUInt32( "donevalue", true );
			kfsetting->_clean._str_condition = xmlnode.ReadString( "clean", true );
			kfsetting->_dynamic_condition_limit = xmlnode.ReadDynamicConditionGroup( "dynamicconditionlimit", true );
			kfsetting->_static_condition_limit = xmlnode.ReadStaticConditionList( "staticconditionlimit", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
