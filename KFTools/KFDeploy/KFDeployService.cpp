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
    void KFDeployService::InitServices()
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
        auto kfglobal = KFGlobal::Instance();
        kfglobal->_game_time = KFClock::GetTime();
        kfglobal->_real_time = KFDate::GetTimeEx();

        kfglobal->_app_name = __KF_STRING__( deploy );
        kfglobal->_app_type = __KF_STRING__( tool );
        kfglobal->_app_id->FromString( "10.0.0" );

        KFDeployDump kfdump( kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(), "10.0.0" );

        // 初始化服务类型
        std::string strnetservice = "1.1";
        kfglobal->InitChannelService( strnetservice );

        // 初始化logger
        KFLogger::Instance()->InitLogger( 1, false );

        // 内存日志
        KFMalloc::Instance()->SetLogOpen( false );

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
