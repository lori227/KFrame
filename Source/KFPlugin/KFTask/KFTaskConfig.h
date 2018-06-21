#ifndef __KF_TASK_CONFIG_H__
#define __KF_TASK_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFAgent.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	class KFTaskSetting
	{
	public:
		KFTaskSetting()
		{
			_start_hour = 0;
			_start_minute = 0;
			_end_hour = 0;
			_end_minute = 0;
		}

		uint32 _id;			    // 任务id
		uint32 _init_value;		// 初始值
		std::string _done_type;	// 完成类型
		uint32 _done_key;		// 完成条件
		uint32 _done_value;		// 完成数量
		uint32 _level;		    // 最低等级
		uint32 _start_hour;	    // 开始时间
		uint32 _start_minute;	// 开始时间
		uint32 _end_hour;	    // 结束时间
		uint32 _end_minute;  	// 结束时间
		KFAgents _rewards;		// 奖励

		uint32 _next_id;		// 下一个任务id( 剧情任务 )
		uint32 _next_value;		// 下一个任务数值( 0 表示不继承 )
	};

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	class KFTaskConfig : public KFConfig, public KFSingleton< KFTaskConfig >
	{
	public:
		// 获得活动配置
		const KFTaskSetting* FindTaskSetting( uint32 taskid ) const;

		// 加载配置
		bool LoadConfig( const char* file );

	protected:
		// 添加任务配置
		void AddTaskSetting( KFTaskSetting* tasksetting );

	public:
		// 任务列表
		KFMap< uint32, uint32, KFTaskSetting > _task_setting;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	inline void KFTaskConfig::AddTaskSetting( KFTaskSetting* kfsetting )
	{
		_task_setting.Insert( kfsetting->_id, kfsetting );
	}

	inline const KFTaskSetting* KFTaskConfig::FindTaskSetting( uint32 taskid ) const
	{
		return _task_setting.Find( taskid );
	}

	///////////////////////////////////////////////////////////////////////////
	static auto _kf_task_config = KFTaskConfig::Instance();
	///////////////////////////////////////////////////////////////////////////
}
#endif