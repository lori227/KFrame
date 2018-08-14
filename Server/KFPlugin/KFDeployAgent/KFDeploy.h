#ifndef __KF_DEPLOY_H__
#define __KF_DEPLOY_H__

#include "KFrame.h"

namespace KFrame
{
    class KFLaunchSetting
    {
    public:
        KFLaunchSetting();

        std::string GetAppPath();
        std::string GetStartupFile();

        void CopyFrom( MapString& values );
        void SaveTo( MapString& values );

    public:
        // 部署路径
        std::string _deploy_path;

        // 部署ftp
        uint32 _ftp_id;

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

        // 日志类型
        uint32 _log_type;

        // 服务类型
        std::string _service;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    class KFDeployData
    {
    public:
        KFDeployData();

        bool IsAppServer( const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid );
        void CopyFrom( MapString& values );
        void SaveTo( MapString& values );

    public:
        // 程序名称( 给web显示用 )
        std::string _app_name;

        // 程序类型( 给web显示用 );
        std::string _app_type;

        // 程序id
        std::string _app_id;

        // 小区id
        uint32 _zone_id;

        ///////////////////////////////////////////////////////////////////////
        // 运行时数据
        // 部署agentid
        std::string _agent_id;

        // agent ip
        std::string _local_ip;

        // 进程id
        uint32 _process_id;

        // 启动时间
        uint64 _startup_time;

        // 是否需要启动
        bool _is_startup;

        // 是否关闭
        bool _is_shutdown;

        // 是否正在下载
        bool _is_download;

        KFLaunchSetting* _kf_launch;
    };
}

#endif