#include "KFDeployService.h"
#include "KFDeployStartup.h"
#include "KFDeployConfig.h"
#include "KFDeployDump.h"


namespace KFrame
{
    void KFDeployService::Run()
    {
        KFGlobal::Instance()->_game_time = KFClock::GetTime();
        KFGlobal::Instance()->_real_time = KFDate::GetTimeEx();

        __try
        {
            KFPluginManage::Instance()->Run();
        }
        __except ( KFDeployDump::MyExceptionFilter( GetExceptionInformation() ) )
        {
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDeployService::InitService()
    {
        // 全区变量
        KFGlobal::Initialize( nullptr );

        // 对象池
        KFMalloc::Initialize( nullptr );

        // 日志
        KFLogger::Initialize( nullptr );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 设置时间
        auto global = KFGlobal::Instance();
        global->_game_time = KFClock::GetTime();
        global->_real_time = KFDate::GetTimeEx();

        global->_app_name = __STRING__( deploy );
        global->_app_type = __STRING__( tool );
        global->_app_id->FromString( "10.0.0" );

        KFDeployDump kfdump( global->_app_name.c_str(), global->_app_type.c_str(), "10.0.0" );

        // 初始化服务类型
        std::string strnetservice = "1.1";
        global->InitChannelService( strnetservice );

        // 初始化logger
        KFLogger::Instance()->InitLogger( "./setting/logger.xml" );

        // 内存日志
        KFMalloc::Instance()->SetLogOpen( false );

        // 读取启动配置
        std::string startupfile = "";
        if ( global->_app_type.empty() )
        {
            startupfile = __FORMAT__( "./startup/{}.startup", global->_app_name );
        }
        else
        {
            startupfile = __FORMAT__( "./startup/{}.{}.startup", global->_app_name, global->_app_type );
        }

        if ( !_kf_deploy_startup->InitStartup( startupfile ) )
        {
            return;
        }

        // 插件初始化
        KFPluginManage::Instance()->InitPlugin();
    }

    void KFDeployService::ShutDown()
    {
        _kf_deploy_startup->ShutDown();
    }
}
