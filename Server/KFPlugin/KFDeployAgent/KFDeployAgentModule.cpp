#include "KFDeployAgentModule.h"
#include "KFDeployAgentConfig.h"
#include "KFProtocol/KFProtocol.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <windows.h>
    #include <direct.h>
    #define __MKDIR__( path ) _mkdir( path.c_str() )
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/wait.h>

    #define __MKDIR__( path ) mkdir( path.c_str(), 0755 )
#endif

namespace KFrame
{
    KFDeployAgentModule::KFDeployAgentModule()
    {
        _deploy_server_id = 0;
        _kf_task = nullptr;
    }

    KFDeployAgentModule::~KFDeployAgentModule()
    {

    }

    void KFDeployAgentModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_deploy_agent_config, true );
    }

    void KFDeployAgentModule::BeforeRun()
    {
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFDeployAgentModule::OnClientConnectServer );
        __REGISTER_LOOP_TIMER__( 1, 10000, &KFDeployAgentModule::OnTimerStartupProcess );
        __REGISTER_LOOP_TIMER__( 2, 1000, &KFDeployAgentModule::OnTimerCheckTaskFinish );
        ////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &KFDeployAgentModule::HandleDeployCommandReq );
    }

    static std::string _pid_path = "./pid";

    void KFDeployAgentModule::OnceRun()
    {
        __MKDIR__( _pid_path );
        BindServerProcess();
    }

    void KFDeployAgentModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_TIMER__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        //////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ );
    }

    /////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFDeployAgentModule::OnClientConnectServer )
    {
        // 连接成功
        if ( servername == __KF_STRING__( deploy ) && servertype == __KF_STRING__( server ) )
        {
            _deploy_server_id = serverid;

            auto kfglobal = KFGlobal::Instance();

            // 把自己注册到Services
            KFMsg::S2SRegisterAgentToServerReq req;
            req.set_agentid( kfglobal->_app_id );
            req.set_name( kfglobal->_app_name );
            req.set_type( kfglobal->_app_type );
            req.set_port( kfglobal->_listen_port );
            req.set_localip( _kf_ip_address->GetLocalIp() );
            _kf_tcp_client->SendNetMessage( serverid, KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &req );

            // 更新服务器状态
            for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
            {
                auto kfsetting = iter.second;
                UpdateProcessToServer( kfsetting );
            }
        }
    }

    __KF_TIMER_FUNCTION__( KFDeployAgentModule::OnTimerStartupProcess )
    {
        // 不是自动启动
        if ( !_kf_deploy_agent_config->_is_auto_startup )
        {
            return;
        }

        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;
            StartupServerProcess( kfsetting );
        }
    }

    void KFDeployAgentModule::StartupServerProcess( KFLaunchSetting* kfsetting )
    {
        CheckServerProcess( kfsetting );
        if ( kfsetting->_is_shutdown || kfsetting->_process_id != 0 )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        auto ok = StartupWinProcess( kfsetting );
#else
        auto ok = StartupLinuxProcess( kfsetting );
#endif
        if ( ok )
        {
            kfsetting->_startup_time = KFGlobal::Instance()->_real_time;
            UpdateProcessToServer( kfsetting );

            // 保存到文件中
            SaveProcessToFile( kfsetting );
        }
    }

    void KFDeployAgentModule::CheckServerProcess( KFLaunchSetting* kfsetting )
    {
        if ( kfsetting->_process_id == 0 )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        CheckWinProcess( kfsetting );
#else
        CheckLinuxProcess( kfsetting );
#endif
        if ( kfsetting->_process_id != 0 )
        {
            return;
        }

        // 更新状态
        UpdateProcessToServer( kfsetting );

        // 保存到文件中
        SaveProcessToFile( kfsetting );
    }

    void KFDeployAgentModule::BindServerProcess()
    {
        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;

            ReadProcessFromFile( kfsetting );
            if ( kfsetting->_process_id == 0 )
            {
                continue;
            }

            UpdateProcessToServer( kfsetting );
        }
    }

    void KFDeployAgentModule::KillServerProcess( uint32 processid )
    {
        if ( processid == 0 )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        KillWinProcess( processid );
#else
        KillLinuxProcess( processid );
#endif
    }

