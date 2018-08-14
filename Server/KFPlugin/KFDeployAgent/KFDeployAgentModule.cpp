#include "KFDeployAgentModule.h"
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
    }

    void KFDeployAgentModule::BeforeRun()
    {
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFDeployAgentModule::OnClientConnectServer );
        __REGISTER_LOOP_TIMER__( 1, 10000, &KFDeployAgentModule::OnTimerStartupProcess );
        __REGISTER_LOOP_TIMER__( 2, 1000, &KFDeployAgentModule::OnTimerCheckTaskFinish );
        ////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &KFDeployAgentModule::HandleDeployCommandReq );
    }


    void KFDeployAgentModule::ShutDown()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        //////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ );
    }

    static std::string _pid_path = "./pid";

    void KFDeployAgentModule::OnceRun()
    {
        __MKDIR__( _pid_path );
        _mysql_driver = _kf_mysql->CreateExecute( __KF_STRING__( deploy ) );
        if ( _mysql_driver == nullptr )
        {
            return __LOG_CRITICAL__( KFLogEnum::System, "deploy mysql is nullprt" );
        }

        // 加载部署信息
        try
        {
            LoadTotalLaunchData();
        }
        catch ( ... )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "load launch exception!" );
        }
    }

    void KFDeployAgentModule::LoadTotalLaunchData()
    {
        _launch_list.Clear();
        _deploy_list.Clear();

        // 加载launch设定
        {
            auto querylaunchdata = _mysql_driver->Select( __KF_STRING__( launch ) );
            for ( auto& values : querylaunchdata->_value )
            {
                auto kfsetting = __KF_CREATE__( KFLaunchSetting );
                kfsetting->CopyFrom( values );

                LaunchKey key( kfsetting->_app_name, kfsetting->_app_type );
                _launch_list.Insert( key, kfsetting );
            }
        }

        // 部署信息
        {
            MapString keyvalue;
            keyvalue[ __KF_STRING__( localip ) ] = KFGlobal::Instance()->_local_ip;
            auto querydeploydata = _mysql_driver->Select( __KF_STRING__( deploy ), keyvalue );
            for ( auto& values : querydeploydata->_value )
            {
                auto deploydata = __KF_CREATE__( KFDeployData );
                deploydata->CopyFrom( values );
                deploydata->_kf_launch = _launch_list.Find( LaunchKey( deploydata->_app_name, deploydata->_app_type ) );
                _deploy_list.Insert( deploydata->_app_id, deploydata );
            }
        }

        BindServerProcess();
    }

    __KF_CLIENT_CONNECT_FUNCTION__( KFDeployAgentModule::OnClientConnectServer )
    {
        // 连接成功
        if ( servername == __KF_STRING__( deploy ) && servertype == __KF_STRING__( server ) )
        {
            _deploy_server_id = serverid;

            auto kfglobal = KFGlobal::Instance();

            // 把自己注册到Services
            KFMsg::S2SRegisterAgentToServerReq req;
            req.set_agentid( kfglobal->_str_app_id );
            req.set_name( kfglobal->_app_name );
            req.set_type( kfglobal->_app_type );
            req.set_port( kfglobal->_listen_port );
            req.set_localip( _kf_ip_address->GetLocalIp() );
            _kf_tcp_client->SendNetMessage( serverid, KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &req );
        }
    }

    __KF_TIMER_FUNCTION__( KFDeployAgentModule::OnTimerStartupProcess )
    {

        try
        {
            for ( auto& iter : _deploy_list._objects )
            {
                auto deploydata = iter.second;
                StartupServerProcess( deploydata );
            }
        }
        catch ( std::exception& exception )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "startup exception={}!", exception.what() );
        }
        catch ( ... )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "startup exception unknown!" );
        }
    }

    void KFDeployAgentModule::StartupServerProcess( KFDeployData* deploydata )
    {
        CheckServerProcess( deploydata );
        if ( !deploydata->_is_startup || deploydata->_is_shutdown || deploydata->_process_id != _invalid_int )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        auto ok = StartupWinProcess( deploydata );
#else
        auto ok = StartupLinuxProcess( deploydata );
#endif
        if ( ok )
        {
            deploydata->_startup_time = KFGlobal::Instance()->_real_time;
            UpdateDeployToDatabase( deploydata );

            // 保存到文件中
            SaveProcessToFile( deploydata );
        }
    }

    void KFDeployAgentModule::CheckServerProcess( KFDeployData* deploydata )
    {
        if ( deploydata->_process_id == _invalid_int || deploydata->_kf_launch == nullptr )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        CheckWinProcess( deploydata );
#else
        CheckLinuxProcess( deploydata );
#endif
        if ( deploydata->_process_id != _invalid_int )
        {
            return;
        }

        // 更新状态
        UpdateDeployToDatabase( deploydata );

        // 保存到文件中
        SaveProcessToFile( deploydata );
    }

    void KFDeployAgentModule::BindServerProcess()
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;
            if ( deploydata->_process_id == _invalid_int )
            {
                ReadProcessFromFile( deploydata );
                if ( deploydata->_process_id != _invalid_int )
                {
                    UpdateDeployToDatabase( deploydata );
                }
            }
        }
    }

    void KFDeployAgentModule::KillServerProcess( uint32 processid )
    {
        if ( processid == 0 )
        {
            return;
        }

        try
        {
#if __KF_SYSTEM__ == __KF_WIN__
            KillWinProcess( processid );
#else
            KillLinuxProcess( processid );
#endif
        }
        catch ( std::exception& exception )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "kill exception={}!", exception.what() );
        }
        catch ( ... )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "kill exception unknown!" );
        }
    }

