#ifndef	__KF_TASK_CONFIG_H__
#define	__KF_TASK_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFTaskSetting : public KFIntSetting
	{
	public:
		// 类型
		uint32 _type = 0u;

		// 品质
		uint32 _quality = 0u;

		// 完成方式
		uint32 _complete_mode = 0u;

		// 完成条件
		DynamicConditionGroupPtr _complete_condition;

		// 任务输出
		UInt32Vector _execute;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFTaskConfig : public KFConfigT< KFTaskSetting >, public KFInstance< KFTaskConfig >
	{
	public:
		KFTaskConfig()
		{
			_key_name = "id";
			_file_name = "task";
		}

		~KFTaskConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xmlnode, KFTaskSetting* kfsetting )
		{
			kfsetting->_type = xmlnode.ReadUInt32( "type", true );
			kfsetting->_quality = xmlnode.ReadUInt32( "quality", true );
			kfsetting->_complete_mode = xmlnode.ReadUInt32( "completemode", true );
			kfsetting->_complete_condition = xmlnode.ReadDynamicConditionGroup( "completecondition", true );
			kfsetting->_execute = xmlnode.ReadUInt32Vector( "execute", true );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
