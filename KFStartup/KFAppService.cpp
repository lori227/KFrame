#include "KFAppService.h"
#include "KFStartup.h"
#include "KFAppConfig.h"
#include "KFApplication.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include "KFDump.h"
#endif

namespace KFrame
{
    void KFAppService::Run()
    {
        do
        {
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
                KFLogger::Instance()->LogStack( ex.what() );
            }
            catch ( ... )                                                                                                                            \
            {
                KFLogger::Instance()->LogStack( "run exception unknown" );
            }
#endif
            KFThread::Sleep( 1 );
        } while ( KFGlobal::Instance()->_app_run );

        ShutDown();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFAppService::InitService( KFApplication* application, StringMap& params )
    {
        _application = application;

        // 全局变量
        KFGlobal::Initialize( nullptr );

        // 对象池
        KFMalloc::Initialize( nullptr );

        // 日志
        KFLogger::Initialize( nullptr );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 设置时间
        auto global = KFGlobal::Instance();
        global->_startup_params = params;
        global->_game_time = KFClock::GetTime();
        global->_real_time = KFDate::GetTimeEx();

        auto str_app = params[ __STRING__( app ) ];
        global->_app_name = KFUtility::SplitString( str_app, "." );
        global->_app_type = KFUtility::SplitString( str_app, "." );
        global->_app_id->FromString( params[ __STRING__( id ) ] );

#if __KF_SYSTEM__ == __KF_WIN__
        KFDump kfdump( global->_app_name.c_str(), global->_app_type.c_str(), global->_app_id->ToString().c_str() );
#endif
        // 日志初始化
        auto log_file = params[ __STRING__( log ) ];
        if ( !KFLogger::Instance()->InitLogger( log_file ) )
        {
            return false;
        }

        // 打开内存日志
        auto log_memory = params[ __STRING__( logmemory ) ];
        KFMalloc::Instance()->SetLogOpen( log_memory == "1" );

        // 设置标题
        global->_title_text = KFUtility::FormatTitleText( global->_app_name, global->_app_type, global->_app_id->ToString() );
        _application->SetTitleText( global->_title_text.c_str() );

        // 网络类型
        global->InitNetType( params[ __STRING__( net ) ] );

        // 初始化服务类型
        global->InitChannelService( params[ __STRING__( service ) ] );

        // 版本号
        global->LoadVersion( "version" );

        // 读取启动配置
        std::string startup_file = __FORMAT__( "./startup/{}.{}.startup", global->_app_name, global->_app_type );
        if ( !_kf_startup->InitStartup( startup_file ) )
        {
            return false;
        }

        // 开启主逻辑线程
        KFThread::Create( this, &KFAppService::Run, __FUNC_LINE__ );
        __LOG_INFO__( "[{}:{}:{}] version[{}] startup ok", global->_app_name, global->_app_type, global->_app_id->ToString(), global->GetVersion() );
        return true;
    }

    void KFAppService::RunUpdate()
    {
        // 更新时间
        UpdateTime();

        // 内存分配日志
        KFMalloc::Instance()->Run();

        // 模块
        KFPluginManage::Instance()->Run();
    }

    void KFAppService::ShutDown()
    {
        _kf_startup->ShutDown();

        _application = nullptr;
    }

    bool KFAppService::IsShutDown()
    {
        return _application == nullptr;
    }

    void KFAppService::UpdateTime()
    {
        auto global = KFGlobal::Instance();

        // 现实时间( 单位:秒 ), 可能会出现对时回退的问题
        {
            auto realtime = KFDate::GetTimeEx();
            if ( realtime > global->_real_time )
            {
                global->_real_time = realtime;
            }
        }

        // 运行时间( 单位:毫秒 ),
        {
            auto last_loop_time = global->_game_time;
            auto game_time = KFClock::GetTime();
            if ( game_time > global->_game_time )
            {
                global->_game_time = game_time;
                global->_last_frame_use_time = global->_game_time - last_loop_time;
            }
            else
            {
                global->_last_frame_use_time = 0u;
            }
        }
    }
}
