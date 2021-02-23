#include "KFDeployClientModule.hpp"
#include "KFLoader/KFLoaderInterface.h"

namespace KFrame
{
    void KFDeployClientModule::BeforeRun()
    {
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFDeployClientModule::OnClientConnectAgent );
        __REGISTER_TCP_CLIENT_SHUTDOWN__( &KFDeployClientModule::OnClientLostAgent );

        __REGISTER_DEPLOY_FUNCTION__( __STRING__( shutdown ), &KFDeployClientModule::OnDeployShutDownServer );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( loglevel ), &KFDeployClientModule::OnDeployLogLevel );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( logmemory ), &KFDeployClientModule::OnDeployLogMemory );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( loadconfig ), &KFDeployClientModule::OnDeployLoadConfig );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( loadplugin ), &KFDeployClientModule::OnDeployLoadPlugin );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( messageclose ), &KFDeployClientModule::OnDeployMessageClose );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( messageopen ), &KFDeployClientModule::OnDeployMessageOpen );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( moduleopen ), &KFDeployClientModule::OnDeployModuleOpen );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( moduleclose ), &KFDeployClientModule::OnDeployModuleClose );
        ////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DEPLOY_COMMAND_TO_CLIENT_REQ, &KFDeployClientModule::HandleDeployCommandToClientReq );
    }

    void KFDeployClientModule::ShutDown()
    {
        __UN_TIMER_0__();
        __UN_TCP_CLIENT_CONNECTION__();
        __UN_TCP_CLIENT_SHUTDOWN__();
        /////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_CLIENT_REQ );
    }

    void KFDeployClientModule::PrepareRun()
    {
        auto agent_data = KFGlobal::Instance()->_startup_params[ __STRING__( agent ) ];
        if ( agent_data.empty() )
        {
            return;
        }

        _agent_id = KFUtility::SplitValue<uint64>( agent_data, "|" );
        _agent_ip = KFUtility::SplitString( agent_data, "|" );
        _agent_port = KFUtility::SplitValue<uint32>( agent_data, "|" );
        _kf_tcp_client->StartClient( __STRING__( deploy ), __STRING__( agent ), _agent_id, _agent_ip, _agent_port );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFDeployClientModule::OnClientConnectAgent )
    {
        if ( net_data->_id == _agent_id )
        {
            // 启动定时器
            __LOOP_TIMER_1__( _agent_id, 45000, 10000, &KFDeployClientModule::OnTimerSendHeartbeatToAgent );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFDeployClientModule::OnClientLostAgent )
    {
        if ( net_data->_id == _agent_id )
        {
            __UN_TIMER_1__( _agent_id );

            // 重新开始连接
            _kf_tcp_client->StartClient( __STRING__( deploy ), __STRING__( agent ), _agent_id, _agent_ip, _agent_port );
        }
    }

    __KF_TIMER_FUNCTION__( KFDeployClientModule::OnTimerSendHeartbeatToAgent )
    {
        KFMsg::S2SDeployHeartbeatToAgentReq req;
        req.set_id( KFGlobal::Instance()->_app_id->GetId() );
        _kf_tcp_client->SendNetMessage( _agent_id, KFMsg::S2S_DEPLOY_HEARTBEAT_TO_AGENT_REQ, &req );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDeployClientModule::AddFunction( const std::string& command, KFModule* module, KFDeployFunction& function )
    {
        auto command_data = _command_data.Create( command );
        auto function_data = command_data->_function_list.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFDeployClientModule::RemoveFunction( const std::string& command, KFModule* module )
    {
        auto command_data = _command_data.Find( command );
        if ( command_data != nullptr )
        {
            command_data->_function_list.Remove( module );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDeployClientModule::HandleDeployCommandToClientReq, KFMsg::S2SDeployCommandToClientReq )
    {
        auto deploy_command = &kfmsg->deploycommand();

        // 部署命令
        DeployCommand( deploy_command->command(), deploy_command->value(),
                       deploy_command->appname(), deploy_command->apptype(), deploy_command->appid(), deploy_command->zoneid() );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDeployClientModule::IsSelfServer( const std::string& app_name, const std::string& app_type, const std::string& app_id, uint32 zone_id )
    {
        auto global = KFGlobal::Instance();

        // 指定appid
        if ( app_id != _globbing_string )
        {
            return ( app_id == global->_app_id->ToString() );
        }

        // appname
        if ( app_name != _globbing_string )
        {
            if ( app_name != global->_app_name )
            {
                return false;
            }
        }

        // apptype
        if ( app_type != _globbing_string )
        {
            if ( app_type != global->_app_type )
            {
                return false;
            }
        }

        // zone_id
        if ( global->_app_name == __STRING__( zone ) )
        {
            if ( zone_id != _invalid_int )
            {
                if ( zone_id != global->_app_id->GetZoneId() )
                {
                    return false;
                }
            }
        }

        return true;
    }

    void KFDeployClientModule::DeployCommand( const std::string& command, const std::string& value,
            const std::string& app_name, const std::string& app_type, const std::string& app_id, uint32 zone_id )
    {
        // 判断是不是自己
        auto ok = IsSelfServer( app_name, app_type, app_id, zone_id );
        if ( !ok )
        {
            return;
        }

        __LOG_INFO__( "[{}:{} | {}:{}:{}:{}] deploy command req", command, value, app_name, app_type, app_id, zone_id );
        CallDeployFunction( command, value );
    }

    void KFDeployClientModule::CallDeployFunction( const std::string& command, const std::string& value )
    {
        auto command_data = _command_data.Find( command );
        if ( command_data == nullptr )
        {
            return;
        }

        for ( auto& iter : command_data->_function_list._objects )
        {
            auto function = iter.second;
            function->Call( value );
        }
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployShutDownServer )
    {
        if ( param.empty() )
        {
            return;
        }

        auto global = KFGlobal::Instance();
        auto delay_time = __TO_UINT32__( param );
        __LOG_INFO__( "[{}:{}:{}:{}] shutdown start", global->_app_name, global->_app_type, global->_app_id->ToString(), delay_time );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        delay_time = __MAX__( delay_time, 10000u );
        if ( global->_app_name != __STRING__( zone ) )
        {
            delay_time += 30000;
        }
        else
        {
            if ( global->_app_type == __STRING__( world ) )
            {
                delay_time += 20000;
            }
        }
        __COUNT_TIMER_1__( global->_app_id->GetId(), delay_time, 1u, &KFDeployClientModule::OnTimerShutDownServer );
    }

    __KF_TIMER_FUNCTION__( KFDeployClientModule::OnTimerShutDownServer )
    {
        auto global = KFGlobal::Instance();
        __LOG_INFO__( "[{}:{}:{}] shutdown ok", global->_app_name, global->_app_type, global->_app_id->ToString() );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallDeployFunction( __STRING__( shutdown ), _invalid_string );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////

        // linux程序正常退出时会core, 应该是每个so文件中的同名全局变量造成的
        // 这里不让程序退出, 由agent等待超时kill
        // global->_app_run = false;
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployLogLevel )
    {
        auto level = __TO_UINT32__( param );
        KFLogger::Instance()->SetLogLevel( level );
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployLogMemory )
    {
        auto memory = __TO_UINT32__( param );
        KFMalloc::Instance()->SetLogOpen( memory == 1u ? true : false );
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployLoadConfig )
    {
        _kf_loader->ReloadConfig();
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployLoadPlugin )
    {
        auto str_command = __FORMAT__( "{}={}", __STRING__( loadplugin ), param );
        _kf_plugin_manage->AddCommand( str_command );
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployMessageOpen )
    {
        UInt32List message_list;
        KFUtility::SplitList( param, __SPLIT_STRING__, message_list );
        for ( auto message_id : message_list )
        {
            _kf_message->OpenFunction( message_id, true );
        }
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployMessageClose )
    {
        UInt32List message_list;
        KFUtility::SplitList( param, __SPLIT_STRING__, message_list );
        for ( auto message_id : message_list )
        {
            _kf_message->OpenFunction( message_id, false );
        }
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployModuleOpen )
    {
        KFPluginManage::Instance()->SetModuleOpen( param, true );
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployModuleClose )
    {
        KFPluginManage::Instance()->SetModuleOpen( param, false );
    }
}