#include "KFDeployAgentModule.h"

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
    void KFDeployAgentModule::AfterLoad()
    {
        auto kfglobal = KFGlobal::Instance();
        _deploy_driver = _kf_mysql->CreateExecute( __KF_STRING__( deploy ) );
        if ( _deploy_driver == nullptr )
        {
            return __LOG_ERROR__( "deploy mysql is nullprt" );
        }

        _deploy_table_name = __FORMAT__( "{}_{}_deploy", kfglobal->_app_id._union._app_data._channel_id, kfglobal->_service_type );

        // 获得本机ip, 查询appid
        auto localip = _kf_ip_address->GetLocalIp();
        auto queryappid = _deploy_driver->QueryString( "select `{}` from `{}` where `{}`='{}'",
                          __KF_STRING__( strappid ), __KF_STRING__( agent ), __KF_STRING__( localip ), localip );
        if ( !queryappid->IsOk() )
        {
            return;
        }

        auto strappid = queryappid->_value;
        if ( strappid.empty() )
        {
            // 插入新的appid
            MapString values;
            values[ __KF_STRING__( localip ) ] = localip;
            _deploy_driver->Insert( __KF_STRING__( agent ), values );

            // 查询新的appid
            auto queryid = _deploy_driver->QueryString( "select `{}` from `{}` where `{}`='{}'",
                           __KF_STRING__( id ), __KF_STRING__( agent ), __KF_STRING__( localip ), localip );
            if ( !queryid->IsOk() || queryid->_value.empty() )
            {
                return;
            }

            KFAppID kfappid( 0 );
            kfappid._union._app_data._channel_id = kfglobal->_app_id._union._app_data._channel_id;
            kfappid._union._app_data._zone_id = 0;
            kfappid._union._app_data._server_type = KFServerEnum::DeployAgent;
            kfappid._union._app_data._worker_id = KFUtility::ToValue< uint16 >( queryid->_value );

            strappid = kfappid.ToString();
            values[ __KF_STRING__( id ) ] = queryid->_value;
            values[ __KF_STRING__( strappid ) ] = strappid;
            values[ __KF_STRING__( appid ) ] = __TO_STRING__( kfappid._union._id );
            _deploy_driver->Insert( __KF_STRING__( agent ), values );
        }

        KFAppID kfappid( strappid );
        kfglobal->_app_id = kfappid;
        kfglobal->_str_app_id = strappid;
    }

    void KFDeployAgentModule::BeforeRun()
    {
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFDeployAgentModule::OnClientConnectServer );
        __REGISTER_LOOP_TIMER__( 1, 20000, &KFDeployAgentModule::OnTimerStartupProcess );
        __REGISTER_LOOP_TIMER__( 2, 1000, &KFDeployAgentModule::OnTimerCheckTaskFinish );
        ////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &KFDeployAgentModule::HandleDeployCommandReq );
    }

    void KFDeployAgentModule::ShutDown()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_SCHEDULE_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        //////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ );
    }

    static std::string _pid_path = "./pid";

    void KFDeployAgentModule::OnceRun()
    {
        __MKDIR__( _pid_path );

        // 加载部署信息
        try
        {
            LoadTotalLaunchData();
        }
        catch ( ... )
        {
            __LOG_ERROR__( "load launch exception!" );
        }

        // 启动计划任务
        auto kfsetting = _kf_schedule->CreateScheduleSetting();
        kfsetting->SetDate( KFScheduleEnum::Loop, 0, 5 );
        kfsetting->SetData( _invalid_int, nullptr, _invalid_int );
        __REGISTER_SCHEDULE_FUNCTION__( kfsetting, &KFDeployAgentModule::OnScheduleRemoveVersion );

        // 启动连接deployserver
        StartConnectDeployServer();
    }

    __KF_SCHEDULE_FUNCTION__( KFDeployAgentModule::OnScheduleRemoveVersion )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 删除旧的版本号
        ExecuteShell( "rm wget-log*" );
        ExecuteShell( "rm -rf ./version/" );
