#include "KFServices.h"
#include "KFStartup.h"
#include "KFAppConfig.h"
#include "KFApplication.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include "KFDump.h"
#endif

namespace KFrame
{
    void KFServices::Run()
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

    void KFServices::UpdateTime()
    {
        auto kfglobal = KFGlobal::Instance();

        // 现实时间( 单位:秒 ), 可能会出现对时回退的问题
        {
            auto realtime = KFDate::GetTimeEx();
            if ( realtime > kfglobal->_real_time )
            {
                kfglobal->_real_time = realtime;
            }
        }

        // 运行时间( 单位:毫秒 ),
        {
            auto lastlooptime = kfglobal->_game_time;
            auto gametime = KFClock::GetTime();
            if ( gametime > kfglobal->_game_time )
            {
                kfglobal->_game_time = gametime;
                kfglobal->_last_frame_use_time = kfglobal->_game_time - lastlooptime;
            }
            else
            {
                kfglobal->_last_frame_use_time = 0u;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFServices::InitService( KFApplication* application, StringMap& params )
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
        auto kfglobal = KFGlobal::Instance();
        kfglobal->_startup_params = params;
        kfglobal->_game_time = KFClock::GetTime();
        kfglobal->_real_time = KFDate::GetTimeEx();

        auto strapp = params[ __STRING__( app ) ];
        kfglobal->_app_name = KFUtility::SplitString( strapp, "." );
        kfglobal->_app_type = KFUtility::SplitString( strapp, "." );
        kfglobal->_app_id->FromString( params[ __STRING__( id ) ] );

#if __KF_SYSTEM__ == __KF_WIN__
        KFDump kfdump( kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(), kfglobal->_app_id->ToString().c_str() );
#endif
        // 日志初始化
        auto logfile = params[ __STRING__( log ) ];
        if ( !KFLogger::Instance()->InitLogger( logfile ) )
        {
            return false;
        }

        // 打开内存日志
        auto logmemory = params[ __STRING__( logmemory ) ];
        KFMalloc::Instance()->SetLogOpen( logmemory == "1" );

        // 设置标题
        kfglobal->_title_text = KFUtility::FormatTitleText( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );
        _application->SetTitleText( kfglobal->_title_text.c_str() );

        // 网络类型
        kfglobal->InitNetType( params[ __STRING__( net ) ] );

        // 初始化服务类型
        kfglobal->InitChannelService( params[ __STRING__( service ) ] );

        // 版本号
        kfglobal->LoadVersion( "version" );

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

        // 加载插件, 并初始化
        if ( !_kf_startup->InitStartup( startupfile ) )
        {
            return false;
        }

        // 开启主逻辑线程
        KFThread::CreateThread( this, &KFServices::Run, __FUNC_LINE__ );
        __LOG_INFO__( "[{}:{}:{}] version[{}] startup ok", kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString(), kfglobal->GetVersion() );
        return true;
    }

    void KFServices::RunUpdate()
    {
        // 更新时间
        UpdateTime();

        // 内存分配日志
        KFMalloc::Instance()->Run();

        // 模块
        KFPluginManage::Instance()->Run();
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
}
