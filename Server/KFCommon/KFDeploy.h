#ifndef __KF_DEPLOY_H__
#define __KF_DEPLOY_H__

#include "KFrame.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    class KFAgentData
    {
    public:
        KFAgentData()
        {
            _agent_id = 0;
            _port = 0;
            status = 0;
        }

    public:
        // 服务器id
        uint32 _agent_id;

        // 局域网地址
        std::string _local_ip;

        // 名字
        std::string _name;

        // 类型
        std::string _type;

        // 端口
        uint32 _port;

        // 状态
        uint32 status;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    class KFDeployData
    {
    public:
        KFDeployData()
        {
            _ftp_id = 0;
            _app_id = 0;
            _process_id = 0;
            _startup_time = 0;
            _zone_id = 0;
            _agent_id = 0;
            _is_startup = false;
            _is_download = false;
        }

        bool IsAppServer( const std::string& appname, const std::string& apptype, uint32 appid, uint32 zoneid )
        {
            if ( appname == _globbing_str )
            {
                return true;
            }

            if ( appname != _app_name )
            {
                return false;
            }

            if ( zoneid != _zone_id )
            {
                return false;
            }

            if ( apptype == _globbing_str )
            {
                return true;
            }

            if ( apptype != _app_type )
            {
                return false;
            }

            if ( appid == _invalid_int )
            {
                return true;
            }

            if ( appid != _app_id )
            {
                return false;
            }

            return true;
        }

        void CopyFrom( MapString& values )
        {
            _app_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( appid ) ] );
            _app_name = values[ __KF_STRING__( appname ) ];
            _app_type = values[ __KF_STRING__( apptype ) ];
            _zone_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( zoneid ) ] );
            _is_startup = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( startup ) ] );
            _process_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( process ) ] );
            _startup_time = KFUtility::ToValue< uint64 >( values[ __KF_STRING__( time ) ] );
            _agent_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( agentid ) ] );
            _local_ip = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( localip ) ] );
        }

        void SaveTo( MapString& values )
        {
            values[ __KF_STRING__( appid ) ] = __TO_STRING__( _app_id );
            values[ __KF_STRING__( appname ) ] = _app_name;
            values[ __KF_STRING__( apptype ) ] = _app_type;
            values[ __KF_STRING__( zoneid ) ] = __TO_STRING__( _zone_id );
            values[ __KF_STRING__( startup ) ] = __TO_STRING__( _is_startup ? 1 : 0 );
            values[ __KF_STRING__( process ) ] = __TO_STRING__( _process_id );
            values[ __KF_STRING__( time ) ] = __TO_STRING__( _startup_time );
            values[ __KF_STRING__( agentid ) ] = __TO_STRING__( _agent_id );
            values[ __KF_STRING__( localip ) ] = _local_ip;
        }

    public:
        // 程序名称( 给web显示用 )
        std::string _app_name;

        // 程序类型( 给web显示用 );
        std::string _app_type;

        // 程序id
        uint32 _app_id;

        // 小区id
        uint32 _zone_id;

        ///////////////////////////////////////////////////////////////////////
        // 运行时数据
        // 部署agentid
        uint32 _agent_id;

        // agent ip
        std::string _local_ip;

        // 进程id
        uint32 _process_id;

        // 启动时间
        uint64 _startup_time;

        // 是否需要启动
        bool _is_startup;

        // 是否正在下载
        bool _is_download;
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    class KFLaunchSetting
    {
    public:
        KFLaunchSetting()
        {
            _min_id = 0;
            _max_id = 0;
        }

        std::string GetStartupFile()
        {
            auto file = _app_file;

#ifdef __KF_DEBUG__
            file += "d";
#endif // __KF_DEBUG__

#if __KF_SYSTEM__ == __KF_WIN__
            file += ".exe";
#endif
            return file;
        }


        void CopyFrom( MapString& values )
        {
            _deploy_path = values[ __KF_STRING__( deploypath ) ];
            _ftp_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( ftpid ) ] );
            _app_name = values[ __KF_STRING__( appname ) ];
            _app_type = values[ __KF_STRING__( apptype ) ];
            _app_path = values[ __KF_STRING__( apppath ) ];
            _app_file = values[ __KF_STRING__( appfile ) ];
            _app_config = values[ __KF_STRING__( appconfig ) ];
            _min_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( minid ) ] );
            _max_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( maxid ) ] );
            _log_type = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( logtype ) ] );
            _service = values[ __KF_STRING__( service ) ];
        }

        void SaveTo( MapString& values )
        {
            values[ __KF_STRING__( deploypath ) ] = _deploy_path;
            values[ __KF_STRING__( ftpid ) ] = __TO_STRING__( _ftp_id );
            values[ __KF_STRING__( appname ) ] = _app_name;
            values[ __KF_STRING__( apptype ) ] = _app_type;
            values[ __KF_STRING__( apppath ) ] = _app_path;
            values[ __KF_STRING__( appfile ) ] = _app_file;
            values[ __KF_STRING__( appconfig ) ] = _app_config;
            values[ __KF_STRING__( minid ) ] = __TO_STRING__( _min_id );
            values[ __KF_STRING__( maxid ) ] = __TO_STRING__( _max_id );
            values[ __KF_STRING__( logtype ) ] = __TO_STRING__( _log_type );
            values[ __KF_STRING__( service ) ] = _service;
        }

    public:
        // 部署路径
        std::string _deploy_path;

        // 部署ftp
        uint32 _ftp_id;

        // 最小id
        uint32 _min_id;

        // 最大id
        uint32 _max_id;

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
}

#endif