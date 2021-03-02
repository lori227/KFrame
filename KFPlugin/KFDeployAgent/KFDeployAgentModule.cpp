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
        auto timer_id = 0;
        __LOOP_TIMER_1__( ++timer_id, 20000, 100, &KFDeployAgentModule::OnTimerStartupProcess );
        __LOOP_TIMER_1__( ++timer_id, 1000, 0, &KFDeployAgentModule::OnTimerCheckTaskFinish );
        __LOOP_TIMER_1__( ++timer_id, 5000, 1000, &KFDeployAgentModule::OnTimerQueryAgentData );
        __LOOP_TIMER_1__( ++timer_id, 30000, 100, &KFDeployAgentModule::OnTimerCheckHeartbeat );

        __REGISTER_TCP_SERVER_DISCOVER__( &KFDeployAgentModule::OnServerDiscoverClient );
        __REGISTER_TCP_SERVER_LOST__( &KFDeployAgentModule::OnServerLostClient );
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFDeployAgentModule::OnClientConnectServer );
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFDeployAgentModule::OnClientLostServer );
        ////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &KFDeployAgentModule::HandleDeployCommandReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DEPLOY_HEARTBEAT_TO_AGENT_REQ, &KFDeployAgentModule::HandleClientHeartbeatReq );
    }

    void KFDeployAgentModule::ShutDown()
    {
        __UN_TIMER_0__();

        __UN_TCP_SERVER_DISCOVER__();
        __UN_TCP_SERVER_LOST__();
        __UN_TCP_CLIENT_CONNECTION__();
        __UN_TCP_CLIENT_SHUTDOWN__();
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
        auto local_ip = KFGlobal::Instance()->_local_ip;
        auto query_result = _deploy_driver->QueryMap( "select * from `agent` where `{}`='{}'", __STRING__( localip ), local_ip );
        if ( query_result->_value.empty() )
        {
            return __LOG_ERROR__( "query agent=[{}] data failed", local_ip );
        }

        auto str_app_id = query_result->_value[ __STRING__( strappid ) ];
        if ( str_app_id.empty() )
        {
            return __LOG_ERROR__( "agent=[{}] app_id is empty", local_ip );
        }

        auto global = KFGlobal::Instance();
        global->_app_id->FromString( str_app_id );

        auto str_service = query_result->_value[ __STRING__( service ) ];
        if ( str_service.empty() )
        {
            return __LOG_ERROR__( "agent=[{}] service is empty", local_ip );
        }

        global->_channel = KFUtility::SplitValue<uint32>( str_service, "." );
        global->_service = KFUtility::SplitValue<uint32>( str_service, "." );

        // 部署表
        _deploy_table_name = __FORMAT__( "{}_{}_deploy", global->_channel, global->_service );
        __LOG_INFO__( "agent table name = [{}]", _deploy_table_name );

        // deploy server
        _str_deploy_server_id = query_result->_value[ "serverid" ];
        _deploy_server_ip = query_result->_value[ "serverip" ];
        _deploy_server_port = __TO_UINT32__( query_result->_value[ "serverport" ] );

        // 关闭定时器
        __UN_TIMER_1__( object_id );

        // 启动连接deployserver
        StartConnectDeployServer();

        // 读取进程启动配置
        LoadTotalLaunchData();
    }

    void KFDeployAgentModule::StartConnectDeployServer()
    {
        if ( _str_deploy_server_id.empty() || _deploy_server_ip.empty() || _deploy_server_port == 0u )
        {
            return;
        }

        auto app_id = KFAppId::ToUInt64( _str_deploy_server_id );
        _kf_tcp_client->StartClient( __STRING__( deploy ), __STRING__( server ), app_id, _deploy_server_ip, _deploy_server_port );
    }

    void KFDeployAgentModule::LoadTotalLaunchData()
    {
        if ( _deploy_table_name.empty() )
        {
            return __LOG_ERROR__( "deploy table name is empty" );
        }

        try
        {
            StringMap keys;
            keys[ __STRING__( agentid ) ] = KFGlobal::Instance()->_app_id->ToString();
            auto query_result = _deploy_driver->Select( _deploy_table_name, keys );
            if ( !query_result->_value.empty() )
            {
                _deploy_list.Clear();
                for ( auto& values : query_result->_value )
                {
                    auto deploy_data = __MAKE_SHARED__( KFDeployData );
                    deploy_data->CopyFrom( values );
                    _deploy_list.Insert( deploy_data->_app_id, deploy_data );
                    __LOG_INFO__( "add server[{}:{}:{}] deploy", deploy_data->_app_name, deploy_data->_app_type, deploy_data->_app_id );
                }

                BindServerProcess();
            }
        }
        catch ( ... )
        {
            __LOG_ERROR__( "load launch exception" );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFDeployAgentModule::OnClientConnectServer )
    {
        // 连接成功
        if ( net_data->_name == __STRING__( deploy ) && net_data->_type == __STRING__( server ) )
        {
            _deploy_server_id = net_data->_id;
            auto global = KFGlobal::Instance();

            // 把自己注册到Services
            KFMsg::S2SRegisterAgentToServerReq req;
            req.set_agentid( global->_app_id->ToString() );
            req.set_name( global->_app_name );
            req.set_type( global->_app_type );
            req.set_localip( global->_local_ip );
            req.set_port( global->_listen_port );
            req.set_service( __FORMAT__( "{}.{}", global->_channel, global->_service ) );
            _kf_tcp_client->SendNetMessage( net_data->_id, KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &req );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFDeployAgentModule::OnClientLostServer )
    {
        // 断开server
        if ( net_data->_name == __STRING__( deploy ) && net_data->_type == __STRING__( server ) )
        {
            _kf_tcp_client->StartClient( __STRING__( deploy ), __STRING__( server ), _deploy_server_id, _deploy_server_ip, _deploy_server_port );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDeployAgentModule::HandleClientHeartbeatReq, KFMsg::S2SDeployHeartbeatToAgentReq )
    {
        auto app_id = KFAppId::ToString( kfmsg->id() );
        auto deploy_data = _deploy_list.Find( app_id );
        if ( deploy_data == nullptr || deploy_data->_heartbeat == 0u )
        {
            return;
        }

        // 5分钟判断超时
        deploy_data->_heartbeat_timeout = KFGlobal::Instance()->_game_time + deploy_data->_heartbeat * KFTimeEnum::OneSecondMicSecond;
    }

    __KF_NET_EVENT_FUNCTION__( KFDeployAgentModule::OnServerDiscoverClient )
    {
        auto deploy_data = _deploy_list.Find( net_data->_str_id );
        if ( deploy_data == nullptr )
        {
            return;
        }

        deploy_data->_is_connected = true;
    }

    __KF_NET_EVENT_FUNCTION__( KFDeployAgentModule::OnServerLostClient )
    {
        auto deploy_data = _deploy_list.Find( net_data->_str_id );
        if ( deploy_data == nullptr )
        {
            return;
        }

        deploy_data->_is_connected = false;
    }

    __KF_TIMER_FUNCTION__( KFDeployAgentModule::OnTimerCheckHeartbeat )
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploy_data = iter.second;
            if ( deploy_data->_heartbeat == 0 ||
                    deploy_data->_process_id == 0 ||
                    deploy_data->_is_shutdown ||
                    !deploy_data->_is_startup ||
                    !deploy_data->_is_connected ||
                    deploy_data->_heartbeat_timeout > KFGlobal::Instance()->_game_time )
            {
                continue;
            }

            // 超时了, 认为client卡死了
            KillServerProcess( deploy_data->_process_id );
            __LOG_INFO__( "app=[{}.{}] id=[{}] heartbeat timeout", deploy_data->_app_name, deploy_data->_app_type, deploy_data->_app_id );
        }
    }

    __KF_TIMER_FUNCTION__( KFDeployAgentModule::OnTimerStartupProcess )
    {
        try
        {
            for ( auto& iter : _deploy_list._objects )
            {
                auto deploy_data = iter.second;
                StartupServerProcess( deploy_data );
            }
        }
        catch ( std::exception& exception )
        {
            __LOG_ERROR__( "startup exception={}", exception.what() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "startup exception unknown" );
        }
    }

    void KFDeployAgentModule::StartupServerProcess( std::shared_ptr<KFDeployData> deploy_data )
    {
        CheckServerProcess( deploy_data );
        if ( !deploy_data->_is_startup || deploy_data->_is_shutdown || deploy_data->_process_id != _invalid_int )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        auto ok = StartupWinProcess( deploy_data );
#else
        auto ok = StartupLinuxProcess( deploy_data );
#endif
        if ( ok )
        {
            deploy_data->_startup_time = KFGlobal::Instance()->_real_time;
            UpdateDeployToDatabase( deploy_data );

            // 保存到文件中
            SaveProcessToFile( deploy_data );

            LogDeploy( "[{}:{}:{}] startup ok", deploy_data->_app_name, deploy_data->_app_type, deploy_data->_app_id );
        }
    }

    void KFDeployAgentModule::CheckServerProcess( std::shared_ptr<KFDeployData> deploy_data )
    {
        if ( deploy_data->_process_id == _invalid_int )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        CheckWinProcess( deploy_data );
#else
        CheckLinuxProcess( deploy_data );
#endif
        if ( deploy_data->_process_id != _invalid_int )
        {
            return;
        }

        // 更新状态
        UpdateDeployToDatabase( deploy_data );

        // 保存到文件中
        SaveProcessToFile( deploy_data );
    }

    void KFDeployAgentModule::BindServerProcess()
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploy_data = iter.second;
            if ( deploy_data->_process_id == _invalid_int )
            {
                ReadProcessFromFile( deploy_data );
                if ( deploy_data->_process_id != _invalid_int )
                {
                    UpdateDeployToDatabase( deploy_data );
                }
            }
        }
    }

    void KFDeployAgentModule::KillServerProcess( uint32 process_id )
    {
        if ( process_id == 0 )
        {
            return;
        }

        try
        {
#if __KF_SYSTEM__ == __KF_WIN__
            KillWinProcess( process_id );
#else
            KillLinuxProcess( process_id );
#endif
        }
        catch ( std::exception& exception )
        {
            __LOG_ERROR__( "kill exception={}", exception.what() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "kill exception unknown" );
        }
    }

