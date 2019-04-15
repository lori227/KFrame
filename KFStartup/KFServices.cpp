#include "KFServices.h"
#include "KFStartup.h"
#include "KFAppConfig.h"
#include "KFApplication.h"

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
            try
            {
                RunUpdate();
            }
            catch ( std::exception& ex )
            {
                __LOG_ERROR__( "exception=[{}]!", ex.what() );
            }
            catch ( ... )                                                                                                                            \
            {
                __LOG_ERROR__( "exception unknown!" );
            }
#endif
            KFThread::Sleep( 1 );
        } while ( KFGlobal::Instance()->_app_run );

        ShutDown();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFServices::InitService( KFApplication* application, MapString& params )
    {
        _application = application;

        // 全区变量
        KFGlobal::Initialize( nullptr );

        // 对象池
        KFMalloc::Initialize( nullptr );

        // 日志
        KFLogger::Initialize( nullptr );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 设置时间
        auto kfglobal = KFGlobal::Instance();
        kfglobal->_startup_params = params;
        kfglobal->_game_time = KFClock::GetTime();
        kfglobal->_real_time = KFDate::GetTimeEx();

        auto strapp = params[ __KF_STRING__( app ) ];
        kfglobal->_app_name = KFUtility::SplitString( strapp, "." );
        kfglobal->_app_type = KFUtility::SplitString( strapp, "." );
        kfglobal->_app_id->FromString( params[ __KF_STRING__( id ) ] );

#if __KF_SYSTEM__ == __KF_WIN__
        KFDump kfdump( kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(), kfglobal->_app_id->ToString().c_str() );
#endif

        // 初始化服务类型
        kfglobal->InitNetService( params[ __KF_STRING__( service ) ] );

        // 版本号
        kfglobal->LoadVersion( "version" );

        // 初始化logger
        auto strlog = params[ __KF_STRING__( log ) ];
        auto loglevel = KFUtility::SplitValue< uint32 >( strlog, "." );
        KFLogger::Instance()->InitLogger( loglevel, true );

        // 内存日志
        auto memorylog = KFUtility::SplitValue< uint32 >( strlog, "." );
        KFMalloc::Instance()->SetLogOpen( memorylog == 1 );

        // 读取启动配置
        std::string startupfile = "";
        if ( kfglobal->_app_type.empty() )
        {
            startupfile = __FORMAT__( "./startup/{}.startup", kfglobal->_app_name );
        }
        else
        {
            startupfile = __FORMAT__( "./startup/{}.{}.startup", kfglobal->_app_name, kfglobal->_app_type );
        }

        if ( !_kf_startup->InitStartup( startupfile ) )
        {
            return false;
        }

        // 插件初始化
        KFPluginManage::Instance()->InitPlugin();

        // 设置标题
        kfglobal->_title_text = KFUtility::FormatTitleText( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );
        _application->SetTitleText( kfglobal->_title_text.c_str() );

        // 初始化内存日志定时器
        InitLogMemoryTimer();

        __LOG_INFO__( "[{}:{}:{}] version[{}] startup ok!", kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString(), kfglobal->GetVersion() );

        // 开启主逻辑线程
        KFThread::CreateThread( this, &KFServices::Run, __FUNC_LINE__ );
        return true;
    }

    void KFServices::RunUpdate()
    {
        KFPluginManage::Instance()->Run();

        // 打印内存信息
        PrintLogMemory();
    }

    void KFServices::ShutDown()
    {
        _kf_startup->ShutDown();

        _application = nullptr;
    }

    bool KFServices::IsShutDown()
    {
        return _application == nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFServices::InitLogMemoryTimer()
    {
        auto kfglobal = KFGlobal::Instance();

        // 把log时间分来, 每日切换的时候, 有可能创建文件夹的时候被占用, 然后切换日志失败
        auto spacetime = kfglobal->_app_id->GetId() % 10000 + kfglobal->RandRange( 100, 1000, 0 );
        _memory_timer.StartTimer( kfglobal->_game_time, 5 * KFTimeEnum::OneMinuteMicSecond + spacetime );
    }

    void KFServices::PrintLogMemory()
    {
        if ( !_memory_timer.DoneTimer( KFGlobal::Instance()->_game_time, true ) )
        {
            return;
        }

        KFMalloc::Instance()->PrintMemoryLog();
    }
}