#endif
    }

    void KFDeployAgentModule::StartConnectDeployServer()
    {
        auto deploydata = KFGlobal::Instance()->_startup_params[ __KF_STRING__( deploy ) ];
        if ( deploydata.empty() )
        {
            return;
        }

        auto serverid = KFUtility::SplitString( deploydata, "|" );
        auto ip = KFUtility::SplitString( deploydata, "|" );
        auto port = KFUtility::SplitValue< uint32 >( deploydata, "|" );

        auto appid = KFAppID::ToUInt64( serverid );
        _kf_tcp_client->StartClient( __KF_STRING__( deploy ), __KF_STRING__( server ), appid, ip, port );
    }


    void KFDeployAgentModule::LoadTotalLaunchData()
    {
        _deploy_list.Clear();

        // 部署信息
        {
            MapString keyvalue;
            keyvalue[ __KF_STRING__( localip ) ] = KFGlobal::Instance()->_local_ip;
            auto querydeploydata = _deploy_driver->Select( _deploy_table_name, keyvalue );
            for ( auto& values : querydeploydata->_value )
            {
                auto deploydata = __KF_NEW__( KFDeployData );
                deploydata->CopyFrom( values );
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
            auto kfglobal = KFGlobal::Instance();

            KFAppID kfappid( serverid );
            kfappid._union._app_data._channel_id = kfglobal->_app_id._union._app_data._channel_id;
            _deploy_server_id = kfappid._union._id;

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
            __LOG_ERROR__( "startup exception={}!", exception.what() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "startup exception unknown!" );
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

            __LOG_INFO__( "[{}:{}:{}] startup ok!", deploydata->_app_name, deploydata->_app_type, deploydata->_app_id );
        }
    }

    void KFDeployAgentModule::CheckServerProcess( KFDeployData* deploydata )
    {
        if ( deploydata->_process_id == _invalid_int )
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
            __LOG_ERROR__( "kill exception={}!", exception.what() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "kill exception unknown!" );
        }
    }

