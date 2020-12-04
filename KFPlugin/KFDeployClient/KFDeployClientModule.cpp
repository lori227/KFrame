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
        __REGISTER_MESSAGE__( KFDeployClientModule, KFMsg::S2S_DEPLOY_COMMAND_TO_CLIENT_REQ, KFMsg::S2SDeployCommandToClientReq, HandleDeployCommandToClientReq );
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
        auto agentdata = KFGlobal::Instance()->_startup_params[ __STRING__( agent ) ];
        if ( agentdata.empty() )
        {
            return;
        }

        _agent_id = KFUtility::SplitValue<uint64>( agentdata, "|" );
        _agent_ip = KFUtility::SplitString( agentdata, "|" );
        _agent_port = KFUtility::SplitValue< uint32 >( agentdata, "|" );
        _kf_tcp_client->StartClient( __STRING__( deploy ), __STRING__( agent ), _agent_id, _agent_ip, _agent_port );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFDeployClientModule::OnClientConnectAgent )
    {
        if ( netdata->_id == _agent_id )
        {
            // 启动定时器
            __LOOP_TIMER_1__( _agent_id, 45000, 10000, &KFDeployClientModule::OnTimerSendHeartbeatToAgent );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFDeployClientModule::OnClientLostAgent )
    {
        if ( netdata->_id == _agent_id )
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
    void KFDeployClientModule::AddFunction( const std::string& command, const std::string& module, KFDeployFunction& function )
    {
        auto kfcommand = _command_data.Create( command );
        auto kffunction = kfcommand->_functions.Create( module );
        kffunction->_function = function;
    }

    void KFDeployClientModule::RemoveFunction( const std::string& command, const std::string& module )
    {
        auto kfcommand = _command_data.Create( command );
        kfcommand->_functions.Remove( module );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDeployClientModule::HandleDeployCommandToClientReq, KFMsg::S2SDeployCommandToClientReq )
    {
        auto pbdeploy = &kfmsg->deploycommand();

        // 部署命令
        DeployCommand( pbdeploy->command(), pbdeploy->value(), pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDeployClientModule::IsSelfServer( const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid )
    {
        auto kfglobal = KFGlobal::Instance();

        // 指定appid
        if ( appid != _globbing_string )
        {
            return ( appid == kfglobal->_app_id->ToString() );
        }

        // appname
        if ( appname != _globbing_string )
        {
            if ( appname != kfglobal->_app_name )
            {
                return false;
            }
        }

        // apptype
        if ( apptype != _globbing_string )
        {
            if ( apptype != kfglobal->_app_type )
            {
                return false;
            }
        }

        // zoneid
        if ( kfglobal->_app_name == __STRING__( zone ) )
        {
            if ( zoneid != _invalid_int )
            {
                if ( zoneid != kfglobal->_app_id->GetZoneId() )
                {
                    return false;
                }
            }
        }

        return true;
    }

    void KFDeployClientModule::DeployCommand( const std::string& command, const std::string& value,
            const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid )
    {
        // 判断是不是自己
        auto ok = IsSelfServer( appname, apptype, appid, zoneid );
        if ( !ok )
        {
            return;
        }

        __LOG_INFO__( "[{}:{} | {}:{}:{}:{}] deploy command req", command, value, appname, apptype, appid, zoneid );
        CallDeployFunction( command, value );
    }

    void KFDeployClientModule::CallDeployFunction( const std::string& command, const std::string& value )
    {
        auto kfcommand = _command_data.Find( command );
        if ( kfcommand == nullptr )
        {
            return;
        }

        for ( auto& iter : kfcommand->_functions._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( value );
        }
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployShutDownServer )
    {
        if ( param.empty() )
        {
            return;
        }

        auto kfglobal = KFGlobal::Instance();
        auto delaytime = __TO_UINT32__( param );
        __LOG_INFO__( "[{}:{}:{}:{}] shutdown start", kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString(), delaytime );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        delaytime = __MAX__( delaytime, 10000u );
        if ( kfglobal->_app_name != __STRING__( zone ) )
        {
            delaytime += 30000;
        }
        else
        {
            if ( kfglobal->_app_type == __STRING__( world ) )
            {
                delaytime += 20000;
            }
        }
        __LIMIT_TIMER_1__( kfglobal->_app_id->GetId(), delaytime, 1u, &KFDeployClientModule::OnTimerShutDownServer );
    }

    __KF_TIMER_FUNCTION__( KFDeployClientModule::OnTimerShutDownServer )
    {
        auto kfglobal = KFGlobal::Instance();
        __LOG_INFO__( "[{}:{}:{}] shutdown ok", kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallDeployFunction( __STRING__( shutdown ), _invalid_string );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////

        // linux程序正常退出时会core, 应该是每个so文件中的同名全局变量造成的
        // 这里不让程序退出, 由agent等待超时kill
        // kfglobal->_app_run = false;
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
        auto strcommand = __FORMAT__( "{}={}", __STRING__( loadplugin ), param );
        _kf_plugin_manage->AddCommand( strcommand );
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployMessageOpen )
    {
        UInt32List messagelist;
        auto strvalue = param;
        KFUtility::SplitList( messagelist, strvalue, __SPLIT_STRING__ );
        for ( auto messageid : messagelist )
        {
            _kf_message->OpenHandle( messageid, true );
        }
    }

    __KF_DEPLOY_FUNCTION__( KFDeployClientModule::OnDeployMessageClose )
    {
        UInt32List messagelist;
        auto strvalue = param;
        KFUtility::SplitList( messagelist, strvalue, __SPLIT_STRING__ );
        for ( auto messageid : messagelist )
        {
            _kf_message->OpenHandle( messageid, false );
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