#if __KF_SYSTEM__ == __KF_WIN__
    bool KFDeployAgentModule::StartupWinProcess( KFDeployData* deploydata )
    {
        auto kflaunch = deploydata->_kf_launch;
        if ( kflaunch == nullptr )
        {
            return false;
        }

        // 启动进程
        STARTUPINFO startupinfo;
        memset( &startupinfo, 0, sizeof( STARTUPINFO ) );
        startupinfo.cb = sizeof( STARTUPINFO );
        startupinfo.wShowWindow = SW_SHOW;
        startupinfo.dwFlags = STARTF_USESHOWWINDOW;

        uint32 createflag = CREATE_NO_WINDOW;
        auto apppath = kflaunch->GetAppPath();
        auto startupfile = kflaunch->GetStartupFile();
        auto param = __FORMAT__( " {}={} {}={} {}={}",
                                 __KF_STRING__( appid ), deploydata->_app_id,
                                 __KF_STRING__( log ), kflaunch->_log_type,
                                 __KF_STRING__( startup ), kflaunch->_app_config );

        // 启动进程
        PROCESS_INFORMATION processinfo;
        BOOL result = CreateProcess( startupfile.c_str(), const_cast< char* >( param.c_str() ), NULL, NULL, FALSE,
                                     createflag, NULL, apppath.c_str(), &startupinfo, &processinfo );

        deploydata->_process_id = processinfo.dwProcessId;
        if ( deploydata->_process_id != _invalid_int )
        {
            __LOG_DEBUG__( KFLogEnum::Logic, "startup [ {}:{}:{} ] ok! process={}",
                           deploydata->_app_name, deploydata->_app_type, deploydata->_app_id, deploydata->_process_id );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::System, "startup [ {}:{}:{} ] failed!",
                           deploydata->_app_name, deploydata->_app_type, deploydata->_app_id );
        }

        return deploydata->_process_id != _invalid_int;
    }

    void KFDeployAgentModule::CheckWinProcess( KFDeployData* deploydata )
    {
        DWORD flag = PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION;
        HANDLE handle = OpenProcess( flag, FALSE, deploydata->_process_id );
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

        deploydata->_process_id = 0;
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
    bool KFDeployAgentModule::StartupLinuxProcess( KFDeployData* deploydata )
    {
        auto kflaunch = deploydata->_kf_launch;
        if ( kflaunch == nullptr )
        {
            return false;
        }

        std::vector<char*> args;

        auto startupfile = kflaunch->GetStartupFile();
        args.push_back( const_cast< char* >( startupfile.c_str() ) );

        auto strpause = __FORMAT__( "{}={}", __KF_STRING__( appid ), deploydata->_app_id );
        args.push_back( const_cast< char* >( strpause.c_str() ) );

        auto strappid = __FORMAT__( "{}={}", __KF_STRING__( log ), kflaunch->_log_type );
        args.push_back( const_cast< char* >( strappid.c_str() ) );

        auto strfile = __FORMAT__( "{}={}", __KF_STRING__( startup ), kflaunch->_app_config );
        args.push_back( const_cast< char* >( strfile.c_str() ) );

        args.push_back( nullptr );

        auto pid = fork();
        if ( pid == 0 )	 // 子进程
        {
            // 新的目录
            auto apppath = kflaunch->GetAppPath();
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

        deploydata->_process_id = FindProcessIdByName( deploydata, startupfile );
        return true;
    }

    uint32 KFDeployAgentModule::FindProcessIdByName( KFDeployData* deploydata, const std::string& startupfile )
    {
        auto strshell = __FORMAT__( "ps -ef|grep '{} {}={}'|grep -v 'grep'|awk '{{print $2}}'",
                                    startupfile, __KF_STRING__( appid ), deploydata->_app_id );
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

    void KFDeployAgentModule::CheckLinuxProcess( KFDeployData* deploydata )
    {
        if ( kill( deploydata->_process_id, 0 ) != 0 )
        {
            auto startupfile = deploydata->_kf_launch->GetStartupFile();
            deploydata->_process_id = FindProcessIdByName( deploydata, startupfile );
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

    void KFDeployAgentModule::UpdateDeployToDatabase( KFDeployData* deploydata )
    {
        MapString updatevalues;
        updatevalues[ __KF_STRING__( shutdown ) ] = __TO_STRING__( deploydata->_is_shutdown ? 1 : 0 );
        updatevalues[ __KF_STRING__( process ) ] = __TO_STRING__( deploydata->_process_id );
        updatevalues[ __KF_STRING__( time ) ] = __TO_STRING__( deploydata->_startup_time );
        updatevalues[ __KF_STRING__( agentid ) ] = KFGlobal::Instance()->_str_app_id;

        MapString keyvalues;
        keyvalues[ __KF_STRING__( appid ) ] = deploydata->_app_id;
        _mysql_driver->Update( __KF_STRING__( deploy ), keyvalues, updatevalues );
    }

    std::string KFDeployAgentModule::FormatPidFileName( KFDeployData* deploydata )
    {
        // 文件名字
        return __FORMAT__( "{}/{}-{}-{}", _pid_path, deploydata->_app_name, deploydata->_app_type, deploydata->_app_id );
    }

    void KFDeployAgentModule::SaveProcessToFile( KFDeployData* deploydata )
    {
        auto file = FormatPidFileName( deploydata );

        auto pidpath = _pid_path + "/";
        __MKDIR__( pidpath );

        std::ofstream offile( file );
        if ( !offile )
        {
            return;
        }

        offile << deploydata->_process_id;
        offile << DEFAULT_SPLIT_STRING;
        offile << deploydata->_startup_time;

        offile.flush();
        offile.close();
    }

    void KFDeployAgentModule::ReadProcessFromFile( KFDeployData* deploydata )
    {
        auto file = FormatPidFileName( deploydata );

        std::ifstream infile( file );
        if ( !infile )
        {
            return;
        }

        std::ostringstream os;
        infile >> os.rdbuf();
        infile.close();

        std::string strdata = os.str();
        deploydata->_process_id = KFUtility::SplitValue< uint32 >( strdata, DEFAULT_SPLIT_STRING );
        deploydata->_startup_time = KFUtility::SplitValue< uint64 >( strdata, DEFAULT_SPLIT_STRING );
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
            const std::string& appid, uint32 zoneid )
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
        try
        {
            auto ok = CheckTaskFinish();
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
        catch ( std::exception& exception )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "check finish exception={}!", exception.what() );
        }
        catch ( ... )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "check finish exception unknown!" );
        }
    }

    bool KFDeployAgentModule::CheckTaskFinish()
    {
        if ( _kf_task == nullptr )
        {
            return false;
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

        return ok;
    }

    void KFDeployAgentModule::StartDeployTask()
    {
        _kf_task->_start_time = KFGlobal::Instance()->_game_time;

        try
        {
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
            else if ( _kf_task->_command == __KF_STRING__( launch ) )
            {
                LoadTotalLaunchData();
            }
            else
            {
                SendTaskToMaster();
            }

            __LOG_INFO__( KFLogEnum::Logic, "[{}:{} | {}:{}:{}:{}] task start!",
                          _kf_task->_command, _kf_task->_value, _kf_task->_app_name, _kf_task->_app_type,
                          _kf_task->_app_id, _kf_task->_zone_id );
        }
        catch ( std::exception& exception )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "start task exception={}!", exception.what() );
        }
        catch ( ... )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "start task exception unknown!" );
        }
    }

    void KFDeployAgentModule::StartKillServerTask()
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                deploydata->_is_shutdown = true;
                KillServerProcess( deploydata->_process_id );

                UpdateDeployToDatabase( deploydata );
            }
        }
    }

    void KFDeployAgentModule::StartShutDownServerTask()
    {
        _kf_task->_start_time += KFUtility::ToValue< uint32 >( _kf_task->_value );

        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                deploydata->_is_shutdown = true;
                UpdateDeployToDatabase( deploydata );
            }
        }

        SendTaskToMaster();
    }

    bool KFDeployAgentModule::CheckShutDownServerTaskFinish()
    {
        // 指定的server都关闭了
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                if ( deploydata->_process_id != _invalid_int )
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

        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                deploydata->_is_shutdown = false;
                UpdateDeployToDatabase( deploydata );
            }
        }
    }

    bool KFDeployAgentModule::CheckStartupServerTaskFinish()
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                if ( deploydata->_process_id == _invalid_int )
                {
                    return false;
                }
            }
        }

        return true;
    }

    void KFDeployAgentModule::StartUpdateServerTask()
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;
            auto kflaunch = deploydata->_kf_launch;
            if ( kflaunch == nullptr )
            {
                continue;
            }

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                deploydata->_is_download = true;
                _kf_ftp->StartDownload( kflaunch->_ftp_id, kflaunch->_app_path, this, &KFDeployAgentModule::OnFtpDownLoadCallBack );
            }
        }
    }

    bool KFDeployAgentModule::CheckUpdateServerTaskFinish()
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                if ( deploydata->_is_download )
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
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;
            auto kflaunch = deploydata->_kf_launch;
            if ( kflaunch == nullptr )
            {
                continue;
            }

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                if ( kflaunch->_app_path == apppath )
                {
                    deploydata->_is_download = false;
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