#if __KF_SYSTEM__ == __KF_WIN__
    bool KFDeployAgentModule::StartupWinProcess( KFLaunchSetting* kfsetting )
    {
        // 启动进程
        STARTUPINFO startupinfo;
        memset( &startupinfo, 0, sizeof( STARTUPINFO ) );
        startupinfo.cb = sizeof( STARTUPINFO );
        startupinfo.wShowWindow = SW_SHOW;
        startupinfo.dwFlags = STARTF_USESHOWWINDOW;

        uint32 createflag = CREATE_NEW_CONSOLE;
        if ( !_kf_deploy_agent_config->_is_show_window )
        {
            createflag = CREATE_NO_WINDOW;
        }

        auto apppatch = _kf_deploy_agent_config->_deploy_path + kfsetting->_app_path + "/";
        auto startupfile = apppatch + kfsetting->GetStartupFile();
        auto param = __FORMAT__( " {}={} {}={} {}={}",
                                 __KF_STRING__( appid ), kfsetting->_app_id,
                                 __KF_STRING__( log ), kfsetting->_log_type,
                                 __KF_STRING__( startup ), kfsetting->_app_config );

        // 启动进程
        PROCESS_INFORMATION processinfo;
        BOOL result = CreateProcess( startupfile.c_str(), const_cast< char* >( param.c_str() ), NULL, NULL, FALSE,
                                     createflag, NULL, apppatch.c_str(), &startupinfo, &processinfo );

        kfsetting->_process_id = processinfo.dwProcessId;
        if ( kfsetting->_process_id != _invalid_int )
        {
            __LOG_DEBUG__( KFLogEnum::Logic, "startup [ {}:{}:{} ] ok! process={}",
                           kfsetting->_app_name, kfsetting->_app_type, kfsetting->_app_id, kfsetting->_process_id );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::System, "startup [ {}:{}:{} ] failed!",
                           kfsetting->_app_name, kfsetting->_app_type, kfsetting->_app_id );
        }

        return kfsetting->_process_id != _invalid_int;
    }

    void KFDeployAgentModule::CheckWinProcess( KFLaunchSetting* kfsetting )
    {
        DWORD flag = PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION;
        HANDLE handle = OpenProcess( flag, FALSE, kfsetting->_process_id );
        if ( handle != nullptr )
        {
            DWORD code = STILL_ACTIVE;
            GetExitCodeProcess( handle, &code );
            if ( code == STILL_ACTIVE )
            {
                return;
            }

            TerminateProcess( handle, 0 );
        }

        kfsetting->_process_id = 0;
    }

    void KFDeployAgentModule::KillWinProcess( uint32 processid )
    {
        DWORD flag = PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION;
        HANDLE handle = OpenProcess( flag, FALSE, processid );
        if ( handle == nullptr )
        {
            return;
        }

        TerminateProcess( handle, 0 );
    }

