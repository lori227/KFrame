#ifndef __KF_ACTIVITY_CONFIG_H__
#define __KF_ACTIVITY_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFAgent.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	namespace KFActivityEnum
	{
		// 活动函数枚举
		enum EConstDefine
		{
			DynamicTime = 1,	// 动态时间
			StaticTime = 2,		// 静态时间
			//////////////////////////////////////////////////////////////
			ResetDaily = 1,		// 每日重置
			//////////////////////////////////////////////////////////////
			UseOperate = 1,		// 更新使用操作的值
			UseFinal = 2,		// 使用最终的值
		};
	}
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFActivityData
	{
	public:

		uint32 _id;
		uint32 _done_value;
		KFAgents _rewards;		// 奖励
	};

	__ST_CLASS__( KFActivitySetting )
	{
	public:
		KFActivitySetting() : _max_value( 0 )
		{
		}

		~KFActivitySetting()
		{

		}

		uint32 _type;				// 活动类型
		uint32 _time_type;			// 时间类型
		uint32 _duration;			// 持续时间
		uint64 _start_time;			// 开始时间
		uint64 _end_time;			// 结束时间
		uint32 _reset_type;			// 重置类型
		uint32 _max_value;			// 活动最大值	

		uint32 _level;				// 等级限制
		std::string _parent_name;	// 类型
		std::string _data_name;		// 属性
		uint32 _trigger_type;		// 触发条件
		uint32 _trigger_value;		// 触发值
		uint32 _use_type;			// 使用的类型
		uint32 _use_value;			// 使用的实际数值
		uint32 _key;				// 键值
		uint32 _operate;			// 操作类型

		std::map< uint64, KFActivityData* > _datas;  // 活动数据

		const KFActivityData* FindActivityData( uint32 id ) const;
		void AddActivityData( KFActivityData* data );

		// 判断是否在时间内
		bool CheckInTime( uint64 nowtime, uint64 createtime ) const;
		bool CheckResetActivity( uint64 nowtime, uint64 createtime, uint64 eventtime ) const;

		// 判断是否可以更新
		bool CheckCanUpdate( uint32 key, uint32 level, uint32 operate ) const;
		uint32 CheckTriggerValue( uint32 operatevalue, uint32 nowvalue ) const;
		uint32 GetUseValue( uint32 operatevalue ) const;
	};

	///////////////////////////////////////////////////////////////////////////////

	class KFActivityType
	{
	public:
		inline void AddKFAchieveType( KFActivitySetting* setting )
		{
			_activity_type.push_back( setting );
		}

	public:
		// 类型列表
		std::vector< KFActivitySetting* > _activity_type;
	};

	///////////////////////////////////////////////////////////////////////////////

	class KFActivityConfig : public KFConfig, public KFSingleton< KFActivityConfig >
	{
	public:
		KFActivityConfig();
		~KFActivityConfig();

		// 获得活动配置
		const KFActivitySetting* FindActivitySetting( uint32 type );
		const KFActivityData* FindActivityData( uint32 type, uint32 id );

		// 获得某种类型的所有成就
		const KFActivityType* FindActivityType( const std::string& parentname, const std::string& dataname );

		bool LoadConfig( const char* file );

	protected:
		void AddActivitySetting( KFActivitySetting* setting, KFActivityData* eventdata );
		void AddActivityType( KFActivitySetting* setting );

	public:
		// 活动列表
		KFMap< uint32, uint32, KFActivitySetting > _activity_setting;

		// 触发列表
		typedef std::pair< std::string, std::string > ActivityTypeKey;
		std::map< ActivityTypeKey, KFActivityType > _activity_types;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_activity_config = KFActivityConfig::Instance();
}

#endif