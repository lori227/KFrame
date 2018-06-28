#ifndef __KF_REWEARD_H__
#define __KF_REWEARD_H__

#include "KFrame.h"

namespace KFrame
{
	class KFData;
	__ST_CLASS__( KFAgentValue )
	{
	public:
		KFAgentValue();

		bool SetValue( const std::string& dataname, std::string value );
		uint64 GetValue( float multiple = 1.0f );
		uint64 GetUseValue();
		//////////////////////////////////////////////////////////////////
	public:
		std::string _data_name;
		uint64 _min_value;
		uint64 _max_value;
		uint64 _use_value;
	};

	class KFAgents;
	__ST_CLASS__( KFAgent )
	{
	public:
		KFAgent();
		~KFAgent();

		// non-copy 
	private:
		KFAgent( const KFAgent& other );
		KFAgent& operator=( const KFAgent& other );

	public:
		uint64 GetValue( const std::string& value );
		uint64 GetUseValue( const std::string& value );
		KFAgentValue* FindAgentValue( const std::string& dataname );

		// 添加属性
		void AddValue( const std::string& dataname, std::string value );

		// 是否是符合条件的数据
		bool IsEligibilityData( KFData* kfdata, const std::string& exclude );
	public:
		// 父属性
		KFAgents * _kf_agents;

		// 配置字串
		std::string _string;

		// 数据名称
		std::string _data_name;

		// 操作 +-=* 等.....
		uint32 _operate;

		// 配置id
		uint32 _config_id;

		// 属性数据
		KFMap< std::string, const std::string&, KFAgentValue > _datas;
	};

	__ST_CLASS__( KFAgents )
	{
	public:
		KFAgents();
		~KFAgents();

		// non-copy 
	private:
		KFAgents( const KFAgents& other );
		KFAgents& operator=( const KFAgents& other );

	public:

		// 解析字符串奖励
		bool ParseFromString( const std::string& agentstring, const char* function, uint32 line );

		// 设置操作类型
		void SetOperate( uint32 operate );

		// 格式化字串
		const std::string& FormatRewardAgent( float multiple ) const;

	public:
		// 配置字串
		std::string _string;

		// 是否是随机奖励
		bool _is_rand_agent;

		// 是否有添加属性
		bool _is_add_value;

		// 奖励列表
		std::vector< KFAgent* > _agents;
	};

}


#endif