#else
    bool KFDeployAgentModule::StartupLinuxProcess( KFLaunchSetting* kfsetting )
    {
        std::vector<char*> args;

        auto apppath = _kf_deploy_agent_config->_deploy_path + kfsetting->_app_path + "/";
        auto startupfile = apppath + kfsetting->GetStartupFile();
        args.push_back( const_cast< char* >( startupfile.c_str() ) );

        auto strpause = __FORMAT__( "{}={}", __KF_CHAR__( appid ), kfsetting->_app_id );
        args.push_back( const_cast< char* >( strpause.c_str() ) );

        auto strappid = __FORMAT__( "{}={}", __KF_CHAR__( log ), kfsetting->_log_type );
        args.push_back( const_cast< char* >( strappid.c_str() ) );

        auto strfile = __FORMAT__( "{}={}", __KF_CHAR__( startup ), kfsetting->_app_config );
        args.push_back( const_cast< char* >( strfile.c_str() ) );
        args.push_back( nullptr );

        auto pid = fork();
        if ( pid == 0 )	 // 子进程
        {
            // 新的目录
            if ( chdir( apppath.c_str() ) != 0 )
            {
                _exit( 72 );
            }

            // 关闭资源
            for ( int i = 3; i < sysconf( _SC_OPEN_MAX ); ++i )
            {
                close( i );
            }

            // 启动新进程
            execvp( args[ 0 ], &args[ 0 ] );
            _exit( 72 );
        }

        // 等待返回
        int32 rc;
        do
        {
            int32 status;
            rc = waitpid( pid, &status, 0 );
        } while ( rc < 0 && errno == EINTR );

        kfsetting->_process_id = FindProcessIdByName( kfsetting, startupfile );
        return true;
    }

    uint32 KFDeployAgentModule::FindProcessIdByName( KFLaunchSetting* kfsetting, const std::string& startupfile )
    {
        auto strshell = __FORMAT__( "ps -ef|grep '{} {}={}'|grep -v 'grep'|awk '{{print $2}}'",
                                    startupfile, __KF_STRING__( appid ), kfsetting->_app_id );
        FILE* fp = popen( strshell.c_str(), "r" );
        if ( fp != nullptr )
        {
            // 逐行读取执行结果
            char buff[ 32 ] = { 0 };
            if ( fgets( buff, sizeof( buff ), fp ) != NULL )
            {
                return atoi( buff );
            }

            // 关闭管道指针
            pclose( fp );
        }

        return _invalid_int;
    }

    void KFDeployAgentModule::CheckLinuxProcess( KFLaunchSetting* kfsetting )
    {
        if ( kill( kfsetting->_process_id, 0 ) != 0 )
        {
            kfsetting->_process_id = 0;
        }
    }

    void KFDeployAgentModule::KillLinuxProcess( uint32 processid )
    {
        auto strkill = __FORMAT__( "kill -s 9 {}", processid );
        FILE* fp = popen( strkill.c_str(), "r" );
        if ( fp != nullptr )
        {
            pclose( fp );
        }
    }
