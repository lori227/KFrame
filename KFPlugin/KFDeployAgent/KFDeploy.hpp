#ifndef __KF_DEPLOY_H__
#define __KF_DEPLOY_H__

#include "KFrame.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    class KFDeployData
    {
    public:
        KFDeployData();


        std::string GetAppPath();
        std::string GetStartupFile( bool is_debug );

        bool IsAppServer( const std::string& app_name, const std::string& app_type, const std::string& app_id, uint32 zone_id );
        void CopyFrom( StringMap& values );
        void SaveTo( StringMap& values );

    public:
        // 部署路径
        std::string _deploy_path;

        // 运行路径
        std::string _app_path;

        // 运行程序路径
        std::string _app_file;

        // 程序名称( 给web显示用 )
        std::string _app_name;

        // 程序类型( 给web显示用 );
        std::string _app_type;

        // 程序id
        std::string _app_id;

        // 小区id
        uint32 _zone_id;

        // 网络类型
        std::string _net_type;

        // 服务类型
        std::string _service_type;

        // 启动参数
        std::string _params;

        // 心跳超时时间
        uint32 _heartbeat = 0;
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

        // 是否debug模式
        bool _is_debug;

        // 是否需要启动
        bool _is_startup;

        // 是否关闭
        bool _is_shutdown;

        // 是否连接
        bool _is_connected = false;

        // 超时时间
        uint64 _heartbeat_timeout = 0u;
    };
}

#endif