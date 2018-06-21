#ifndef __KF_GROUP_CLIENT_CONFIG_H__
#define __KF_GROUP_CLIENT_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFGroupClientConfig : public KFConfig, public KFSingleton< KFGroupClientConfig >
	{
	public:
		KFGroupClientConfig();
		~KFGroupClientConfig();

		// 加载配置
		bool LoadConfig( const char* file );

	public:
		// 队伍最大个数
		uint32 _max_member_count;

		// 拒绝时间, 单位:秒
		uint32 _refuse_time;

		// 邀请时间
		uint32 _invite_valid_time;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_group_config = KFGroupClientConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif