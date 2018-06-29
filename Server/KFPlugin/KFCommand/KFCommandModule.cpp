#include "KFCommandModule.h"
#include "KFCommandConfig.h"
#include "KFJson.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    KFCommandModule::KFCommandModule()
    {

    }

    KFCommandModule::~KFCommandModule()
    {
    }

    void KFCommandModule::InitModule()
    {
        /////////////////////////////////////////////////////////////////////////////
        __KF_ADD_CONFIG__( _kf_command_config, true );
    }

    void KFCommandModule::BeforeRun()
    {
        _kf_timer->RegisterLoopTimer( __LINE__, 5000, this, &KFCommandModule::OnTimerQueryCommand );

        /////////////////////////////////////////////////////////////////////////////
        RegisterCommandFunction( "shutdown", this, &KFCommandModule::ExecuteShutDown );
        RegisterCommandFunction( "loadconfig", this, &KFCommandModule::ExecuteLoadConfig );
    }

    void KFCommandModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        _kf_timer->UnRegisterTimer( this );
        /////////////////////////////////////////////////////////////////////////////

    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
#define __COMMAND_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_login )

    void KFCommandModule::AddCommand( const char* appname, uint32 appid, const char* command )
    {
        // 保存到redis
        auto redisdriver = __COMMAND_REDIS_DRIVER__;
        if ( redisdriver == nullptr )
        {
            return;
        }

        redisdriver->VoidExecute( "sadd %s:%u %s", appname, appid, command );
    }

    void KFCommandModule::RegisterFunction( const char* command, KFCommandFunction& function )
    {
        auto kffunction = _kf_command_function.Create( command );
        kffunction->_function = function;
    }

    void KFCommandModule::UnRegisterCommandFunction( const char* command )
    {
        _kf_command_function.Remove( command );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFCommandModule::OnTimerQueryCommand )
    {
        auto redisdriver = __COMMAND_REDIS_DRIVER__;
        if ( redisdriver == nullptr )
        {
            return;
        }

        auto kfglobal = KFGlobal::Instance();

        VectorString commands;
        redisdriver->VectorExecute( commands, "smembers %s:%u", kfglobal->_app_name.c_str(), kfglobal->_app_id );
        if ( commands.empty() )
        {
            return;
        }

        // 删除数据
        redisdriver->VoidExecute( "del %s:%u", kfglobal->_app_name.c_str(), kfglobal->_app_id );

        // 执行执行
        for ( auto& command : commands )
        {
            ExecuteCommand( command );
        }
    }

    void KFCommandModule::ExecuteCommand( const std::string& command )
    {
        KFLogger::LogSystem( KFLogger::Info, "Execute Command [%s]", command.c_str() );

        KFJson json( command );
        auto strcommand = json.GetString( KFField::_command );

        auto kffunction = _kf_command_function.Find( strcommand );
        if ( kffunction == nullptr )
        {
            return;
        }

        kffunction->_function( json );
    }

    void KFCommandModule::ExecuteLoadConfig( KFJson& json )
    {
        auto file = json.GetString( KFField::_file );
        _kf_config->LoadConfig( file );
    }

    void KFCommandModule::ExecuteShutDown( KFJson& json )
    {
        auto time = json.GetUInt32( KFField::_time );

        // 调用回调
        _kf_timer->RegisterLimitTimer( __LINE__, time * TimeEnum::OneSecondMicSecond, 1, this, &KFCommandModule::OnTimerServiceShutDown );
    }

    __KF_TIMER_FUNCTION__( KFCommandModule::OnTimerServiceShutDown )
    {
        KFGlobal::Instance()->_app_run = false;
    }
}