#if __KF_SYSTEM__ == __KF_WIN__
    bool KFDeployAgentModule::StartupWinProcess( std::shared_ptr<KFDeployData> deploy_data )
    {
        auto global = KFGlobal::Instance();

        // 启动进程
        STARTUPINFO startup_info;
        memset( &startup_info, 0, sizeof( STARTUPINFO ) );
        startup_info.cb = sizeof( STARTUPINFO );
        startup_info.wShowWindow = SW_SHOW;
        startup_info.dwFlags = STARTF_USESHOWWINDOW;

        uint32 create_flag = CREATE_NO_WINDOW;
        auto apppath = deploy_data->GetAppPath();
        auto startupfile = deploy_data->GetStartupFile( deploy_data->_is_debug );
        auto param = __FORMAT__( " {}={}.{} {}={} {}={} {}={}|{}|{}",
                                 __STRING__( app ), deploy_data->_app_name, deploy_data->_app_type,
                                 __STRING__( id ), deploy_data->_app_id,
                                 __STRING__( service ), deploy_data->_service_type,
                                 __STRING__( net ), deploy_data->_net_type,
                                 __STRING__( agent ), global->_app_id->ToString(), global->_local_ip, global->_listen_port );

        // 启动进程
        PROCESS_INFORMATION process_info;
        BOOL result = CreateProcess( startupfile.c_str(), const_cast< char* >( param.c_str() ), NULL, NULL, FALSE,
                                     create_flag, NULL, apppath.c_str(), &startup_info, &process_info );

        deploy_data->_process_id = process_info.dwProcessId;
        if ( deploy_data->_process_id != _invalid_int )
        {
            __LOG_DEBUG__( "startup [ {}:{}:{} ] ok! process={}",
                           deploy_data->_app_name, deploy_data->_app_type, deploy_data->_app_id, deploy_data->_process_id );
        }
        else
        {
            __LOG_ERROR__( "startup [ {}:{}:{} ] failed",
                           deploy_data->_app_name, deploy_data->_app_type, deploy_data->_app_id );
        }

        return deploy_data->_process_id != _invalid_int;
    }

    void KFDeployAgentModule::CheckWinProcess( std::shared_ptr<KFDeployData> deploy_data )
    {
        DWORD flag = PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION;
        HANDLE handle = OpenProcess( flag, FALSE, deploy_data->_process_id );
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

        deploy_data->_process_id = 0;
    }

    void KFDeployAgentModule::KillWinProcess( uint32 process_id )
    {
        DWORD flag = PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION;
        HANDLE handle = OpenProcess( flag, FALSE, process_id );
        if ( handle == nullptr )
        {
            return;
        }

        TerminateProcess( handle, 0 );
    }

