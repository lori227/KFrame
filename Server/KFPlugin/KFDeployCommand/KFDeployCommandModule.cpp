#include "KFDeployCommandModule.h"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    void KFDeployCommandModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_SERVER_REQ, &KFDeployCommandModule::HandleDeployCommandToServerReq );
        /////////////////////////////////////////////////////////////////////////////
    }

    void KFDeployCommandModule::BeforeShut()
    {
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_COMMAND_TO_SERVER_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    void KFDeployCommandModule::AddCommandFunction( const std::string& command, const std::string& module, KFCommandFunction& function )
    {
        auto kfcommand = _command_data.Create( command );
        auto kffunction = kfcommand->_command_function.Create( module );
        kffunction->_function = function;
    }

    void KFDeployCommandModule::RemoveComandFunction( const std::string& command, const std::string& module )
    {
        auto kfcommand = _command_data.Create( command );
        kfcommand->_command_function.Remove( module );
    }

    bool KFDeployCommandModule::IsSelfServer( const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid )
    {
        auto kfglobal = KFGlobal::Instance();

        if ( zoneid != _invalid_int )
        {
            if ( zoneid != kfglobal->_zone_id )
            {
                return false;
            }
        }

        if ( appname == _globbing_str )
        {
            return true;
        }

        if ( appname != kfglobal->_app_name )
        {
            return false;
        }

        if ( apptype == _globbing_str )
        {
            return true;
        }

        if ( apptype != kfglobal->_app_type )
        {
            return false;
        }

        if ( appid == _globbing_str )
        {
            return true;
        }

        if ( appid != kfglobal->_str_app_id )
        {
            return false;
        }

        return true;

    }

    __KF_MESSAGE_FUNCTION__( KFDeployCommandModule::HandleDeployCommandToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployCommandToServerReq );

        auto* pbdeploy = &kfmsg.deploycommand();
        DeployCommand( pbdeploy->command(), pbdeploy->value(), pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );
    }

    void KFDeployCommandModule::DeployCommand( const std::string& command, const std::string& value, const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid )
    {
        __LOG_INFO__( "[{}:{} | {}:{}:{}:{}] deploy command req!", command, value, appname, apptype, appid, zoneid );

        // 判断是不是自己
        auto ok = IsSelfServer( appname, apptype, appid, zoneid );
        if ( !ok )
        {
            return;
        }

        __LOG_INFO__( "[{}:{} | {}:{}:{}:{}] deploy command start!", command, value, appname, apptype, appid, zoneid );

        if ( command == __KF_STRING__( shutdown ) )
        {
            auto delaytime = KFUtility::ToValue< uint32 >( value );
            return ShutDownServer( appname, apptype, appid, zoneid, delaytime );
        }
        else if ( command == __KF_STRING__( log ) )
        {
            auto level = KFUtility::ToValue< uint32 >( value );
            return KFGlobal::Instance()->SetLogLevel( level );
        }

        auto kfcommand = _command_data.Find( command );
        if ( kfcommand == nullptr )
        {
            return;
        }

        __LOG_INFO__( "[{}:{} | {}:{}:{}:{}] deploy command process!", command, value, appname, apptype, appid, zoneid );

        for ( auto& iter : kfcommand->_command_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( value );
        }
    }

    void KFDeployCommandModule::ShutDownServer( const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid, uint32 delaytime )
    {
        auto kfglobal = KFGlobal::Instance();
        __LOG_INFO__( "[{}:{}:{}:{}:{}] shutdown start!",
                      kfglobal->_app_name, kfglobal->_app_type,
                      kfglobal->_str_app_id, kfglobal->_zone_id, delaytime );

        // 如果是服务
        if ( appname != __KF_STRING__( zone ) )
        {
            delaytime += 30000;
        }

        // 启动一个定时器
        __REGISTER_DELAY_TIMER__( KFAppID::ToUInt32( appid ), delaytime, &KFDeployCommandModule::OnTimerShutDownPrepare );
    }

    __KF_TIMER_FUNCTION__( KFDeployCommandModule::OnTimerShutDownPrepare )
    {
        __REGISTER_LIMIT_TIMER__( objectid, 10000, 1, &KFDeployCommandModule::OnTimerShutDownServer );

        auto kfglobal = KFGlobal::Instance();
        __LOG_INFO__( "[{}:{}:{}:{}] shutdown prepare!",
                      kfglobal->_app_name, kfglobal->_app_type,
                      kfglobal->_str_app_id, kfglobal->_zone_id );

        auto kfcommand = _command_data.Find( __KF_STRING__( shutdown ) );
        if ( kfcommand != nullptr )
        {
            for ( auto& iter : kfcommand->_command_function._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( _invalid_str );
            }
        }
    }

    __KF_TIMER_FUNCTION__( KFDeployCommandModule::OnTimerShutDownServer )
    {
        auto kfglobal = KFGlobal::Instance();

        __LOG_INFO__( "[{}:{}:{}:{}] shutdown ok!",
                      kfglobal->_app_name, kfglobal->_app_type,
                      kfglobal->_str_app_id, kfglobal->_zone_id );

        // linux程序正常退出时会core, 判断是每个so文件中的同名全局函数造成
        // 这里不让程序退出, 由agent等待超时kill
        // kfglobal->_app_run = false;
    }
}