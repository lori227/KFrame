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
        __REGISTER_MESSAGE__( KFMsg::S2S_SHUT_DOWN_SERVER_TO_SERVER_REQ, &KFDeployCommandModule::HandleShutDownServerToServerReq );
        /////////////////////////////////////////////////////////////////////////////
    }

    void KFDeployCommandModule::BeforeShut()
    {
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SHUT_DOWN_SERVER_TO_SERVER_REQ );
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

    bool KFDeployCommandModule::IsSelfServer( const std::string& appname, const std::string& apptype, uint32 appid, uint32 zoneid )
    {
        auto kfglobal = KFGlobal::Instance();

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

        if ( appid == _invalid_int )
        {
            return true;
        }

        if ( appid != kfglobal->_app_id )
        {
            return false;
        }

        return true;

    }

    __KF_MESSAGE_FUNCTION__( KFDeployCommandModule::HandleShutDownServerToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SShutDownServerToServerReq );

        ShutDownServer( kfmsg.appname(), kfmsg.apptype(), kfmsg.appid(), kfmsg.zoneid(), kfmsg.delaytime() );
    }

    void KFDeployCommandModule::ShutDownServer( const std::string& appname, const std::string& apptype, uint32 appid, uint32 zoneid, uint32 delaytime )
    {
        KFLogger::LogSystem( KFLogger::Info, "[%s:%s:%u:%u:%u] shutdown req!",
                             appname.c_str(), apptype.c_str(), appid, zoneid, delaytime );

        // 判断是不是自己
        auto ok = IsSelfServer( appname, apptype, appid, zoneid );
        if ( !ok )
        {
            return;
        }

        auto kfglobal = KFGlobal::Instance();
        KFLogger::LogSystem( KFLogger::Info, "[%s:%s:%u:%u:%u] shutdown start!",
                             kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(),
                             kfglobal->_app_id, kfglobal->_zone_id, delaytime );

        // 如果是数据库
        if ( appname == KFField::_data )
        {
            delaytime += 30000;
        }

        // 启动一个定时器
        __REGISTER_DELAY_TIMER__( appid, delaytime, &KFDeployCommandModule::OnTimerShutDownPrepare );
    }

    __KF_TIMER_FUNCTION__( KFDeployCommandModule::OnTimerShutDownPrepare )
    {
        __REGISTER_LIMIT_TIMER__( objectid, 10000, 1, &KFDeployCommandModule::OnTimerShutDownServer );

        auto kfglobal = KFGlobal::Instance();
        KFLogger::LogSystem( KFLogger::Info, "[%s:%s:%u:%u] shutdown prepare!",
                             kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(),
                             kfglobal->_app_id, kfglobal->_zone_id );

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

        KFLogger::LogSystem( KFLogger::Info, "[%s:%s:%u:%u] shutdown ok!",
                             kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(),
                             kfglobal->_app_id, kfglobal->_zone_id );

        kfglobal->_app_run = false;
    }
}