#else
    bool KFDeployAgentModule::StartupLinuxProcess( std::shared_ptr<KFDeployData> deploy_data )
    {
        std::vector<char*> args;
        auto global = KFGlobal::Instance();

#define __ADD_ARGS__( param )\
    {\
        auto size = param.size();\
        auto arg = new char[ size + 1 ]; \
        memcpy( arg, param.c_str(), size );\
        arg[ size ] = 0;\
        args.push_back( arg );\
    }

        auto startup_file = deploy_data->GetStartupFile( deploy_data->_is_debug );
        __ADD_ARGS__( startup_file );

        auto str_app_name = __FORMAT__( "{}={}.{}", __STRING__( app ), deploy_data->_app_name, deploy_data->_app_type );
        __ADD_ARGS__( str_app_name );

        auto str_app_id = __FORMAT__( "{}={}", __STRING__( id ), deploy_data->_app_id );
        __ADD_ARGS__( str_app_id );

        auto str_service = __FORMAT__( "{}={}", __STRING__( service ), deploy_data->_service_type );
        __ADD_ARGS__( str_service );

        auto str_net = __FORMAT__( "{}={}", __STRING__( net ), deploy_data->_net_type );
        __ADD_ARGS__( str_net );

        auto str_agent = __FORMAT__( "{}={}|{}|{}", __STRING__( agent ), global->_app_id->ToString(), global->_local_ip, global->_listen_port );
        __ADD_ARGS__( str_agent );

        auto params = deploy_data->_params;
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
            auto app_path = deploy_data->GetAppPath();
            if ( chdir( app_path.c_str() ) != 0 )
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

        deploy_data->_process_id = FindProcessIdByName( deploy_data );
        return true;
    }

    uint32 KFDeployAgentModule::FindProcessIdByName( std::shared_ptr<KFDeployData> deploy_data )
    {
        auto process_id = ExecuteShell( "ps -ef|grep '{}={}'|grep -v 'grep'|awk '{{print $2}}'", __STRING__( id ), deploy_data->_app_id );
        return __TO_UINT32__( process_id );
    }

    void KFDeployAgentModule::CheckLinuxProcess( std::shared_ptr<KFDeployData> deploy_data )
    {
        if ( kill( deploy_data->_process_id, 0 ) != 0 )
        {
            deploy_data->_process_id = FindProcessIdByName( deploy_data );
        }
    }

    void KFDeployAgentModule::KillLinuxProcess( uint32 process_id )
    {
        ExecuteShell( "kill -s 9 {}", process_id );
    }

    std::string KFDeployAgentModule::ExecuteShellCommand( const std::string& command )
    {
        FILE* fp = popen( command.c_str(), "r" );
        if ( fp == nullptr )
        {
            __LOG_ERROR__( "[{}] open failed", command );
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

    void KFDeployAgentModule::UpdateDeployToDatabase( std::shared_ptr<KFDeployData> deploy_data )
    {
        StringMap values;
        values[ __STRING__( shutdown ) ] = __TO_STRING__( deploy_data->_is_shutdown ? 1 : 0 );
        values[ __STRING__( process ) ] = __TO_STRING__( deploy_data->_process_id );
        values[ __STRING__( time ) ] = __TO_STRING__( deploy_data->_startup_time );

        StringMap keys;
        keys[ __STRING__( appid ) ] = deploy_data->_app_id;
        _deploy_driver->Update( _deploy_table_name, keys, values );
    }

    std::string KFDeployAgentModule::FormatPidFileName( std::shared_ptr<KFDeployData> deploy_data )
    {
        // 文件名字
        return __FORMAT__( "{}/{}-{}-{}", _pid_path, deploy_data->_app_name, deploy_data->_app_type, deploy_data->_app_id );
    }

    void KFDeployAgentModule::SaveProcessToFile( std::shared_ptr<KFDeployData> deploy_data )
    {
        auto file = FormatPidFileName( deploy_data );

        auto pid_path = _pid_path + "/";
        __MKDIR__( pid_path );

        std::ofstream write_file( file );
        if ( !write_file )
        {
            return;
        }

        write_file << deploy_data->_process_id;
        write_file << __OR_STRING__;
        write_file << deploy_data->_startup_time;

        write_file.flush();
        write_file.close();
    }

    void KFDeployAgentModule::ReadProcessFromFile( std::shared_ptr<KFDeployData> deploy_data )
    {
        auto file = FormatPidFileName( deploy_data );

        std::ifstream read_file( file );
        if ( !read_file )
        {
            return;
        }

        std::ostringstream os;
        read_file >> os.rdbuf();
        read_file.close();

        std::string data = os.str();
        deploy_data->_process_id = KFUtility::SplitValue<uint32>( data, __OR_STRING__ );
        deploy_data->_startup_time = KFUtility::SplitValue<uint64>( data, __OR_STRING__ );
    }

    bool KFDeployAgentModule::IsAgentDeploy( const std::string& command, const std::string& app_name, const std::string& app_type, const std::string& app_id, uint32 zone_id )
    {
        if ( command == __STRING__( launch ) )
        {
            return true;
        }

        for ( auto& iter : _deploy_list._objects )
        {
            auto deploy_data = iter.second;

            auto is_server = deploy_data->IsAppServer( app_name, app_type, app_id, zone_id );
            if ( is_server )
            {
                return true;
            }
        }

        return false;
    }

    void KFDeployAgentModule::FindAppDeployPath( const std::string& app_name, uint32 zone_id, StringSet& deploy_path_list )
    {
        deploy_path_list.clear();

        for ( auto& iter : _deploy_list._objects )
        {
            auto deploy_data = iter.second;
            if ( app_name != _globbing_string )
            {
                if ( app_name == deploy_data->_app_name )
                {
                    if ( app_name != __STRING__( zone ) || zone_id == _invalid_int || deploy_data->_zone_id == zone_id )
                    {
                        deploy_path_list.insert( deploy_data->_deploy_path );
                    }
                }
            }
            else
            {
                deploy_path_list.insert( deploy_data->_deploy_path );
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

    __KF_MESSAGE_FUNCTION__( KFDeployAgentModule::HandleDeployCommandReq, KFMsg::S2SDeployCommandToAgentReq )
    {
        auto deploy_command = &kfmsg->deploycommand();
        LogDeploy( "command=[{}:{}|{}:{}:{}:{}]",
                   deploy_command->command(), deploy_command->value(), deploy_command->appname(), deploy_command->apptype(), deploy_command->appid(), deploy_command->zoneid() );

        // 判断是否agent的进程
        auto ok = IsAgentDeploy( deploy_command->command(), deploy_command->appname(), deploy_command->apptype(), deploy_command->appid(), deploy_command->zoneid() );
        if ( !ok )
        {
            return;
        }

        if ( deploy_command->command() == __STRING__( restart ) )
        {
            AddDeployTask( __STRING__( shutdown ), deploy_command );
            AddDeployTask( __STRING__( wait ), deploy_command );
            AddDeployTask( __STRING__( kill ), deploy_command );
            AddDeployTask( __STRING__( startup ), deploy_command );
        }
        else if ( deploy_command->command() == __STRING__( version ) )
        {
            if ( deploy_command->value() != _invalid_string )
            {
                AddDeployTask( __STRING__( shutdown ), deploy_command );
                AddDeployTask( __STRING__( wget ), deploy_command );
                AddDeployTask( __STRING__( kill ), deploy_command );
                AddDeployTask( __STRING__( copy ), deploy_command );
                AddDeployTask( __STRING__( startup ), deploy_command );
            }
        }
        else if ( deploy_command->command() == __STRING__( reloadconfig ) )
        {
            AddDeployTask( __STRING__( downfile ), deploy_command );
            AddDeployTask( __STRING__( loadconfig ), deploy_command );
        }
        else if ( deploy_command->command() == __STRING__( reloadscript ) )
        {
            AddDeployTask( __STRING__( downfile ), deploy_command );
            AddDeployTask( __STRING__( loadscript ), deploy_command );
        }
        else if ( deploy_command->command() == __STRING__( resource ) )
        {
            AddDeployTask( __STRING__( downresource ), deploy_command );

            AddDeployTask( __STRING__( loadconfig ), deploy_command );
            AddDeployTask( __STRING__( loadscript ), deploy_command );
        }
        else if ( deploy_command->command() == __STRING__( developreload ) )
        {
            AddDeployTask( __STRING__( developupdate ), deploy_command );
            AddDeployTask( __STRING__( loadconfig ), deploy_command );
        }
        else if ( deploy_command->command() == __STRING__( reloadplugin ) )
        {
            AddDeployTask( __STRING__( downplugin ), deploy_command );
            AddDeployTask( __STRING__( loadplugin ), deploy_command );
        }
        else if ( deploy_command->command() == __STRING__( cleantask ) )
        {
            _deploy_task = nullptr;
            _deploy_task_list.clear();

            UpdateCommandToDatabase();
            LogDeploy( "task cleanup ok" );
        }
        else
        {
            AddDeployTask( deploy_command->command(), deploy_command );
        }
    }

    void KFDeployAgentModule::AddDeployTask( const std::string& command, const KFMsg::PBDeployCommand* deploy_command )
    {
        auto deploy_task = __MAKE_SHARED__( KFDeployTask );
        deploy_task->_command = command;
        deploy_task->_value = deploy_command->value();
        deploy_task->_app_name = deploy_command->appname();
        deploy_task->_app_type = deploy_command->apptype();
        deploy_task->_app_id = deploy_command->appid();
        deploy_task->_zone_id = deploy_command->zoneid();

        if ( _deploy_task == nullptr )
        {
            _deploy_task = deploy_task;
            StartDeployTask();
        }
        else
        {
            _deploy_task_list.push_back( deploy_task );

            LogDeploy( "add [{}:{} | {}:{}:{}:{}] count[{}]",
                       deploy_task->_command, deploy_task->_value,
                       deploy_task->_app_name, deploy_task->_app_type,
                       deploy_task->_app_id, deploy_task->_zone_id, _deploy_task_list.size() );
        }
    }

    __KF_TIMER_FUNCTION__( KFDeployAgentModule::OnTimerCheckTaskFinish )
    {
        try
        {
            auto ok = CheckTaskFinish();
            if ( ok )
            {
                LogDeploy( "[{}:{} | {}:{}:{}:{}] task finish",
                           _deploy_task->_command, _deploy_task->_value,
                           _deploy_task->_app_name, _deploy_task->_app_type, _deploy_task->_app_id, _deploy_task->_zone_id );

                _deploy_task = nullptr;

                if ( !_deploy_task_list.empty() )
                {
                    _deploy_task = _deploy_task_list.front();
                    _deploy_task_list.pop_front();

                    StartDeployTask();
                }
                else
                {
                    UpdateCommandToDatabase();
                    LogDeploy( "all task finish" );
                }
            }
        }
        catch ( std::exception& exception )
        {
            __LOG_ERROR__( "check finish exception={}", exception.what() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "check finish exception unknown" );
        }
    }

    bool KFDeployAgentModule::CheckTaskFinish()
    {
        if ( _deploy_task == nullptr )
        {
            return false;
        }

        auto ok = true;
        if ( _deploy_task->_command == __STRING__( kill ) )
        {
            ok = CheckShutDownServerTaskFinish();

        }
        else if ( _deploy_task->_command == __STRING__( wait ) )
        {
            ok = CheckWaitTaskFinish();
        }
        else if ( _deploy_task->_command == __STRING__( startup ) )
        {
            ok = CheckStartupServerTaskFinish();
        }
        else if ( _deploy_task->_command == __STRING__( wget ) )
        {
            ok = CheckWgetVersionTaskFinish();
        }
        else if ( _deploy_task->_command == __STRING__( downfile ) )
        {
            ok = CheckDownFileTaskFinish();
        }
        else if ( _deploy_task->_command == __STRING__( downresource ) )
        {
            ok = CheckDownResourceTaskFinish();
        }
        else if ( _deploy_task->_command == __STRING__( downplugin ) )
        {
            ok = CheckDownPluginTaskFinish();
        }

        return ok;
    }

    void KFDeployAgentModule::UpdateCommandToDatabase()
    {
        std::string command = "";
        if ( _deploy_task != nullptr )
        {
            command = __FORMAT__( "{}:{} | {}:{}:{}:{}",
                                  _deploy_task->_command, _deploy_task->_value,
                                  _deploy_task->_app_name, _deploy_task->_app_type, _deploy_task->_app_id, _deploy_task->_zone_id );
        }

        _deploy_driver->Execute( "update `agent` set `command`='{}' where `{}`='{}'", command, __STRING__( localip ), KFGlobal::Instance()->_local_ip );
    }

    void KFDeployAgentModule::StartDeployTask()
    {
        _deploy_task->_start_time = KFGlobal::Instance()->_game_time;

        try
        {
            UpdateCommandToDatabase();
            LogDeploy( "[{}:{} | {}:{}:{}:{}] task start",
                       _deploy_task->_command, _deploy_task->_value,
                       _deploy_task->_app_name, _deploy_task->_app_type, _deploy_task->_app_id, _deploy_task->_zone_id );

            if ( _deploy_task->_command == __STRING__( startup ) )
            {
                StartStartupServerTask();
            }
            else if ( _deploy_task->_command == __STRING__( kill ) )
            {
                StartKillServerTask();
            }
            else if ( _deploy_task->_command == __STRING__( shutdown ) )
            {
                StartShutDownServerTask();
            }
            else if ( _deploy_task->_command == __STRING__( wait ) )
            {
                StartWaitTask();
            }
            else if ( _deploy_task->_command == __STRING__( launch ) )
            {
                LoadTotalLaunchData();
            }
            else if ( _deploy_task->_command == __STRING__( wget ) )
            {
                StartWgetVersionTask();
            }
            else if ( _deploy_task->_command == __STRING__( copy ) )
            {
                StartCopyVersionTask();
            }
            else if ( _deploy_task->_command == __STRING__( downfile ) )
            {
                StartDownFileTask();
            }
            else if ( _deploy_task->_command == __STRING__( downresource ) )
            {
                StartWgetResourceTask();
            }
            else if ( _deploy_task->_command == __STRING__( downplugin ) )
            {
                StartDownPluginTask();
            }
            else if ( _deploy_task->_command == __STRING__( developupdate ) )
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
            LogDeploy( "start task exception={}", exception.what() );
        }
        catch ( ... )
        {
            LogDeploy( "start task exception unknown" );
        }
    }

    void KFDeployAgentModule::StartKillServerTask()
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploy_data = iter.second;

            auto is_server = deploy_data->IsAppServer( _deploy_task->_app_name, _deploy_task->_app_type, _deploy_task->_app_id, _deploy_task->_zone_id );
            if ( is_server )
            {
                deploy_data->_is_shutdown = true;
                KillServerProcess( deploy_data->_process_id );

                UpdateDeployToDatabase( deploy_data );

                LogDeploy( "[{}:{}:{}] kill ok", deploy_data->_app_name, deploy_data->_app_type, deploy_data->_app_id );
            }
        }
    }

    void KFDeployAgentModule::StartShutDownServerTask()
    {
        _deploy_task->_start_time += __TO_UINT32__( _deploy_task->_value );

        for ( auto& iter : _deploy_list._objects )
        {
            auto deploy_data = iter.second;

            auto is_server = deploy_data->IsAppServer( _deploy_task->_app_name, _deploy_task->_app_type, _deploy_task->_app_id, _deploy_task->_zone_id );
            if ( is_server )
            {
                deploy_data->_is_shutdown = true;
                UpdateDeployToDatabase( deploy_data );
            }
        }

        SendTaskToMaster();
    }

    void KFDeployAgentModule::StartWaitTask()
    {
        auto wait_time = __TO_UINT32__( _deploy_task->_value );
        if ( wait_time < 20000u )
        {
            wait_time = 20000u;
        }

        _deploy_task->_start_time = KFGlobal::Instance()->_game_time + wait_time;
    }

    bool KFDeployAgentModule::CheckShutDownServerTaskFinish()
    {
        // 指定的server都关闭了
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploy_data = iter.second;

            auto is_server = deploy_data->IsAppServer( _deploy_task->_app_name, _deploy_task->_app_type, _deploy_task->_app_id, _deploy_task->_zone_id );
            if ( is_server )
            {
                if ( deploy_data->_process_id != _invalid_int )
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool KFDeployAgentModule::CheckWaitTaskFinish()
    {
        return KFGlobal::Instance()->_game_time > _deploy_task->_start_time;
    }

    void KFDeployAgentModule::StartStartupServerTask()
    {
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploy_data = iter.second;

            auto is_server = deploy_data->IsAppServer( _deploy_task->_app_name, _deploy_task->_app_type, _deploy_task->_app_id, _deploy_task->_zone_id );
            if ( is_server )
            {
                deploy_data->_is_shutdown = false;
                UpdateDeployToDatabase( deploy_data );
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
        auto deploy_command = req.mutable_deploycommand();

        deploy_command->set_command( _deploy_task->_command );
        deploy_command->set_value( _deploy_task->_value );
        deploy_command->set_appname( _deploy_task->_app_name );
        deploy_command->set_apptype( _deploy_task->_app_type );
        deploy_command->set_appid( _deploy_task->_app_id );
        deploy_command->set_zoneid( _deploy_task->_zone_id );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_CLIENT_REQ, &req );
    }

    void KFDeployAgentModule::StartWgetVersionTask()
    {
        // 查询版本路径
        auto query_url = _deploy_driver->QueryString( "select `version_url` from `version` where `version_name`='{}';", _deploy_task->_value );
        if ( !query_url->IsOk() || query_url->_value.empty() )
        {
            return LogDeploy( "can't find [{}] url", _deploy_task->_value );
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 执行下载命令
        ExecuteShell( "wget -c -P ./version/ {}", query_url->_value );
#endif
    }

    void KFDeployAgentModule::StartCopyVersionTask()
    {
#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 解压
        ExecuteShell( "rm -rf ./version/conf_output/" );
        ExecuteShell( "tar -zxf ./version/{} -C ./version/", _deploy_task->_value );

        // 把文件拷贝过去
        StringSet deploy_path_list;
        FindAppDeployPath( _deploy_task->_app_name, _deploy_task->_zone_id, deploy_path_list );
        for ( auto& deploy_path : deploy_path_list )
        {
            ExecuteShell( "mkdir -p {}", deploy_path );
            ExecuteShell( "rm {}/* -rf", deploy_path );
            ExecuteShell( "cp -rf ./version/conf_output/* {}", deploy_path );
            ExecuteShell( "chmod 777 -R {}", deploy_path );
            ExecuteShell( "echo {} > {}/package.txt", _deploy_task->_value, deploy_path );
        }

        ExecuteShell( "rm wget-log*" );
        ExecuteShell( "rm -rf ./version/*" );
        LogDeploy( "[{}] update version ok", _deploy_task->_app_name );
#endif

        // 更新版本号
        for ( auto& iter : _deploy_list._objects )
        {
            auto deploy_data = iter.second;

            auto is_server = deploy_data->IsAppServer( _deploy_task->_app_name, _deploy_task->_app_type, _deploy_task->_app_id, _deploy_task->_zone_id );
            if ( is_server )
            {
                StringMap values;
                values[ __STRING__( version ) ] = _deploy_task->_value;

                StringMap keys;
                keys[ __STRING__( appid ) ] = deploy_data->_app_id;
                keys[ __STRING__( service ) ] = deploy_data->_service_type;
                _deploy_driver->Update( _deploy_table_name, keys, values );
            }
        }
    }

    bool KFDeployAgentModule::CheckWgetVersionTaskFinish()
    {
        auto query_md5 = _deploy_driver->QueryString( "select `version_md5` from `version` where `version_name`='{}';", _deploy_task->_value );
        if ( !query_md5->IsOk() || query_md5->_value.empty() )
        {
            return true;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现

#else
        // 执行下载命令
        auto md5 = ExecuteShell( "md5sum ./version/{} | awk '{{print $1}}'", _deploy_task->_value  );
        if ( md5 != query_md5->_value )
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
        auto query_url = _deploy_driver->QueryString( "select `file_url` from `file` where `file_name`='{}';", _deploy_task->_value );
        if ( !query_url->IsOk() || query_url->_value.empty() )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 执行下载命令
        ExecuteShell( "wget -c -P ./version/ {}", query_url->_value );
#endif
    }

    bool KFDeployAgentModule::CheckDownFileTaskFinish()
    {
        auto query_result = _deploy_driver->QueryMap( "select * from `file` where `file_name`='{}';", _deploy_task->_value );
        if ( !query_result->IsOk() || query_result->_value.empty() )
        {
            return true;
        }

        auto query_md5 = query_result->_value[ "file_md5" ];
        auto query_path = query_result->_value[ "file_path" ];
        if ( query_md5.empty() || query_path.empty() )
        {
            return false;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现

#else
        // 执行下载命令
        auto md5 = ExecuteShell( "md5sum ./version/{} | awk '{{print $1}}'", _deploy_task->_value );
        if ( md5 != query_md5 )
        {
            StartDownFileTask();
            return false;
        }

        // 把文件拷贝过去
        StringSet deploy_path_list;
        FindAppDeployPath( _deploy_task->_app_name, _deploy_task->_zone_id, deploy_path_list );
        for ( auto& deploy_path : deploy_path_list )
        {
            ExecuteShell( "mkdir -p {}", deploy_path );
            ExecuteShell( "cp -rf ./version/{} {}/{}/", _deploy_task->_value, deploy_path, query_path );
        }

        LogDeploy( "update file [{}] ok", _deploy_task->_value );
#endif

        return true;
    }

    void KFDeployAgentModule::StartWgetResourceTask()
    {
        // 查询版本路径
        auto query_url = _deploy_driver->QueryString( "select `resource_url` from `resource` where `resource_name`='{}';", _deploy_task->_value );
        if ( !query_url->IsOk() || query_url->_value.empty() )
        {
            return LogDeploy( "can't find [{}] url", _deploy_task->_value );
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 执行下载命令
        ExecuteShell( "wget -c -P ./version/ {}", query_url->_value );
#endif
    }

    bool KFDeployAgentModule::CheckDownResourceTaskFinish()
    {
        auto query_md5 = _deploy_driver->QueryString( "select `resource_md5` from `resource` where `resource_name`='{}';", _deploy_task->_value );
        if ( !query_md5->IsOk() || query_md5->_value.empty() )
        {
            return true;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现

#else
        // 执行下载命令
        auto md5 = ExecuteShell( "md5sum ./version/{} | awk '{{print $1}}'", _deploy_task->_value );
        if ( md5 != query_md5->_value )
        {
            StartDeployTask();
            return false;
        }

        // 解压
        ExecuteShell( "rm -rf ./version/conf_output/" );
        ExecuteShell( "tar -zxf ./version/{} -C ./version/", _deploy_task->_value );

        // 把文件拷贝过去
        StringSet deploy_path_list;
        FindAppDeployPath( _deploy_task->_app_name, _deploy_task->_zone_id, deploy_path_list );
        for ( auto& deploy_path : deploy_path_list )
        {
            ExecuteShell( "mkdir -p {}", deploy_path );
            ExecuteShell( "cp -rf ./version/conf_output/* {}", deploy_path );
            ExecuteShell( "echo {} > {}/resource.txt", _deploy_task->_value, deploy_path );
        }

        ExecuteShell( "rm wget-log*" );
        ExecuteShell( "rm -rf ./version/*" );
        LogDeploy( "[{}] update resource ok", _deploy_task->_app_name );
#endif

        return true;
    }

    void KFDeployAgentModule::StartDownPluginTask()
    {
        auto value = _deploy_task->_value;
        KFUtility::SplitString( value, "=" );
        auto filename = KFUtility::SplitString( value, "=" );

        // 查询版本路径
        auto query_url = _deploy_driver->QueryString( "select `file_url` from `plugin` where `file_name`='{}';", filename );
        if ( !query_url->IsOk() || query_url->_value.empty() )
        {
            return;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        // 执行下载命令
        ExecuteShell( "wget -c -P ./version/ {}", query_url->_value );
#endif
    }

    bool KFDeployAgentModule::CheckDownPluginTaskFinish()
    {
        auto value = _deploy_task->_value;
        KFUtility::SplitString( value, "=" );
        auto filename = KFUtility::SplitString( value, "=" );

        auto query_result = _deploy_driver->QueryMap( "select * from `plugin` where `file_name`='{}';", filename );
        if ( !query_result->IsOk() || query_result->_value.empty() )
        {
            LogDeploy( "plugin data [{}] empty", filename );
            return true;
        }

        auto query_md5 = query_result->_value[ "file_md5" ];
        auto query_path = query_result->_value[ "file_path" ];
        if ( query_md5.empty() || query_path.empty() )
        {
            return false;
        }

#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现

#else
        // 执行下载命令
        auto md5 = ExecuteShell( "md5sum ./version/{} | awk '{{print $1}}'", filename );
        if ( md5 != query_md5 )
        {
            StartDownPluginTask();
            return false;
        }

        // 把文件拷贝过去
        StringSet deploy_path_list;
        FindAppDeployPath( _deploy_task->_app_name, _deploy_task->_zone_id, deploy_path_list );
        for ( auto& deploy_path : deploy_path_list )
        {
            ExecuteShell( "mkdir -p {}", deploy_path );
            ExecuteShell( "mv -f ./version/{} {}/bin/{}.new", filename, deploy_path, filename );
        }

        LogDeploy( "down plugin [{}] ok", filename );
#endif

        return true;
    }

    void KFDeployAgentModule::StartDevelopUpdateTask()
    {
#if __KF_SYSTEM__ == __KF_WIN__
        // todo win64暂时没有实现
#else
        ExecuteShell( "sh {}", _deploy_task->_value );
#endif
    }
}