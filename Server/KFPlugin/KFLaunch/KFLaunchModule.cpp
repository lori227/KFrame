#include "KFLaunchModule.h"
#include "KFJson.h"
#include "KFLaunchConfig.h"
#include "KFFtp/KFFtpInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFCommand/KFCommandInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

#if __KF_SYSTEM__ == __KF_WIN__
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace KFrame
{
	KFLaunchModule::KFLaunchModule()
	{
	}

	KFLaunchModule::~KFLaunchModule()
	{

	}

	void KFLaunchModule::InitModule()
	{
		// 启动定时器
		_launch_timer.StartTimer( 1, 5000 );

		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		_kf_config->AddConfig( _kf_launch_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
	}

	void KFLaunchModule::BeforeRun()
	{
		__REGISTER_RUN_FUNCTION__( &KFLaunchModule::Run );

		BindLaunch();

		// 更新launch到数据库
		UpdateLaunchToDatabase();
		/////////////////////////////////////////////////////////////////////
		__REGISTER_HTTP_FUNCTION__( KFField::_shut_down, false, &KFLaunchModule::HandleShutDown );
		__REGISTER_HTTP_FUNCTION__( KFField::_startup, false, &KFLaunchModule::HandleStartup );
		__REGISTER_HTTP_FUNCTION__( KFField::_download, false, &KFLaunchModule::HandleFtpDownload );
		__REGISTER_HTTP_FUNCTION__( KFField::_transmit, false, &KFLaunchModule::HandleTransmitCommand );
	}

	void KFLaunchModule::BeforeShut()
	{
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );

		__UNREGISTER_HTTP_FUNCTION__( KFField::_shut_down );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_startup );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_download );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_transmit );

		////////////////////////////////////////////////////////////////////////////////////
		_kf_schedule->UnRegisterSchedule( this );
	}

	void KFLaunchModule::ShutDown()
	{
		__UNREGISTER_RUN_FUNCTION__();

		for ( auto& iter : _kf_launch_config->_kf_launch_setting._objects )
		{
			auto kfsetting = iter.second;
			if ( kfsetting->_process_id == 0 )
			{
				continue;
			}

			KillProcess( kfsetting->_process_id );
			kfsetting->_process_id = 0;
		}
	}

	void KFLaunchModule::Run()
	{
		RunLaunch();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////	
#define __LAUNCH_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_login );
	
	void KFLaunchModule::RunLaunch()
	{
		if ( !_launch_timer.DoneTimer( KFGlobal::Instance()->_game_time, true ) )
		{
			return;
		}

		// 不是自动启动
		if ( !_kf_launch_config->_auto_startup )
		{
			return;
		}

		for ( auto& iter : _kf_launch_config->_kf_launch_setting._objects )
		{
			auto kfsetting = iter.second;
			if ( !kfsetting->_startup )
			{
				continue;
			}

			// 检查并启动
#if __KF_SYSTEM__ == __KF_WIN__
			auto startup = StartWinProcess( kfsetting );
#else
			auto startup = StartLinuxProcess( kfsetting );
#endif
			if ( startup )
			{
				UpdateServiceStatus( kfsetting );
			}
		}
	}

	void KFLaunchModule::BindLaunch()
	{
#if __KF_SYSTEM__ == __KF_WIN__
		// 关闭原来的launch
		KillWinProcess( KFGlobal::Instance()->_title_text );

		// 绑定
		BindWinLaunch();
#else
		KillLinuxProcess( KFGlobal::Instance()->_title_text );

		BindLinuxLaunch();
#endif
	}

	void KFLaunchModule::KillProcess( const std::string& titletext )
	{
#if __KF_SYSTEM__ == __KF_WIN__
		KillWinProcess( titletext );
#else
		KillLinuxProcess( titletext );
#endif
	}

	void KFLaunchModule::KillProcess( uint32 processid )
	{
#if __KF_SYSTEM__ == __KF_WIN__
		KillWinProcess( processid );
#else
		KillLinuxProcess( processid );
#endif
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

#if __KF_SYSTEM__ == __KF_WIN__
	void KFLaunchModule::KillWinProcess( const std::string& titletext )
	{
		auto findhwnd = ::FindWindow( NULL, titletext.c_str() );
		if ( findhwnd == nullptr )
		{
			return;
		}

		DWORD processid;
		GetWindowThreadProcessId( findhwnd, &processid );
		KillWinProcess( processid );
	}

	void KFLaunchModule::KillWinProcess( uint32 processid )
	{
		DWORD flag = PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION;
		HANDLE handle = OpenProcess( flag, FALSE, processid );
		if ( handle == nullptr )
		{
			return;
		}

		TerminateProcess( handle, 0 );
	}

	void KFLaunchModule::BindWinLaunch()
	{
		for ( auto& iter : _kf_launch_config->_kf_launch_setting._objects )
		{
			auto kfsetting = iter.second;

			auto titletext = KFGlobal::Instance()->FormatTitleText( kfsetting->_app_name, "", kfsetting->_app_id );
			auto findhwnd = ::FindWindow( NULL, titletext.c_str() );
			if ( findhwnd == nullptr )
			{
				continue;
			}

			DWORD processid;
			GetWindowThreadProcessId( findhwnd, &processid );
			if ( kfsetting->_startup )
			{
				kfsetting->_process_id = processid;
			}
			else
			{
				KillWinProcess( processid );
			}
		}
	}

	void KFLaunchModule::CheckWinProcess( KFLaunchSetting* kfsetting )
	{
		if ( kfsetting->_process_id == 0 )
		{
			return;
		}

		DWORD flag = PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION;
		HANDLE handle = OpenProcess( flag, FALSE, kfsetting->_process_id );
		if ( handle != nullptr )
		{
			DWORD code = STILL_ACTIVE;
			GetExitCodeProcess( handle, &code );
			if ( code == STILL_ACTIVE )
			{
				return;
			}
			TerminateProcess( handle, 0 );
		}

		kfsetting->_process_id = 0;
		UpdateServiceStatus( kfsetting );
	}

	bool KFLaunchModule::StartWinProcess( KFLaunchSetting* kfsetting )
	{
		CheckWinProcess( kfsetting );
		if ( kfsetting->_stop || kfsetting->_process_id != 0 )
		{
			return false;
		}

		// 启动进程
		STARTUPINFO startupinfo;
		memset( &startupinfo, 0, sizeof( STARTUPINFO ) );
		startupinfo.cb = sizeof( STARTUPINFO );
		startupinfo.wShowWindow = SW_SHOW;
		startupinfo.dwFlags = STARTF_USESHOWWINDOW;

		std::string param = KFUtility::Format( " %u %s %u %s %s", kfsetting->_pause, 
			kfsetting->_app_name.c_str(), kfsetting->_app_id, 
			kfsetting->_logger_file.c_str(), kfsetting->_startup_config.c_str() );

		//CREATE_NEW_CONSOLE
		uint32 createflag = CREATE_NEW_CONSOLE;
		if ( !kfsetting->_show_window )
		{
			createflag = CREATE_NO_WINDOW;
		}

		auto startupfile = kfsetting->_app_path + _kf_launch_config->_startup_file;

		PROCESS_INFORMATION processinfo;
		BOOL result = CreateProcess( startupfile.c_str(), const_cast<char*>(param.c_str()),
			NULL, NULL, FALSE, createflag, NULL, kfsetting->_app_path.c_str(), &startupinfo, &processinfo );

		kfsetting->_process_id = processinfo.dwProcessId;
		if ( kfsetting->_process_id != 0 )
		{
			KFLogger::LogLogic( KFLogger::Info, "Startup [ %s:%d ] Ok! ProcessId=%u", kfsetting->_app_name.c_str(), kfsetting->_app_id, kfsetting->_process_id );
		}
		else
		{
			KFLogger::LogLogic( KFLogger::Error, "Startup [ %s:%d ] Failed!", kfsetting->_app_name.c_str(), kfsetting->_app_id );
		}

		return true;
	}
#else
	void KFLaunchModule::KillLinuxProcess( const std::string & titletext )
	{
		std::string killstr = "killall -9 " + titletext;
		FILE *fp = popen( killstr.c_str(), "r" );
		if ( fp == nullptr )
		{
			KFLogger::LogLogic( KFLogger::Error, "KillLinuxProcess [ %s ] Failed!", titletext.c_str() );
			return;
		}
		pclose( fp );
	}

	void KFLaunchModule::KillLinuxProcess( uint32 processid )
	{
		auto strvalue = KFUtility::ToString( processid );
		std::string killstr = "kill -s 9  " + strvalue;
		FILE *fp = popen( killstr.c_str(), "r" );
		if ( fp == nullptr )
		{
			KFLogger::LogLogic( KFLogger::Error, "KillLinuxProcess [ %s ] Failed!", strvalue.c_str() );
			return;
		}
		pclose( fp );
	}

	void KFLaunchModule::BindLinuxLaunch()
	{
		for ( auto& iter : _kf_launch_config->_kf_launch_setting._objects )
		{
			auto kfsetting = iter.second;

			char titletext[ 128 ] = { 0 };
			sprintf( titletext, "%s %s %u", kfsetting->_app_name.c_str(), "0", kfsetting->_app_id );

			//获取程序的pid
			uint32 pid = 0;
			GetPidByName( pid, titletext );
			if ( pid == 0 )
				continue;

			if ( kfsetting->_startup )
			{
				kfsetting->_process_id = pid;
			}
			else
			{
				KillLinuxProcess( pid );
			}
		}
	}


	void KFLaunchModule::CheckLinuxProcess( KFLaunchSetting * kfsetting )
	{
		if ( kfsetting->_process_id == 0 )
		{
			return;
		}
		
		char shellstr[ 128 ] = { 0 };
		sprintf( shellstr, "ps -e | grep -c %d", kfsetting->_process_id );
		FILE *fp = popen( shellstr, "r" );
		if ( fp != nullptr )
		{
			char numbuff[ 32 ] = { 0 };
			if ( fgets( numbuff, sizeof( numbuff ), fp ) != numbuff )
			{
				auto num = atoi( numbuff );
				if ( num == 1 )
				{
					pclose( fp );
					return;
				}
			}
			pclose( fp );
		}

		KillLinuxProcess( kfsetting->_process_id );
		kfsetting->_process_id = 0;
		UpdateServiceStatus( kfsetting );
	}

	bool KFLaunchModule::StartLinuxProcess( KFLaunchSetting * kfsetting )
	{
		CheckLinuxProcess(kfsetting);

		if ( kfsetting->_stop || kfsetting->_process_id != 0 )
		{
			return false;
		}

		FILE *fp = popen( kfsetting->_linux_path.c_str(), "r" );
		if ( fp == nullptr )
		{
			return false;
		}
		fprintf( fp, kfsetting->_linux_restar.c_str() );
		pclose( fp );

		return true;
	}

	void KFLaunchModule::GetPidByName( uint32 & pid, const char * titletext )
	{
		// 打开管道,执行shell命令
		char shellstr[ 256 ] = { 0 };
		sprintf( shellstr, "ps -ef|grep '%s'|grep -v 'grep'|awk '{print $2}'", titletext );
		FILE *fp = popen( shellstr, "r" );
		if ( fp != nullptr )
		{
			// 逐行读取执行结果
			char buff[ 32 ] = { 0 };
			if ( fgets( buff, sizeof( buff ), fp ) != NULL )
			{
				pid = atoi( buff );
			}

			// 关闭管道指针
			pclose( fp );
		}
	}

#endif

	void KFLaunchModule::UpdateLaunchToDatabase()
	{
		auto redisdriver = __LAUNCH_REDIS_DRIVER__;

		// 获得完整路径
		auto httpurl = _kf_http_server->GetHttpUrl();

		auto kfglobal = KFGlobal::Instance();
		redisdriver->AppendCommand( "sadd %s %u", KFField::_launch_list.c_str(), kfglobal->_app_id );
		redisdriver->AppendCommand( "hmset %s:%u %s %s %s %u %s %s",
			KFField::_launch.c_str(), kfglobal->_app_id, KFField::_name.c_str(), kfglobal->_app_name.c_str(), KFField::_id.c_str(), kfglobal->_app_id,
			KFField::_url.c_str(), httpurl.c_str() );

		redisdriver->PipelineExecute();
	}

	void KFLaunchModule::UpdateServiceStatus()
	{
		for ( auto& iter : _kf_launch_config->_kf_launch_setting._objects )
		{
			auto kfsetting = iter.second;
			UpdateServiceStatus( kfsetting );
		}
	}

	void KFLaunchModule::UpdateServiceStatus( KFLaunchSetting* kfsetting )
	{
		auto redisdriver = __LAUNCH_REDIS_DRIVER__;


		auto kfglobal = KFGlobal::Instance();
		redisdriver->AppendCommand( "sadd %s %u", KFField::_launch_service.c_str(), kfglobal->_app_id );
		redisdriver->AppendCommand( "hmset %s:%u %s %s %s %u %s %u %s %u",
			KFField::_launch_service.c_str(), kfglobal->_app_id, KFField::_name.c_str(), kfsetting->_app_name.c_str(), KFField::_id.c_str(), kfsetting->_app_id,
			KFField::_startup.c_str(), kfsetting->_startup, KFField::_process.c_str(), kfsetting->_process_id );
		redisdriver->PipelineExecute();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string KFLaunchModule::HandleStartup( const std::string& ip, const std::string& data )
	{
		KFJson request( data );
		auto time = request.GetUInt32( KFField::_time );

		if ( time <= KFGlobal::Instance()->_real_time )
		{
			ScheduleStartup( 0, data.c_str(), static_cast<uint32>(data.size()) );
		}
		else
		{
			auto kfsetting = _kf_schedule->CreateScheduleSetting();
			kfsetting->SetTime( time );
			kfsetting->SetData( __LINE__, reinterpret_cast<const void*>(data.data()), static_cast<uint32>(data.size()) );
			_kf_schedule->RegisterSchedule( kfsetting, this, &KFLaunchModule::ScheduleStartup );
		}

		return _kf_http_server->SendResponseCode( KFErrorEnum::Success );
	}

	void KFLaunchModule::ScheduleStartup( uint32 id, const char* data, uint32 size )
	{
		KFJson request( data, size );

		// 关闭指定进程
		auto processlist = request[ KFField::_process ];
		for ( uint32 i = 0; i < processlist.size(); ++i )
		{
			auto id = processlist[ i ].asUInt();
			auto kfsetting = _kf_launch_config->FindLaunchSetting( id );
			if ( kfsetting == nullptr )
			{
				continue;
			}

			kfsetting->_stop = false;
		}
	}

	std::string KFLaunchModule::HandleShutDown( const std::string& ip, const std::string& data )
	{
		KFJson request( data );
		auto time = request.GetUInt32( KFField::_time );

		if ( time <= KFGlobal::Instance()->_real_time )
		{
			ScheduleShutDown( 0, data.c_str(), static_cast<uint32>(data.size()) );
		}
		else
		{
			auto kfsetting = _kf_schedule->CreateScheduleSetting();
			kfsetting->SetTime( time );
			kfsetting->SetData( __LINE__, reinterpret_cast<const void*>(data.data()), static_cast<uint32>(data.size()) );
			_kf_schedule->RegisterSchedule( kfsetting, this, &KFLaunchModule::ScheduleShutDown );
		}

		return _kf_http_server->SendResponseCode( KFErrorEnum::Success );
	}

	void KFLaunchModule::ScheduleShutDown( uint32 id, const char* data, uint32 size )
	{
		KFJson request( data, size );

		auto processlist = request[ KFField::_process ];
		for ( uint32 i = 0; i < processlist.size(); ++i )
		{
			auto id = processlist[i].asUInt();
			auto kfsetting = _kf_launch_config->FindLaunchSetting( id );
			if ( kfsetting == nullptr )
			{
				continue;
			}

			// 设置停止
			kfsetting->_stop = true;

			uint32 shuttime = kfsetting->GetShutTime();
			if ( shuttime == 0 )
			{
				shuttime = 1;
			}

			// 添加命令
			KFJson json;
			json.SetValue< const std::string& >( KFField::_command, KFField::_shut_down );
			json.SetValue( KFField::_time, shuttime );

			auto command = json.Serialize();
			_kf_command->AddCommand( kfsetting->_app_name.c_str(), kfsetting->_app_id, command.c_str() );
			KFLogger::LogLogic( KFLogger::Info, "[ %s : %u ] ShutDown Command!", kfsetting->_app_name.c_str(), kfsetting->_app_id );
		}
	}

	std::string KFLaunchModule::HandleFtpDownload( const std::string& ip, const std::string& data )
	{
		KFJson request( data );
		auto time = request.GetUInt32( KFField::_time );

		if ( time == 0 )
		{
			ScheduleFtpDownload( 0, data.c_str(), static_cast<uint32>(data.size()) );
		}
		else
		{
			auto kfsetting = _kf_schedule->CreateScheduleSetting();
			kfsetting->SetTime( time );
			kfsetting->SetData( __LINE__, reinterpret_cast<const void*>(data.data()), static_cast<uint32>(data.size()) );
			_kf_schedule->RegisterSchedule( kfsetting, this, &KFLaunchModule::ScheduleFtpDownload );
		}

		return _kf_http_server->SendResponseCode( KFErrorEnum::Success );
	}

	void KFLaunchModule::ScheduleFtpDownload( uint32 id, const char* data, uint32 size )
	{
		KFJson request( data, size );

		// 开启ftp下载
		_kf_ftp->StartDownload( _kf_launch_config->_ftp_id, this, &KFLaunchModule::FtpDownLoadFinish );
		KFLogger::LogLogic( KFLogger::Info, "Start Ftp DownLoad!" );
	}

	void KFLaunchModule::FtpDownLoadFinish( uint32 result )
	{
		KFLogger::LogLogic( KFLogger::Info, "Ftp DownLoad Finish! Result=[%s]", KFUtility::ToString( result ).c_str() );
	}

	std::string KFLaunchModule::HandleTransmitCommand( const std::string& ip, const std::string& data )
	{
		KFJson request( data );
		auto time = request.GetUInt32( KFField::_time );

		if ( time == 0 )
		{
			ScheduleTransmitCommand( 0, data.c_str(), static_cast<uint32>(data.size()) );
		}
		else
		{
			auto kfsetting = _kf_schedule->CreateScheduleSetting();
			kfsetting->SetTime( time );
			kfsetting->SetData( __LINE__, reinterpret_cast<const void*>(data.data()), static_cast<uint32>(data.size()) );
			_kf_schedule->RegisterSchedule( kfsetting, this, &KFLaunchModule::ScheduleTransmitCommand );
		}

		return _kf_http_server->SendResponseCode( KFErrorEnum::Success );
	}

	void KFLaunchModule::ScheduleTransmitCommand( uint32 id, const char* data, uint32 size )
	{
		std::string command;
		command.assign( data, size );
		KFJson request( command );

		auto processlist = request[ KFField::_process ];
		for ( uint32 i = 0; i < processlist.size(); ++i )
		{
			auto id = processlist[i].asUInt();
			auto kfsetting = _kf_launch_config->FindLaunchSetting( id );
			if ( kfsetting == nullptr )
			{
				continue;
			}

			// 添加命令
			_kf_command->AddCommand( kfsetting->_app_name.c_str(), kfsetting->_app_id, command.c_str() );
		}

		KFLogger::LogLogic( KFLogger::Info, "Transmit Command [%s] !", command.c_str() );
	}
}
