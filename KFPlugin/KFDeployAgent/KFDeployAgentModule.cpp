#include "KFDeployAgentModule.hpp"

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
    static std::string _pid_path = "./pid";
    void KFDeployAgentModule::BeforeRun()
    {
        auto timerid = 0;
        __LOOP_TIMER_1__( ++timerid, 20000, 100, &KFDeployAgentModule::OnTimerStartupProcess );
        __LOOP_TIMER_1__( ++timerid, 1000, 0, &KFDeployAgentModule::OnTimerCheckTaskFinish );
        __LOOP_TIMER_1__( ++timerid, 5000, 1000, &KFDeployAgentModule::OnTimerQueryAgentData );
        __LOOP_TIMER_1__( ++timerid, 30000, 100, &KFDeployAgentModule::OnTimerCheckHeartbeat );

        __REGISTER_SERVER_DISCOVER__( &KFDeployAgentModule::OnServerDiscoverClient );
        __REGISTER_CLIENT_CONNECTION__( &KFDeployAgentModule::OnClientConnectServer );
        ////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &KFDeployAgentModule::HandleDeployCommandReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_HEARTBEAT_TO_AGENT_REQ, &KFDeployAgentModule::HandleClientHeartbeatReq );
    }

    void KFDeployAgentModule::ShutDown()
    {
        __UN_TIMER_0__();

        __UN_SERVER_DISCOVER__();
        __UN_CLIENT_CONNECTION__();
        //////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEPLOY_HEARTBEAT_TO_AGENT_REQ );
    }

    void KFDeployAgentModule::PrepareRun()
    {
        __MKDIR__( _pid_path );
        _deploy_driver = _kf_mysql->Create( __STRING__( deploy ) );
    }

    __KF_TIMER_FUNCTION__( KFDeployAgentModule::OnTimerQueryAgentData )
    {
        // 获得本机ip, 查询appid
        auto localip = _kf_ip_address->GetLocalIp();
        auto kfquery = _deploy_driver->QueryMap( "select * from `agent` where `{}`='{}'", __STRING__( localip ), localip );
        if ( kfquery->_value.empty() )
        {
            return __LOG_ERROR__( "query agent=[{}] data failed!", localip );
        }

        auto strappid = kfquery->_value[ __STRING__( strappid ) ];
        if ( strappid.empty() )
        {
            return __LOG_ERROR__( "agent=[{}] appid is empty!", localip );
        }

        auto kfglobal = KFGlobal::Instance();
        kfglobal->_app_id->FromString( strappid );

        auto strservice = kfquery->_value[ __STRING__( service ) ];
        if ( strservice.empty() )
        {
            return __LOG_ERROR__( "agent=[{}] service is empty!", localip );
        }

        kfglobal->_channel = KFUtility::SplitValue< uint32 >( strservice, "." );
        kfglobal->_service = KFUtility::SplitValue< uint32 >( strservice, "." );

        // 部署表
        _deploy_table_name = __FORMAT__( "{}_{}_deploy", kfglobal->_channel, kfglobal->_service );
        __LOG_INFO__( "agent table name = [{}]", _deploy_table_name );

        // deploy server
        _deploy_server_strid = kfquery->_value[ "serverid" ];
        _deploy_server_ip = kfquery->_value[ "serverip" ];
        _deploy_server_port = KFUtility::ToValue<uint32>( kfquery->_value[ "serverport" ] );

        // 关闭定时器
        __UN_TIMER_1__( objectid );

        // 启动连接deployserver
        StartConnectDeployServer();

        // 读取进程启动配置
        LoadTotalLaunchData();
    }

    void KFDeployAgentModule::StartConnectDeployServer()
    {
        if ( _deploy_server_strid.empty() || _deploy_server_ip.empty() || _deploy_server_port == 0u )
        {
            return;
        }

        auto appid = KFAppId::ToUInt64( _deploy_server_strid );
        _kf_tcp_client->StartClient( __STRING__( deploy ), __STRING__( server ), appid, _deploy_server_ip, _deploy_server_port );
    }

    void KFDeployAgentModule::LoadTotalLaunchData()
    {
        if ( _deploy_table_name.empty() )
        {
            return __LOG_ERROR__( "depoly table name is empty!" );
        }

        try
        {
            MapString keyvalue;
            keyvalue[ __STRING__( agentid ) ] = KFGlobal::Instance()->_app_id->ToString();
            auto querydeploydata = _deploy_driver->Select( _deploy_table_name, keyvalue );
            if ( !querydeploydata->_value.empty() )
            {
                _deploy_list.Clear();
                for ( auto& values : querydeploydata->_value )
                {
                    auto deploydata = __KF_NEW__( KFDeployData );
                    deploydata->CopyFrom( values );
                    _deploy_list.Insert( deploydata->_app_id, deploydata );
                    __LOG_INFO__( "add server[{}:{}:{}] deploy!", deploydata->_app_name, deploydata->_app_type, deploydata->_app_id );
                }

                BindServerProcess();
            }
        }
        catch ( ... )
        {
            __LOG_ERROR__( "load launch exception!" );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFDeployAgentModule::OnClientConnectServer )
    {
        // 连接成功
        if ( netdata->_name == __STRING__( deploy ) && netdata->_type == __STRING__( server ) )
        {
            _deploy_server_id = netdata->_id;
            auto kfglobal = KFGlobal::Instance();

            // 把自己注册到Services
            KFMsg::S2SRegisterAgentToServerReq req;
            req.set_agentid( kfglobal->_app_id->ToString() );
            req.set_name( kfglobal->_app_name );
            req.set_type( kfglobal->_app_type );
            req.set_port( kfglobal->_listen_port );
            req.set_localip( _kf_ip_address->GetLocalIp() );
            req.set_service( __FORMAT__( "{}.{}", kfglobal->_channel, kfglobal->_service ) );
            _kf_tcp_client->SendNetMessage( netdata->_id, KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDeployAgentModule::HandleClientHeartbeatReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployHeartbeatToAgentReq );
        auto strid = KFAppId::ToString( kfmsg.id() );

        auto kfdeploydata = _deploy_list.Find( strid );
        if ( kfdeploydata == nullptr || kfdeploydata->_heartbeat == 0u )
        {
            return;
        }

        // 5分钟判断超时
        kfdeploydata->_heartbeat_timeout = KFGlobal::Instance()->_game_time + kfdeploydata->_heartbeat * KFTimeEnum::OneSecondMicSecond;
    }

    __KF_NET_EVENT_FUNCTION__( KFDeployAgentModule::OnServerDiscoverClient )
    {
        auto kfdeploydata = _deploy_list.Find( netdata->_str_id );
        if ( kfdeploydata == nullptr )
        {
            return;
        }

        kfdeploydata->_is_conencted = true;
    }

    __KF_NET_EVENT_FUNCTION__( KFDeployAgentModule::OnServerLostClient )
    {
        auto kfdeploydata = _deploy_list.Find( netdata->_str_id );
        if ( kfdeploydata == nullptr )
        {
            return;
        }

        kfdeploydata->_is_conencted = false;
    }

    __KF_TIMER_FUNCTION__( KFDeployAgentModule::OnTimerCheckHeartbeat )
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;
            if ( deploydata->_heartbeat == 0 ||
                    deploydata->_process_id == 0 ||
                    deploydata->_is_shutdown ||
                    !deploydata->_is_startup ||
                    !deploydata->_is_conencted ||
                    deploydata->_heartbeat == 0u ||
                    deploydata->_heartbeat_timeout > KFGlobal::Instance()->_game_time )
            {
                continue;
            }

            // 超时了, 认为client卡死了
            KillServerProcess( deploydata->_process_id );
            __LOG_INFO__( "app=[{}.{}] id=[{}] heartbeat timeout!", deploydata->_app_name, deploydata->_app_type, deploydata->_app_id );
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

            LogDeploy( "[{}:{}:{}] startup ok!", deploydata->_app_name, deploydata->_app_type, deploydata->_app_id );
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
        auto param = __FORMAT__( " {}={}.{} {}={} {}={} {}={} {}={}|{}|{}",
                                 __STRING__( app ), deploydata->_app_name, deploydata->_app_type,
                                 __STRING__( id ), deploydata->_app_id,
                                 __STRING__( service ), deploydata->_service_type,
                                 __STRING__( net ), deploydata->_net_type,
                                 __STRING__( log ), deploydata->_log_type,
                                 __STRING__( agent ), kfglobal->_app_id->ToString(), kfglobal->_local_ip, kfglobal->_listen_port );

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

#define __ADD_ARGS__( strparam )\
    {\
        auto size = strparam.size();\
        auto arg = new char[ size + 1 ]; \
        memcpy( arg, strparam.c_str(), size );\
        arg[ size ] = 0;\
        args.push_back( arg );\
    }

        auto startupfile = deploydata->GetStartupFile( deploydata->_is_debug );
        __ADD_ARGS__( startupfile );

        auto strappname = __FORMAT__( "{}={}.{}", __STRING__( app ), deploydata->_app_name, deploydata->_app_type );
        __ADD_ARGS__( strappname );

        auto strappid = __FORMAT__( "{}={}", __STRING__( id ), deploydata->_app_id );
        __ADD_ARGS__( strappid );

        auto strservice = __FORMAT__( "{}={}", __STRING__( service ), deploydata->_service_type );
        __ADD_ARGS__( strservice );

        auto strnet = __FORMAT__( "{}={}", __STRING__( net ), deploydata->_net_type );
        __ADD_ARGS__( strnet );

        auto strlogtype = __FORMAT__( "{}={}", __STRING__( log ), deploydata->_log_type );
        __ADD_ARGS__( strlogtype );

        auto stragent = __FORMAT__( "{}={}|{}|{}", __STRING__( agent ), kfglobal->_app_id->ToString(), kfglobal->_local_ip, kfglobal->_listen_port );
        __ADD_ARGS__( stragent );

        auto params = deploydata->_params;
        while ( !params.empty() )
        {
            auto param = KFUtility::SplitString( params, " " );
            if ( !param.empty() )
            {
                __ADD_ARGS__( param );
            }
        }

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

        // 删除参数列表
        for ( auto arg : args )
        {
            __DELETE_ARRAY__( arg );
        }
        args.clear();

        deploydata->_process_id = FindProcessIdByName( deploydata );
        return true;
    }

    uint32 KFDeployAgentModule::FindProcessIdByName( KFDeployData* deploydata )
    {
        auto strprocessid = ExecuteShell( "ps -ef|grep '{}={}'|grep -v 'grep'|awk '{{print $2}}'", __STRING__( id ), deploydata->_app_id );
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
            return _invalid_string;
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
        updatevalues[ __STRING__( shutdown ) ] = __TO_STRING__( deploydata->_is_shutdown ? 1 : 0 );
        updatevalues[ __STRING__( process ) ] = __TO_STRING__( deploydata->_process_id );
        updatevalues[ __STRING__( time ) ] = __TO_STRING__( deploydata->_startup_time );

        MapString keyvalues;
        keyvalues[ __STRING__( appid ) ] = deploydata->_app_id;
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
        offile << __SPLIT_STRING__;
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
        deploydata->_process_id = KFUtility::SplitValue< uint32 >( strdata, __SPLIT_STRING__ );
        deploydata->_startup_time = KFUtility::SplitValue< uint64 >( strdata, __SPLIT_STRING__ );
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

            if ( appname != _globbing_string )
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

    void KFDeployAgentModule::SendLogMessage( const std::string& msg )
    {
        __LOG_INFO__( "{}", msg );

        if ( _deploy_server_id != 0u )
        {
            KFMsg::S2SDeployLogToServerAck ack;
            ack.set_content( msg );
            ack.set_agentid( KFGlobal::Instance()->_app_id->GetId() );
            _kf_tcp_client->SendNetMessage( _deploy_server_id, KFMsg::S2S_DEPLOY_LOG_TO_SERVER_ACK, &ack );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDeployAgentModule::HandleDeployCommandReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployCommandToAgentReq );

        auto pbdeploy = kfmsg.mutable_deploycommand();

        LogDeploy( "command=[{}:{}|{}:{}:{}:{}]!",
                   pbdeploy->command(), pbdeploy->value(), pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );

        // 判断是否agent的进程
        auto ok = IsAgentDeploy( pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );
        if ( !ok )
        {
            return;
        }

        if ( pbdeploy->command() == __STRING__( restart ) )
        {
            AddDeployTask( __STRING__( shutdown ), pbdeploy );
            AddDeployTask( __STRING__( wait ), pbdeploy );
            AddDeployTask( __STRING__( kill ), pbdeploy );
            AddDeployTask( __STRING__( startup ), pbdeploy );
        }
        else if ( pbdeploy->command() == __STRING__( version ) )
        {
            if ( pbdeploy->value() != _invalid_string )
            {
                AddDeployTask( __STRING__( shutdown ), pbdeploy );
                AddDeployTask( __STRING__( wget ), pbdeploy );
                AddDeployTask( __STRING__( kill ), pbdeploy );
                AddDeployTask( __STRING__( copy ), pbdeploy );
                AddDeployTask( __STRING__( startup ), pbdeploy );
            }
        }
        else if ( pbdeploy->command() == __STRING__( reloadconfig ) )
        {
            AddDeployTask( __STRING__( downfile ), pbdeploy );
            AddDeployTask( __STRING__( loadconfig ), pbdeploy );
        }
        else if ( pbdeploy->command() == __STRING__( reloadscript ) )
        {
            AddDeployTask( __STRING__( downfile ), pbdeploy );
            AddDeployTask( __STRING__( loadscript ), pbdeploy );
        }
        else if ( pbdeploy->command() == __STRING__( resource ) )
        {
            AddDeployTask( __STRING__( downresource ), pbdeploy );

            pbdeploy->set_value( _globbing_string );
            AddDeployTask( __STRING__( loadconfig ), pbdeploy );
            AddDeployTask( __STRING__( loadscript ), pbdeploy );
        }
        else if ( pbdeploy->command() == __STRING__( developreload ) )
        {
            AddDeployTask( __STRING__( developupdate ), pbdeploy );
            AddDeployTask( __STRING__( loadconfig ), pbdeploy );
        }
        else if ( pbdeploy->command() == __STRING__( reloadplugin ) )
        {
            AddDeployTask( __STRING__( downplugin ), pbdeploy );
            AddDeployTask( __STRING__( loadplugin ), pbdeploy );
        }
        else if ( pbdeploy->command() == __STRING__( cleantask ) )
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

            UpdateCommandToDatabase();
            LogDeploy( "task cleanup ok!" );
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

        if ( _kf_task == nullptr )
        {
            _kf_task = kftask;
            StartDeployTask();
        }
        else
        {
            _deploy_task.push_back( kftask );

            LogDeploy( "add [{}:{} | {}:{}:{}:{}] count[{}]!",
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
                LogDeploy( "[{}:{} | {}:{}:{}:{}] task finish!",
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
                else
                {
                    UpdateCommandToDatabase();
                    LogDeploy( "all task finish!" );
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
        if ( _kf_task->_command == __STRING__( kill ) )
        {
            ok = CheckShutDownServerTaskFinish();

        }
        else if ( _kf_task->_command == __STRING__( wait ) )
        {
            ok = CheckWaitTaskFinish();
        }
        else if ( _kf_task->_command == __STRING__( startup ) )
        {
            ok = CheckStartupServerTaskFinish();
        }
        else if ( _kf_task->_command == __STRING__( wget ) )
        {
            ok = CheckWgetVersionTaskFinish();
        }
        else if ( _kf_task->_command == __STRING__( downfile ) )
        {
            ok = CheckDownFileTaskFinish();
        }
        else if ( _kf_task->_command == __STRING__( downresource ) )
        {
            ok = CheckDownResourceTaskFinish();
        }
        else if ( _kf_task->_command == __STRING__( downplugin ) )
        {
            ok = CheckDownPluginTaskFinish();
        }

        return ok;
    }

    void KFDeployAgentModule::UpdateCommandToDatabase()
    {
        std::string command = "";
        if ( _kf_task != nullptr )
        {
            command = __FORMAT__( "{}:{} | {}:{}:{}:{}",
                                  _kf_task->_command, _kf_task->_value,
                                  _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
        }

        auto localip = _kf_ip_address->GetLocalIp();
        _deploy_driver->Execute( "update `agent` set `command`='{}' where `{}`='{}'", command, __STRING__( localip ), localip );
    }

    void KFDeployAgentModule::StartDeployTask()
    {
        _kf_task->_start_time = KFGlobal::Instance()->_game_time;

        try
        {
            UpdateCommandToDatabase();
            LogDeploy( "[{}:{} | {}:{}:{}:{}] task start!",
                       _kf_task->_command, _kf_task->_value,
                       _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );

            if ( _kf_task->_command == __STRING__( startup ) )
            {
                StartStartupServerTask();
            }
            else if ( _kf_task->_command == __STRING__( kill ) )
            {
                StartKillServerTask();
            }
            else if ( _kf_task->_command == __STRING__( shutdown ) )
            {
                StartShutDownServerTask();
            }
            else if ( _kf_task->_command == __STRING__( wait ) )
            {
                StartWaitTask();
            }
            else if ( _kf_task->_command == __STRING__( launch ) )
            {
                LoadTotalLaunchData();
            }
            else if ( _kf_task->_command == __STRING__( wget ) )
            {
                StartWgetVersionTask();
            }
            else if ( _kf_task->_command == __STRING__( copy ) )
            {
                StartCopyVersionTask();
            }
            else if ( _kf_task->_command == __STRING__( downfile ) )
            {
                StartDownFileTask();
            }
            else if ( _kf_task->_command == __STRING__( downresource ) )
            {
                StartWgetResourceTask();
            }
            else if ( _kf_task->_command == __STRING__( downplugin ) )
            {
                StartDownPluginTask();
            }
            else if ( _kf_task->_command == __STRING__( developupdate ) )
            {
                StartDevelopUpdateTask();
            }
            else
            {
                SendTaskToMaster();
            }
        }
        catch ( std::exception& exception )
        {
            LogDeploy( "start task exception={}!", exception.what() );
        }
        catch ( ... )
        {
            LogDeploy( "start task exception unknown!" );
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

                LogDeploy( "[{}:{}:{}] kill ok!", deploydata->_app_name, deploydata->_app_type, deploydata->_app_id );
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

    void KFDeployAgentModule::StartWaitTask()
    {
        auto waittime = KFUtility::ToValue< uint32 >( _kf_task->_value );
        if ( waittime < 20000u )
        {
            waittime = 20000u;
        }

        _kf_task->_start_time = KFGlobal::Instance()->_game_time + waittime;
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

    bool KFDeployAgentModule::CheckWaitTaskFinish()
    {
        return KFGlobal::Instance()->_game_time > _kf_task->_start_time;
    }

    void KFDeployAgentModule::StartStartupServerTask()
    {
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
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_CLIENT_REQ, &req );
    }

    void KFDeployAgentModule::StartWgetVersionTask()
    {
        // 查询版本路径
        auto queryurl = _deploy_driver->QueryString( "select `version_url` from `version` where `version_name`='{}';", _kf_task->_value );
        if ( !queryurl->IsOk() || queryurl->_value.empty() )
        {
            return LogDeploy( "can't find [{}] url!", _kf_task->_value );
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 执行下载命令
        ExecuteShell( "wget -c -P ./version/ {}", queryurl->_value );
#endif
    }

    void KFDeployAgentModule::StartCopyVersionTask()
    {
#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
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

        ExecuteShell( "rm wget-log*" );
        ExecuteShell( "rm -rf ./version/*" );
        LogDeploy( "[{}] update version ok!", _kf_task->_app_name );
#endif

        // 更新版本号
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploydata = iter.second;

            auto isserver = deploydata->IsAppServer( _kf_task->_app_name, _kf_task->_app_type, _kf_task->_app_id, _kf_task->_zone_id );
            if ( isserver )
            {
                MapString updatevalues;
                updatevalues[ __STRING__( version ) ] = _kf_task->_value;

                MapString keyvalues;
                keyvalues[ __STRING__( appid ) ] = deploydata->_app_id;
                keyvalues[ __STRING__( service ) ] = deploydata->_service_type;
                _deploy_driver->Update( _deploy_table_name, keyvalues, updatevalues );
            }
        }
    }

    bool KFDeployAgentModule::CheckWgetVersionTaskFinish()
    {
        auto querymd5 = _deploy_driver->QueryString( "select `version_md5` from `version` where `version_name`='{}';", _kf_task->_value );
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
#endif
        return true;
    }

    void KFDeployAgentModule::StartDownFileTask()
    {
        // 查询版本路径
        auto queryurl = _deploy_driver->QueryString( "select `file_url` from `file` where `file_name`='{}';", _kf_task->_value );
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
        auto querymap = _deploy_driver->QueryMap( "select * from `file` where `file_name`='{}';", _kf_task->_value );
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

        LogDeploy( "update file [{}] ok!", _kf_task->_value );
#endif

        return true;
    }

    void KFDeployAgentModule::StartWgetResourceTask()
    {
        // 查询版本路径
        auto queryurl = _deploy_driver->QueryString( "select `resource_url` from `resource` where `resource_name`='{}';", _kf_task->_value );
        if ( !queryurl->IsOk() || queryurl->_value.empty() )
        {
            return LogDeploy( "can't find [{}] url!", _kf_task->_value );
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 执行下载命令
        ExecuteShell( "wget -c -P ./version/ {}", queryurl->_value );
#endif
    }

    bool KFDeployAgentModule::CheckDownResourceTaskFinish()
    {
        auto querymd5 = _deploy_driver->QueryString( "select `resource_md5` from `resource` where `resource_name`='{}';", _kf_task->_value );
        if ( !querymd5->IsOk() || querymd5->_value.empty() )
        {
            return true;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现

#else
        // 执行下载命令
        auto md5 = ExecuteShell( "md5sum ./version/{} | awk '{{print $1}}'", _kf_task->_value );
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
            ExecuteShell( "cp -rf ./version/conf_output/* {}", deploypath );
            ExecuteShell( "echo {} > {}/resource.txt", _kf_task->_value, deploypath );
        }

        ExecuteShell( "rm wget-log*" );
        ExecuteShell( "rm -rf ./version/*" );
        LogDeploy( "[{}] update resource ok!", _kf_task->_app_name );
#endif

        return true;
    }

    void KFDeployAgentModule::StartDownPluginTask()
    {
        auto value = _kf_task->_value;
        KFUtility::SplitString( value, "=" );
        auto filename = KFUtility::SplitString( value, "=" );

        // 查询版本路径
        auto queryurl = _deploy_driver->QueryString( "select `file_url` from `plugin` where `file_name`='{}';", filename );
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

    bool KFDeployAgentModule::CheckDownPluginTaskFinish()
    {
        auto value = _kf_task->_value;
        KFUtility::SplitString( value, "=" );
        auto filename = KFUtility::SplitString( value, "=" );

        auto querymap = _deploy_driver->QueryMap( "select * from `plugin` where `file_name`='{}';", filename );
        if ( !querymap->IsOk() || querymap->_value.empty() )
        {
            LogDeploy( "plugin data [{}] empty!", filename );
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
        auto md5 = ExecuteShell( "md5sum ./version/{} | awk '{{print $1}}'", filename );
        if ( md5 != querymd5 )
        {
            StartDownPluginTask();
            return false;
        }

        // 把文件拷贝过去
        std::set< std::string > deploypathlist;
        FindAppDeployPath( _kf_task->_app_name, deploypathlist );
        for ( auto& deploypath : deploypathlist )
        {
            ExecuteShell( "mkdir -p {}", deploypath );
            ExecuteShell( "mv -f ./version/{} {}/bin/{}.new", filename, deploypath, filename );
        }

        LogDeploy( "down plugin [{}] ok!", filename );
#endif

        return true;
    }

    void KFDeployAgentModule::StartDevelopUpdateTask()
    {
#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        ExecuteShell( "sh {}", _kf_task->_value );
#endif
    }
}