#endif

    void KFDeployAgentModule::UpdateProcessToServer( KFLaunchSetting* kfsetting )
    {
        if ( _deploy_server_id == 0 )
        {
            return;
        }

        KFMsg::S2SUpdateServerStatusReq req;
        req.set_agentid( KFGlobal::Instance()->_app_id );
        req.set_appid( kfsetting->_app_id );
        req.set_appname( kfsetting->_app_name );
        req.set_apptype( kfsetting->_app_type );
        req.set_zoneid( kfsetting->_zone_id );
        req.set_process( kfsetting->_process_id );
        req.set_startuptime( kfsetting->_startup_time );
        req.set_isshutdown( kfsetting->_is_shutdown );
        _kf_tcp_client->SendNetMessage( _deploy_server_id, KFMsg::S2S_UPDATE_SERVER_STATUS_REQ, &req );
    }

    std::string KFDeployAgentModule::FormatPidFileName( KFLaunchSetting* kfsetting )
    {
        // 文件名字
        return __FORMAT__( "{}/{}/{}-{}-{}", _pid_path, kfsetting->_app_path,
                           kfsetting->_app_name, kfsetting->_app_type, kfsetting->_app_id );
    }

    void KFDeployAgentModule::SaveProcessToFile( KFLaunchSetting* kfsetting )
    {
        auto file = FormatPidFileName( kfsetting );

        auto pidpath = _pid_path + "/" + kfsetting->_app_path;
        __MKDIR__( pidpath );

        std::ofstream offile( file );
        if ( !offile )
        {
            return;
        }

        offile << kfsetting->_process_id;
        offile << DEFAULT_SPLIT_STRING;
        offile << kfsetting->_startup_time;

        offile.flush();
        offile.close();
    }

    void KFDeployAgentModule::ReadProcessFromFile( KFLaunchSetting* kfsetting )
    {
        auto file = FormatPidFileName( kfsetting );

        std::ifstream infile( file );
        if ( !infile )
        {
            return;
        }

        std::ostringstream os;
        infile >> os.rdbuf();
        infile.close();

        std::string strdata = os.str();
        kfsetting->_process_id = KFUtility::SplitValue< uint32 >( strdata, DEFAULT_SPLIT_STRING );
        kfsetting->_startup_time = KFUtility::SplitValue< uint64 >( strdata, DEFAULT_SPLIT_STRING );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployAgentModule::HandleDeployCommandReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployCommandToAgentReq );

        auto pbdeploy = &kfmsg.deploycommand();

        if ( pbdeploy->command() == __KF_STRING__( restart ) )
        {
            AddDeployTask( __KF_STRING__( shutdown ), pbdeploy->value(), pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );
            AddDeployTask( __KF_STRING__( download ), pbdeploy->value(), pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );
            AddDeployTask( __KF_STRING__( startup ), pbdeploy->value(), pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );
        }
        else
        {
            AddDeployTask( pbdeploy->command(), pbdeploy->value(), pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );
        }
    }

    void KFDeployAgentModule::AddDeployTask( const std::string& command, const std::string& value, const std::string& appname, const std::string& apptype,
            uint32 appid, uint32 zoneid )
    {


        auto kftask = __KF_CREATE__( KFDeployTask );
        kftask->_command = command;
        kftask->_value = value;
        kftask->_app_name = appname;
        kftask->_app_type = apptype;
        kftask->_app_id = appid;
        kftask->_zone_id = zoneid;

        if ( _kf_task == nullptr )
        {
            _kf_task = kftask;
            StartDeployTask();
        }
        else
        {
            _deploy_task.push_back( kftask );

            __LOG_INFO__( KFLogEnum::Logic, "add task [{}:{} | {}:{}:{}:{}] tasklist cout[{}]!",
                          command, value, appname, apptype, appid, zoneid, _deploy_task.size() );
        }
    }

    __KF_TIMER_FUNCTION__( KFDeployAgentModule::OnTimerCheckTaskFinish )
    {
        if ( _kf_task == nullptr )
        {
            return;
        }

        auto ok = true;
        if ( _kf_task->_command == __KF_STRING__( kill ) || _kf_task->_command == __KF_STRING__( shutdown ) )
        {
            ok = CheckShutDownServerTaskFinish();
            if ( !ok )
            {
                // 判断当前时间, 如果任务不能完成, 直接杀死进程
                if ( KFGlobal::Instance()->_game_time > _kf_task->_start_time + 60000 )
                {
                    _kf_task->_command = __KF_STRING__( kill );
                    StartDeployTask();
                }
            }
        }
        else if ( _kf_task->_command == __KF_STRING__( startup ) )
        {
            ok = CheckStartupServerTaskFinish();
        }
        else if ( _kf_task->_command == __KF_STRING__( download ) )
        {
            ok = CheckUpdateServerTaskFinish();
        }

        if ( ok )
        {
            __LOG_INFO__( KFLogEnum::Logic, "[{}:{} | {}:{}:{}:{}] task finish!",
                          _kf_task->_command, _kf_task->_value, _kf_task->_app_name, _kf_task->_app_type,
                          _kf_task->_app_id, _kf_task->_zone_id );

            __KF_DESTROY__( KFDeployTask, _kf_task );
            _kf_task = nullptr;

            if ( !_deploy_task.empty() )
            {
                _kf_task = _deploy_task.front();
                _deploy_task.pop_front();

                StartDeployTask();
            }
        }
    }

    void KFDeployAgentModule::StartDeployTask()
    {
        _kf_task->_start_time = KFGlobal::Instance()->_game_time;

        if ( _kf_task->_command == __KF_STRING__( startup ) )
        {
            StartStartupServerTask();
        }
        else if ( _kf_task->_command == __KF_STRING__( kill ) )
        {
            StartKillServerTask();
        }
        else if ( _kf_task->_command == __KF_STRING__( shutdown ) )
        {
            StartShutDownServerTask();
        }
        else if ( _kf_task->_command == __KF_STRING__( download ) )
        {
            StartUpdateServerTask();
        }
        else
        {
            SendTaskToMaster();
        }

        __LOG_INFO__( KFLogEnum::Logic, "[{}:{} | {}:{}:{}:{}] task start!",
                      _kf_task->_command, _kf_task->_value, _kf_task->_app_name, _kf_task->_app_type,
                      _kf_task->_app_id, _kf_task->_zone_id );
    }

    void KFDeployAgentModule::StartKillServerTask()
    {
        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;

            auto isserver = kfsetting->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                kfsetting->_is_shutdown = true;
                KillServerProcess( kfsetting->_process_id );
            }
        }
    }

    void KFDeployAgentModule::StartShutDownServerTask()
    {
        _kf_task->_start_time += KFUtility::ToValue< uint32 >( _kf_task->_value );

        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;

            auto isserver = kfsetting->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                kfsetting->_is_shutdown = true;
            }
        }

        SendTaskToMaster();
    }

    bool KFDeployAgentModule::CheckShutDownServerTaskFinish()
    {
        // 指定的server都关闭了
        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;

            auto isserver = kfsetting->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                if ( kfsetting->_process_id != 0 )
                {
                    return false;
                }
            }
        }

        return true;
    }

    void KFDeployAgentModule::StartStartupServerTask()
    {
        __REGISTER_LOOP_TIMER__( 1, 10000, &KFDeployAgentModule::OnTimerStartupProcess );

        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;

            auto isserver = kfsetting->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                kfsetting->_is_shutdown = false;
            }
        }
    }

    bool KFDeployAgentModule::CheckStartupServerTaskFinish()
    {
        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;

            auto isserver = kfsetting->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                if ( kfsetting->_process_id == 0 )
                {
                    return false;
                }
            }
        }

        return true;
    }

    void KFDeployAgentModule::StartUpdateServerTask()
    {
        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;

            auto isserver = kfsetting->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                kfsetting->_is_download = true;
                _kf_ftp->StartDownload( _kf_deploy_agent_config->_ftp_id, kfsetting->_app_path, this, &KFDeployAgentModule::OnFtpDownLoadCallBack );
            }
        }
    }

    bool KFDeployAgentModule::CheckUpdateServerTaskFinish()
    {
        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;

            auto isserver = kfsetting->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                if ( kfsetting->_is_download )
                {
                    return false;
                }
            }
        }

        return true;
    }

    void KFDeployAgentModule::OnFtpDownLoadCallBack( uint32 objectid, const std::string& apppath, bool ftpok )
    {
        // 设置不在下载
        for ( auto& iter : _kf_deploy_agent_config->_kf_launch_setting._objects )
        {
            auto kfsetting = iter.second;

            auto isserver = kfsetting->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                if ( kfsetting->_app_path == apppath )
                {
                    kfsetting->_is_download = false;
                }
            }
        }

        if ( !ftpok )
        {
            // 下载失败, 重新启动任务
            auto kftask = __KF_CREATE__( KFDeployTask );
            *kftask = *_kf_task;
            _deploy_task.push_front( kftask );
        }
    }

    void KFDeployAgentModule::SendTaskToMaster()
    {
        KFMsg::S2SDeployCommandToMasterReq req;
        auto pbdeploy = req.mutable_deploycommand();

        pbdeploy->set_command( _kf_task->_command );
        pbdeploy->set_value( _kf_task->_value );
        pbdeploy->set_appname( _kf_task->_app_name );
        pbdeploy->set_apptype( _kf_task->_app_type );
        pbdeploy->set_appid( _kf_task->_app_id );
        pbdeploy->set_zoneid( _kf_task->_zone_id );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_MASTER_REQ, &req );
    }
}