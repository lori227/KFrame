#include "KFServices.h"
#include "KFStartup.h"
#include "KFAppConfig.h"
#include "KFApplication.h"
#include "KFThread/KFThread.h"
#include "KFMemory/KFLogMemory.h"

#if __KF_SYSTEM__ == __KF_WIN__
#include "KFDump.h"
#endif

namespace KFrame
{
	KFServices::KFServices()
	{
		_application = nullptr;
	}

	KFServices::~KFServices()
	{
		_application = nullptr;
	}

	void KFServices::Run()
	{
		do
		{
			KFGlobal::Instance()->_game_time = KFClock::GetTime();
			KFGlobal::Instance()->_real_time = KFDate::GetTimeEx();
#if __KF_SYSTEM__ == __KF_WIN__
			__try
			{
				RunUpdate();
			}
			__except ( KFDump::MyExceptionFilter( GetExceptionInformation() ) )
			{
			}
#else
			RunUpdate();
#endif
			KFThread::Sleep( 1 );
		} while ( KFGlobal::Instance()->_app_run );

		ShutDown();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool KFServices::InitService( KFApplication* application, const std::vector< std::string >& params )
	{
		_application = application;

		// 全区变量
		KFGlobal::Initialize( nullptr );

		// 对象池
		KFMalloc::Initialize( nullptr );

#ifdef __KF_RELEASE__
		KFMalloc::Instance()->SetLogMemoryOpen( true );
#endif
		// 设置时间
		auto kfglobal = KFGlobal::Instance();
		kfglobal->_game_time = KFClock::GetTime();
		kfglobal->_real_time = KFDate::GetTimeEx();
		kfglobal->_app_id = KFUtility::ToValue<uint32>( params[ 1 ] );

		// 读取配置
		if ( !_kf_startup->InitStartup( params[ 2 ].c_str() ) )
		{
			return false;
		}

		// 设置日志
		static std::string _init_log_file = "./setting/initapp.log4cxx";
		KFLogger::Initialize( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id, _init_log_file );
		if ( !_kf_startup->LoadPlugin() )
		{
			return false;
		}

		// 插件初始化
		_kf_plugin_manage->InitPlugin();

#if __KF_SYSTEM__ == __KF_WIN__
		KFDump kfdump( kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(), kfglobal->_app_id );
#endif	
		// 设置标题
		kfglobal->_title_text = kfglobal->FormatTitleText( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id );
		_application->SetTitleText( kfglobal->_title_text.c_str() );

		// 初始化内存日志定时器
		InitLogMemoryTimer();

		KFLogger::LogInit( KFLogger::Info, "[%s:%s:%u] startup ok!",
			kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(), kfglobal->_app_id );

		// log4cxx重新初始化
		static std::string _template_log_file = "./setting/templateapp.log4cxx";
		KFLogger::Initialize( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id, _template_log_file );

		KFThread::CreateThread( this, &KFServices::Run, __FUNCTION_LINE__ );
		return true;
	}

	void KFServices::RunUpdate()
	{
		// 打印内存信息
		PrintLogMemory();

		_kf_plugin_manage->Run();
	}

	void KFServices::ShutDown()
	{
		_kf_plugin_manage->ShutDown();
		_kf_startup->ShutDown();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void KFServices::InitLogMemoryTimer()
	{
		// 把log时间分来, 每日切换的时候, 有可能创建文件夹的时候被占用, 然后切换日志失败
		auto spacetime = KFGlobal::Instance()->_app_id % 10000 + KFRand::STRandDistrict( 100, 1000, 0 );
		_memory_timer.StartTimer( KFGlobal::Instance()->_game_time, 5 * TimeEnum::OneMinuteMicSecond + spacetime );
	}
	
	void KFServices::PrintLogMemory()
	{
		if ( !_memory_timer.DoneTimer( KFGlobal::Instance()->_game_time, true ) )
		{
			return;
		}

		KFMalloc::Instance()->PrintLogMemory();
	}
}