#if __KF_SYSTEM__ == __KF_WIN__
    bool KFDeployAgentModule::StartupWinProcess( KFDeployData* deploydata )
    {
        auto kfglobal = KFGlobal::Instance();

        // 启动进程
        STARTUPINFO startupinfo;
        memset( &startupinfo, 0, sizeof( STARTUPINFO ) );
        startupinfo.cb = sizeof( STARTUPINFO );
        startupinfo.wShowWindow = SW_SHOW;
        startupinfo.dwFlags = STARTF_USESHOWWINDOW;

        uint32 createflag = CREATE_NO_WINDOW;
        auto apppath = deploydata->GetAppPath();
        auto startupfile = deploydata->GetStartupFile( deploydata->_is_debug );
        auto param = __FORMAT__( " {}={} {}={} {}={} {}={} {}={} {}={}|{}|{}",
                                 __KF_STRING__( appname ), deploydata->_app_name,
                                 __KF_STRING__( apptype ), deploydata->_app_type,
                                 __KF_STRING__( appid ), deploydata->_app_id,
                                 __KF_STRING__( log ), deploydata->_log_type,
                                 __KF_STRING__( service ), deploydata->_service_type,
                                 __KF_STRING__( agent ), kfglobal->_str_app_id, kfglobal->_local_ip, kfglobal->_listen_port );

        // 启动进程
        PROCESS_INFORMATION processinfo;
        BOOL result = CreateProcess( startupfile.c_str(), const_cast< char* >( param.c_str() ), NULL, NULL, FALSE,
                                     createflag, NULL, apppath.c_str(), &startupinfo, &processinfo );

        deploydata->_process_id = processinfo.dwProcessId;
        if ( deploydata->_process_id != _invalid_int )
        {
            __LOG_DEBUG__( "startup [ {}:{}:{} ] ok! process={}",
                           deploydata->_app_name, deploydata->_app_type, deploydata->_app_id, deploydata->_process_id );
        }
        else
        {
            __LOG_ERROR__( "startup [ {}:{}:{} ] failed!",
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
        std::vector<char*> args;
        auto kfglobal = KFGlobal::Instance();

        auto startupfile = deploydata->GetStartupFile( deploydata->_is_debug );
        args.push_back( const_cast< char* >( startupfile.c_str() ) );

        auto strappname = __FORMAT__( "{}={}", __KF_STRING__( appname ), deploydata->_app_name );
        args.push_back( const_cast< char* >( strappname.c_str() ) );

        auto strapptype = __FORMAT__( "{}={}", __KF_STRING__( apptype ), deploydata->_app_type );
        args.push_back( const_cast< char* >( strapptype.c_str() ) );

        auto strappid = __FORMAT__( "{}={}", __KF_STRING__( appid ), deploydata->_app_id );
        args.push_back( const_cast< char* >( strappid.c_str() ) );

        auto strlogtype = __FORMAT__( "{}={}", __KF_STRING__( log ), deploydata->_log_type );
        args.push_back( const_cast< char* >( strlogtype.c_str() ) );

        auto strservice = __FORMAT__( "{}={}", __KF_STRING__( service ), deploydata->_service_type );
        args.push_back( const_cast< char* >( strservice.c_str() ) );

        auto stragent = __FORMAT__( "{}={}|{}|{}", __KF_STRING__( agent ), kfglobal->_str_app_id, kfglobal->_local_ip, kfglobal->_listen_port );
        args.push_back( const_cast< char* >( stragent.c_str() ) );

        args.push_back( nullptr );

        auto pid = fork();
        if ( pid == 0 )	 // 子进程
        {
            // 新的目录
            auto apppath = deploydata->GetAppPath();
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

        deploydata->_process_id = FindProcessIdByName( deploydata );
        return true;
    }

    uint32 KFDeployAgentModule::FindProcessIdByName( KFDeployData* deploydata )
    {
        auto strprocessid = ExecuteShell( "ps -ef|grep '{}={}'|grep -v 'grep'|awk '{{print $2}}'", __KF_STRING__( appid ), deploydata->_app_id );
        return KFUtility::ToValue< uint32 >( strprocessid );
    }

    void KFDeployAgentModule::CheckLinuxProcess( KFDeployData* deploydata )
    {
        if ( kill( deploydata->_process_id, 0 ) != 0 )
        {
            deploydata->_process_id = FindProcessIdByName( deploydata );
        }
    }

    void KFDeployAgentModule::KillLinuxProcess( uint32 processid )
    {
        ExecuteShell( "kill -s 9 {}", processid );
    }

    std::string KFDeployAgentModule::ExecuteShellCommand( const std::string& command )
    {
        FILE* fp = popen( command.c_str(), "r" );
        if ( fp == nullptr )
        {
            __LOG_ERROR__( "[{}] open failed!", command );
            return _invalid_str;
        }

        char buffer[ 1024 ] = { 0 };
        if ( fgets( buffer, sizeof( buffer ), fp ) != NULL )
        {
            auto length = strlen( buffer ) - 1;
            if ( buffer[ length ] == '\n' )
            {
                buffer[ length ] = '\0'; //去除换行符
            }
        }

        pclose( fp );
        return buffer;
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
        keyvalues[ __KF_STRING__( service ) ] = deploydata->_service_type;
        _deploy_driver->Update( _deploy_table_name, keyvalues, updatevalues );
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

    bool KFDeployAgentModule::IsAgentDeploy( const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid )
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            auto isserver = deploydata->IsAppServer( appname, apptype, appid, zoneid );
            if ( isserver )
            {
                return true;
            }
        }

        return false;
    }

    void KFDeployAgentModule::FindAppDeployPath( const std::string& appname, std::set<std::string>& deploypathlist )
    {
        deploypathlist.clear();

        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            if ( appname != _globbing_str )
            {
                if ( appname == deploydata->_app_name )
                {
                    deploypathlist.insert( deploydata->_deploy_path );
                    break;
                }
            }
            else
            {
                deploypathlist.insert( deploydata->_deploy_path );
            }
        }
    }

    void KFDeployAgentModule::SendLogMessage( const std::string& url, const std::string& msg )
    {
        __LOG_DEBUG__( "{}", msg );

        if ( !url.empty() )
        {
            auto strmsg = __FORMAT__( "agent[{}] {}", KFGlobal::Instance()->_str_app_id, msg );

            __JSON_DOCUMENT__( response );
            __JSON_SET_VALUE__( response, __KF_STRING__( msg ), strmsg );
            _kf_http_client->MTGet< KFDeployAgentModule >( url, response );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDeployAgentModule::HandleDeployCommandReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployCommandToAgentReq );

        auto pbdeploy = kfmsg.mutable_deploycommand();

        // 判断是否agent的进程
        auto ok = IsAgentDeploy( pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );
        if ( !ok )
        {
            return;
        }

        if ( pbdeploy->command() == __KF_STRING__( restart ) )
        {
            AddDeployTask( __KF_STRING__( shutdown ), pbdeploy );
            AddDeployTask( __KF_STRING__( startup ), pbdeploy );
        }
        else if ( pbdeploy->command() == __KF_STRING__( version ) )
        {
            if ( pbdeploy->value() != _invalid_str )
            {
                AddDeployTask( __KF_STRING__( shutdown ), pbdeploy );
                AddDeployTask( __KF_STRING__( wget ), pbdeploy );
                AddDeployTask( __KF_STRING__( startup ), pbdeploy );
            }
        }
        else if ( pbdeploy->command() == __KF_STRING__( reloadconfig ) )
        {
            AddDeployTask( __KF_STRING__( downfile ), pbdeploy );
            AddDeployTask( __KF_STRING__( loadconfig ), pbdeploy );
        }
        else if ( pbdeploy->command() == __KF_STRING__( reloadscript ) )
        {
            AddDeployTask( __KF_STRING__( downfile ), pbdeploy );
            AddDeployTask( __KF_STRING__( loadscript ), pbdeploy );
        }
        else if ( pbdeploy->command() == __KF_STRING__( cleantask ) )
        {
            for ( auto kftask : _deploy_task )
            {
                __KF_DELETE__( KFDeployTask, kftask );
            }
            _deploy_task.clear();

            if ( _kf_task != nullptr )
            {
                __KF_DELETE__( KFDeployTask, _kf_task );
                _kf_task = nullptr;
            }

            LogDeploy( pbdeploy->logurl(), "task cleanup ok!" );
        }
        else
        {
            AddDeployTask( pbdeploy->command(), pbdeploy );
        }
    }

    void KFDeployAgentModule::AddDeployTask( const std::string& command, KFMsg::PBDeployCommand* pbdeploy )
    {
        auto kftask = __KF_NEW__( KFDeployTask );
        kftask->_command = command;
        kftask->_value = pbdeploy->value();
        kftask->_app_name = pbdeploy->appname();
        kftask->_app_type = pbdeploy->apptype();
        kftask->_app_id = pbdeploy->appid();
        kftask->_zone_id = pbdeploy->zoneid();
        kftask->_log_url = pbdeploy->logurl();

        if ( _kf_task == nullptr )
        {
            _kf_task = kftask;
            StartDeployTask();
        }
        else
        {
            _deploy_task.push_back( kftask );

            LogDeploy( pbdeploy->logurl(), "add task [{}:{} | {}:{}:{}:{}] tasklist cout[{}]!",
                       kftask->_command, kftask->_value,
                       kftask->_app_name, kftask->_app_type,
                       kftask->_app_id, kftask->_zone_id, _deploy_task.size() );
        }
    }

    __KF_TIMER_FUNCTION__( KFDeployAgentModule::OnTimerCheckTaskFinish )
    {
        try
        {
            auto ok = CheckTaskFinish();
            if ( ok )
            {
                LogDeploy( _kf_task->_log_url, "[{}:{} | {}:{}:{}:{}] task finish!",
                           _kf_task->_command, _kf_task->_value,
                           _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );

                __KF_DELETE__( KFDeployTask, _kf_task );
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
            __LOG_ERROR__( "check finish exception={}!", exception.what() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "check finish exception unknown!" );
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
        else if ( _kf_task->_command == __KF_STRING__( wget ) )
        {
            ok = CheckWgetVersionTaskFinish();
        }
        else if ( _kf_task->_command == __KF_STRING__( downfile ) )
        {
            ok = CheckDownFileTaskFinish();
        }

        return ok;
    }

    void KFDeployAgentModule::StartDeployTask()
    {
        _kf_task->_start_time = KFGlobal::Instance()->_game_time;

        try
        {
            LogDeploy( _kf_task->_log_url, "[{}:{} | {}:{}:{}:{}] task start!",
                       _kf_task->_command, _kf_task->_value,
                       _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );

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
            else if ( _kf_task->_command == __KF_STRING__( launch ) )
            {
                LoadTotalLaunchData();
            }
            else if ( _kf_task->_command == __KF_STRING__( wget ) )
            {
                StartWgetVersionTask();
            }
            else if ( _kf_task->_command == __KF_STRING__( downfile ) )
            {
                StartDownFileTask();
            }
            else
            {
                SendTaskToMaster();
            }
        }
        catch ( std::exception& exception )
        {
            LogDeploy( _kf_task->_log_url, "start task exception={}!", exception.what() );
        }
        catch ( ... )
        {
            LogDeploy( _kf_task->_log_url, "start task exception unknown!" );
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

                __LOG_INFO__( "[{}:{}:{}] kill ok!",
                              deploydata->_app_name, deploydata->_app_type, deploydata->_app_id );
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
        __REGISTER_LOOP_TIMER__( 1, 20000, &KFDeployAgentModule::OnTimerStartupProcess );

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

    void KFDeployAgentModule::SendTaskToMaster()
    {
        KFMsg::S2SDeployCommandToClientReq req;
        auto pbdeploy = req.mutable_deploycommand();

        pbdeploy->set_command( _kf_task->_command );
        pbdeploy->set_value( _kf_task->_value );
        pbdeploy->set_appname( _kf_task->_app_name );
        pbdeploy->set_apptype( _kf_task->_app_type );
        pbdeploy->set_appid( _kf_task->_app_id );
        pbdeploy->set_zoneid( _kf_task->_zone_id );
        pbdeploy->set_logurl( _kf_task->_log_url );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_CLIENT_REQ, &req );
    }

    void KFDeployAgentModule::StartWgetVersionTask()
    {
        // 查询版本路径
        auto queryurl = _deploy_driver->QueryString( "select `version_url` from version where `version_name`='{}';", _kf_task->_value );
        if ( !queryurl->IsOk() || queryurl->_value.empty() )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 执行下载命令
        ExecuteShell( "wget -c -P ./version/ {}", queryurl->_value );
#endif
    }

    bool KFDeployAgentModule::CheckWgetVersionTaskFinish()
    {
        auto querymd5 = _deploy_driver->QueryString( "select `version_md5` from version where `version_name`='{}';", _kf_task->_value );
        if ( !querymd5->IsOk() || querymd5->_value.empty() )
        {
            return true;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现

#else
        // 执行下载命令
        auto md5 = ExecuteShell( "md5sum ./version/{} | awk '{{print $1}}'", _kf_task->_value  );
        if ( md5 != querymd5->_value )
        {
            StartDeployTask();
            return false;
        }

        // 解压
        ExecuteShell( "rm -rf ./version/conf_output/" );
        ExecuteShell( "tar -zxf ./version/{} -C ./version/", _kf_task->_value );

        // 把文件拷贝过去
        std::set< std::string > deploypathlist;
        FindAppDeployPath( _kf_task->_app_name, deploypathlist );
        for ( auto& deploypath : deploypathlist )
        {
            ExecuteShell( "mkdir -p {}", deploypath );
            ExecuteShell( "rm {}/* -rf", deploypath );
            ExecuteShell( "cp -rf ./version/conf_output/* {}", deploypath );
            ExecuteShell( "chmod 777 -R {}", deploypath );
            ExecuteShell( "echo {} > {}/package.txt", _kf_task->_value, deploypath );
        }

        LogDeploy( _kf_task->_log_url, "[{}] update version ok!", _kf_task->_app_name );
#endif

        // 更新版本号
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                MapString updatevalues;
                updatevalues[ __KF_STRING__( version ) ] = _kf_task->_value;

                MapString keyvalues;
                keyvalues[ __KF_STRING__( appid ) ] = deploydata->_app_id;
                keyvalues[ __KF_STRING__( service ) ] = deploydata->_service_type;
                _deploy_driver->Update( _deploy_table_name, keyvalues, updatevalues );
            }
        }

        return true;
    }

    void KFDeployAgentModule::StartDownFileTask()
    {
        // 查询版本路径
        auto queryurl = _deploy_driver->QueryString( "select `file_url` from file where `file_name`='{}';", _kf_task->_value );
        if ( !queryurl->IsOk() || queryurl->_value.empty() )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 执行下载命令
        ExecuteShell( "wget -c -P ./version/ {}", queryurl->_value );
#endif
    }

    bool KFDeployAgentModule::CheckDownFileTaskFinish()
    {
        auto querymap = _deploy_driver->QueryMap( "select * from file where `file_name`='{}';", _kf_task->_value );
        if ( !querymap->IsOk() || querymap->_value.empty() )
        {
            return true;
        }

        auto querymd5 = querymap->_value[ "file_md5" ];
        auto querypath = querymap->_value[ "file_path" ];
        if ( querymd5.empty() || querypath.empty() )
        {
            return false;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现

#else
        // 执行下载命令
        auto md5 = ExecuteShell( "md5sum ./version/{} | awk '{{print $1}}'", _kf_task->_value );
        if ( md5 != querymd5 )
        {
            StartDownFileTask();
            return false;
        }

        // 把文件拷贝过去
        std::set< std::string > deploypathlist;
        FindAppDeployPath( _kf_task->_app_name, deploypathlist );
        for ( auto& deploypath : deploypathlist )
        {
            ExecuteShell( "mkdir -p {}", deploypath );
            ExecuteShell( "cp -rf ./version/{} {}/{}/", _kf_task->_value, deploypath, querypath );
        }

        LogDeploy( _kf_task->_log_url, "update file {} ok!", _kf_task->_value );
#endif

        return true;
    }
}