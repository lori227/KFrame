#ifndef __KF_DEPLOY_AGENT_CONFIG_H__
#define __KF_DEPLOY_AGENT_CONFIG_H__

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
            _app_id = 0;
            _launch_id = 0;
            _is_startup = true;
            _is_pause = 0;
            _is_show_window = true;
            _ftp_id = 0;

            _process_id = 0;
            _startup_time = 0;
            _is_shutdown = false;
        }

        std::string GetStartupFile();

        bool IsAppServer( const std::string& appname, const std::string& apptype, uint32 appid );

    public:
        // id
        uint32 _launch_id;

        // 程序id
        uint32 _app_id;

        // 运行路径
        std::string _app_path;

        // 运行程序路径
        std::string _app_file;

        // 启动文件
        std::string _app_config;

        // 程序名称( 给web显示用 )
        std::string _app_name;

        // 程序类型( 给web显示用 );
        std::string _app_type;

        // 是否开启
        bool _is_startup;

        // 是否显示窗口
        bool _is_show_window;

        // 是否启动挂起
        uint32 _is_pause;

        // ftpid
        uint32 _ftp_id;
        ///////////////////////////////////////////////////////////////////////
        // 运行时数据

        // 进程id
        uint32 _process_id;

        // 启动时间
        uint64 _startup_time;

        // 是否已经关闭
        bool _is_shutdown;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////

    class KFDeployAgentConfig : public KFConfig, public KFSingleton< KFDeployAgentConfig >
    {
    public:
        KFDeployAgentConfig();
        ~KFDeployAgentConfig();

        bool LoadConfig( const char* file );

        // 查找设定
        KFLaunchSetting* FindLaunchSetting( uint32 launchid );

    public:
        // 部署路径
        std::string _deploy_path;

        // 自动启动
        bool _is_auto_startup;

        // ftp 地址
        uint32 _ftp_id;

        // 启动列表
        KFMap< uint32, uint32, KFLaunchSetting > _kf_launch_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_deploy_agent_config = KFDeployAgentConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif