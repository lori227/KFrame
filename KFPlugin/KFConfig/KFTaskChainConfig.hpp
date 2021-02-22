#ifndef	__KF_TASKCHAIN_CONFIG_H__
#define	__KF_TASKCHAIN_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	class TaskChainData
	{
	public:
		// 任务序列
		uint32 _index = 0u;

		// 类型
		uint32 _type = 0u;

		// 任务id/任务池id
		uint32 _task = 0u;

		// 任务状态
		uint32 _task_status = 0u;

		// 附加开启类型
		uint32 _extend_type = 0u;

		// 附加开启任务链id/池
		uint32 _extend_chain = 0u;

	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFTaskChainSetting : public KFIntSetting
	{
	public:
		// 任务链数据
		std::vector<TaskChainData> _task_chain_data;
	};

	/////////////////////////////////////////////////////////////////////////////////
	class KFTaskChainConfig : public KFConfigT< KFTaskChainSetting >, public KFInstance< KFTaskChainConfig >
	{
	public:
		KFTaskChainConfig()
		{
			_key_name = "id";
			_file_name = "taskchain";
		}

		~KFTaskChainConfig() = default;

	protected:
		virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFTaskChainSetting> setting )
		{
		
			TaskChainData taskchaindata;
			taskchaindata._index = xml_node.ReadUInt32( "index", true );
			taskchaindata._type = xml_node.ReadUInt32( "type", true );
			taskchaindata._task = xml_node.ReadUInt32( "task", true );
			taskchaindata._task_status = xml_node.ReadUInt32( "taskstatus", true );
			taskchaindata._extend_type = xml_node.ReadUInt32( "extendtype", true );
			taskchaindata._extend_chain = xml_node.ReadUInt32( "extendchain", true );
			setting->_task_chain_data.push_back( taskchaindata );
		}

	};

	/////////////////////////////////////////////////////////////////////////////////
}
#endif
