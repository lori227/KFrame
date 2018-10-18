#include "KFDeployCommandModule.h"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    KFDeployCommandModule::KFDeployCommandModule()
    {

    }

    KFDeployCommandModule::~KFDeployCommandModule()
    {
    }

    void KFDeployCommandModule::InitModule()
    {
    }

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

    void KFDeployCommandModule::AddShutDownFunction( const std::string& module, KFCommandFunction& function )
    {
        auto kffunciton = _shutdown_function.Create( module );
        kffunciton->_function = function;
    }

    void KFDeployCommandModule::RemoveShutDownFunction( const std::string& module )
    {
        _shutdown_function.Remove( module );
    }

    bool KFDeployCommandModule::IsSelfServer( uint32 appchannel, const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid )
    {
        auto kfglobal = KFGlobal::Instance();

        if ( appchannel != kfglobal->_app_channel )
        {
            return false;
        }

        if ( appname == _globbing_str )
        {
            return true;
        }

        if ( appname != kfglobal->_app_name )
        {
            return false;
        }

        if ( zoneid != kfglobal->_zone_id )
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
        DeployCommand( pbdeploy->command(), pbdeploy->value(), pbdeploy->appchannel(),
                       pbdeploy->appname(), pbdeploy->apptype(), pbdeploy->appid(), pbdeploy->zoneid() );
    }

    void KFDeployCommandModule::DeployCommand( const std::string& command, const std::string& value, uint32 appchannel,
            const std::string& appname, const std::string& apptype, const std::string& appid, uint32 zoneid )
    {
        __LOG_INFO__( "[{}:{} | {}:{}:{}:{}:{}] deploy command req!",
                      command, value, appchannel, appname, apptype, appid, zoneid );

        // 判断是不是自己
        auto ok = IsSelfServer( appchannel, appname, apptype, appid, zoneid );
        if ( !ok )
        {
            return;
        }

        __LOG_INFO__( "[{}:{} | {}:{}:{}:{}:{}] deploy command start!",
                      command, value, appchannel, appname, apptype, appid, zoneid );

        if ( command == __KF_STRING__( shutdown ) )
        {
            auto delaytime = KFUtility::ToValue< uint32 >( value );
            ShutDownServer( appname, apptype, appid, zoneid, delaytime );
        }
        else if ( command == __KF_STRING__( log ) )
        {
            auto level = KFUtility::ToValue< uint32 >( value );
            KFGlobal::Instance()->SetLogLevel( level );
        }
        else if ( command == __KF_STRING__( loadconfig ) )
        {
            auto tempvalue = value;
            KFUtility::SplitString( tempvalue, "." );
            if ( tempvalue == __KF_STRING__( config ) )
            {
                _kf_config->LoadConfig( value );
            }
            else if ( tempvalue == __KF_STRING__( lua ) )
            {
                if ( _kf_lua != nullptr )
                {
                    _kf_lua->LoadScript( value );
                }
            }
        }
        else
        {
            __LOG_INFO__( "[{}:{} | {}:{}:{}:{}:{}] deploy command no process!",
                          command, value, appchannel, appname, apptype, appid, zoneid );
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

        // 回调关闭回调
        for ( auto& iter : _shutdown_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function();
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