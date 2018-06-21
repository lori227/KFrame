#ifndef __KF_LAUNCH_CONFIG_H__
#define __KF_LAUNCH_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFLaunchSetting
	{
	public:
		KFLaunchSetting()
		{
			_id = 0;
			_startup = true;
			_show_window = true;
			_pause = 0;
			_app_id = 1;
			_debug_shut_time = 0;
			_release_shut_time = 0;

			_stop = false;
			_process_id = 0;
		}

		uint32 GetShutTime() const;
		
	public:
		// id
		uint32 _id;

		// 是否开启
		bool _startup;

		// 是否后台运行
		bool _show_window;

		// 挂起
		uint32 _pause;

		// 程序路径
		std::string _app_path;

		// 程序名称
		std::string _app_name;

		// 程序id
		uint32 _app_id;

		// 配置目录
		std::string _logger_file;

		// 启动文件
		std::string _startup_config;

		// 关闭时间( 0 表示立即关闭
		uint32 _debug_shut_time;
		uint32 _release_shut_time;

		//////////////////////////////////////////////////
		// 停止
		bool _stop;

		// 启动的进程id
		uint32 _process_id;

		/////////////////////////////////////////////////////
		//linux执行文件路径
		std::string _linux_path;

		//linux进程重启指令
		std::string _linux_restar;
	};

	class KFLaunchConfig : public KFConfig, public KFSingleton< KFLaunchConfig >
	{
	public:
		KFLaunchConfig();
		~KFLaunchConfig();

		bool LoadConfig( const char* file );

		// 查找设定
		KFLaunchSetting* FindLaunchSetting( uint32 id );

	public:
		// 自动启动
		bool _auto_startup;

		// 启动文件
		std::string _startup_file;

		// launch redis in
		uint32 _launch_redis_id;

		// ftpid
		uint32 _ftp_id;

		// 称号列表
		KFMap< uint32, uint32, KFLaunchSetting > _kf_launch_setting;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_launch_config = KFLaunchConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif