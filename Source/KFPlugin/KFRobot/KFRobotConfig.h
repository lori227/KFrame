#ifndef __KF_ROBOT_CONFIG_H__
#define __KF_ROBOT_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{

	class KFRobotConfig : public KFConfig, public KFSingleton< KFRobotConfig >
	{
	public:
		KFRobotConfig();
		~KFRobotConfig();

		// 加载配置
		bool LoadConfig( const char* file );

	public:
		// 认证服务器IP地址
		std::string _auth_server_ip;

		// 机器人名字
		std::string _robot_name;

		// 机器人个数
		uint32 _robot_count;

		// 起始数量
		uint32 _start_count;

		// 登录时间间隔 单位:毫秒
		uint32 _login_span_time;

		// 认证channel
		uint32 _verify_channel;

		// server channel
		uint32 _server_channel;

		// base account id
		uint32 _account_id_base;

		// 设置好友开关
		uint32 _friend_switch;

		// 礼包cdkey
		std::string _cd_key;

		// 机器人状态切换
		std::string _oddStateList;

		// 机器人状态切换
		std::string _evenStateList;

		uint32 _state_rep_time;

		uint32 _next_state_cryl_time;

	};

	///////////////////////////////////////////////////////////////////////////
	static auto* _kf_robot_config = KFRobotConfig::Instance();
	///////////////////////////////////////////////////////////////////////////
}
#endif