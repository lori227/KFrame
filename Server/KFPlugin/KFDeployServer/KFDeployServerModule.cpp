#include "KFDeployServerModule.h"
#include "KFProtocol/KFProtocol.h"
#include "KFJson.h"


namespace KFrame
{
    KFDeployServerModule::KFDeployServerModule()
    {
        _mysql_driver = nullptr;
    }

    KFDeployServerModule::~KFDeployServerModule()
    {

    }

    void KFDeployServerModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFDeployServerModule::OnServerLostClient );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP_FUNCTION__( KFField::_startup, true, &KFDeployServerModule::HandleStartupServer );
        __REGISTER_HTTP_FUNCTION__( KFField::_shut_down, true, &KFDeployServerModule::HandleShutDownServer );
        __REGISTER_HTTP_FUNCTION__( KFField::_kill, true, &KFDeployServerModule::HandleKillServer );
        __REGISTER_HTTP_FUNCTION__( KFField::_download, true, &KFDeployServerModule::HandleUpdateServer );
        __REGISTER_HTTP_FUNCTION__( KFField::_restart, true, &KFDeployServerModule::HandleRestartServer );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &KFDeployServerModule::HandleRegisterAgentToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_SERVER_STATUS_REQ, &KFDeployServerModule::HandleUpdateServerStatusReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_REQ, &KFDeployServerModule::HandleGetAgentIpAddressReq );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }


    void KFDeployServerModule::ShutDown()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_HTTP_FUNCTION__( KFField::_startup );
        __UNREGISTER_HTTP_FUNCTION__( KFField::_shut_down );
        __UNREGISTER_HTTP_FUNCTION__( KFField::_kill );
        __UNREGISTER_HTTP_FUNCTION__( KFField::_download );
        __UNREGISTER_HTTP_FUNCTION__( KFField::_restart );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_SERVER_STATUS_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_REQ );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDeployServerModule::OnceRun()
    {
        _mysql_driver = _kf_mysql->CreateExecute( KFField::_deploy );

        // 清空数据库中的数据
        _mysql_driver->Execute( "truncate table %s;", KFField::_deploy.c_str() );
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFDeployServerModule::OnServerLostClient )
    {
        if ( handlename == KFField::_deploy )
        {
            _agent_list.Remove( handleid );
            _mysql_driver->Delete( KFField::_deploy, __KF_STRING__( handleid ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleRegisterAgentToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterAgentToServerReq );

        // 删除老数据
        _agent_list.Remove( kfmsg.agentid() );
        _mysql_driver->Delete( KFField::_deploy, __KF_STRING__( kfmsg.agentid() ) );

        auto kfagentdata = _agent_list.Create( kfmsg.agentid() );
        kfagentdata->_agent_id = kfmsg.agentid();
        kfagentdata->_local_ip = kfmsg.localip();
        kfagentdata->_name = kfmsg.name();
        kfagentdata->_type = kfmsg.type();
        kfagentdata->_port = kfmsg.port();
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleUpdateServerStatusReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateServerStatusReq );

        auto kfagentdata = _agent_list.Find( kfmsg.agentid() );
        if ( kfagentdata == nullptr )
        {
            return;
        }

        auto kfserverdata = kfagentdata->_server_list.Create( kfmsg.appid() );
        kfserverdata->_app_id = kfmsg.appid();
        kfserverdata->_app_name = kfmsg.appname();
        kfserverdata->_app_type = kfmsg.apptype();
        kfserverdata->_zone_id = kfmsg.zoneid();
        kfserverdata->_process_id = kfmsg.process();
        kfserverdata->_startup_time = kfmsg.startuptime();
        kfserverdata->_is_shutdown = kfmsg.isshutdown();

        // 更新数据库
        MapString values;
        values[ KFField::_app_id ] = __KF_STRING__( kfserverdata->_app_id );
        values[ KFField::_app_name ] = kfserverdata->_app_name;
        values[ KFField::_app_type ] = kfserverdata->_app_type;
        values[ KFField::_zone_id ] = __KF_STRING__( kfserverdata->_zone_id );
        values[ KFField::_process ] = __KF_STRING__( kfserverdata->_process_id );
        values[ KFField::_time ] = __KF_STRING__( kfserverdata->_startup_time );
        values[ KFField::_shut_down ] = __KF_STRING__( kfserverdata->_is_shutdown ? 1 : 0 );
        values[ KFField::_agent_id ] = __KF_STRING__( kfagentdata->_agent_id );
        values[ KFField::_local_ip ] = kfagentdata->_local_ip;
        _mysql_driver->Insert( KFField::_deploy, values );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleGetAgentIpAddressReq )
    {
        __PROTO_PARSE__( KFMsg::S2SGetAgentIpAddressReq );

        auto clientid = __KF_HEAD_ID__( kfguid );
        for ( auto& iter : _agent_list._objects )
        {
            auto* kfagent = iter.second;
            if ( kfagent->_local_ip == kfmsg.localip() )
            {
                KFMsg::S2SGetAgentIpAddressAck ack;
                ack.set_appname( kfagent->_name );
                ack.set_apptype( kfagent->_type );
                ack.set_appid( kfagent->_agent_id );
                ack.set_ip( kfagent->_local_ip );
                ack.set_port( kfagent->_port );
                _kf_tcp_server->SendNetMessage( clientid, KFMsg::S2S_GET_AGENT_IP_ADDRESS_ACK, &ack );
                break;
            }
        }
    }

    __KF_HTTP_FUNCTION__( KFDeployServerModule::HandleShutDownServer )
    {
        KFJson request( data );

        auto scheduletime = request.GetUInt32( KFField::_schedule_time );
        if ( scheduletime == _invalid_int || scheduletime <= KFGlobal::Instance()->_real_time )
        {
            ShutDownServerToAgent( _invalid_int, data.c_str(), data.size() );
        }
        else
        {
            auto kfsetting = _kf_schedule->CreateScheduleSetting();
            kfsetting->SetTime( scheduletime );
            kfsetting->SetData( _invalid_int, data.c_str(), data.size() );
            _kf_schedule->RegisterSchedule( kfsetting, this, &KFDeployServerModule::ShutDownServerToAgent );
        }

        return _invalid_str;
    }

    void KFDeployServerModule::ShutDownServerToAgent( uint32 id, const char* data, uint32 size )
    {
        KFJson request( data, size );

        auto appname = request.GetString( KFField::_app_name );
        auto apptype = request.GetString( KFField::_app_type );
        auto appid = request.GetUInt32( KFField::_app_id );
        auto zoneid = request.GetUInt32( KFField::_zone_id );
        auto delaytime = request.GetUInt32( KFField::_delay_time );

        KFMsg::S2SShutDownServerToAgentReq req;
        req.set_appname( appname );
        req.set_apptype( apptype );
        req.set_appid( appid );
        req.set_zoneid( zoneid );
        req.set_delaytime( __MAX__( delaytime, 10000 ) );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_SHUT_DOWN_SERVER_TO_AGENT_REQ, &req );
    }

    __KF_HTTP_FUNCTION__( KFDeployServerModule::HandleStartupServer )
    {
        KFJson request( data );

        auto scheduletime = request.GetUInt32( KFField::_schedule_time );
        if ( scheduletime == _invalid_int || scheduletime <= KFGlobal::Instance()->_real_time )
        {
            StartupServerToAgent( _invalid_int, data.c_str(), data.size() );
        }
        else
        {
            auto kfsetting = _kf_schedule->CreateScheduleSetting();
            kfsetting->SetTime( scheduletime );
            kfsetting->SetData( _invalid_int, data.c_str(), data.size() );
            _kf_schedule->RegisterSchedule( kfsetting, this, &KFDeployServerModule::StartupServerToAgent );
        }

        return _invalid_str;
    }

    void KFDeployServerModule::StartupServerToAgent( uint32 id, const char* data, uint32 size )
    {
        KFJson request( data, size );

        auto appname = request.GetString( KFField::_app_name );
        auto apptype = request.GetString( KFField::_app_type );
        auto appid = request.GetUInt32( KFField::_app_id );
        auto zoneid = request.GetUInt32( KFField::_zone_id );

        KFMsg::S2SStartupServerToAgentReq req;
        req.set_appname( appname );
        req.set_apptype( apptype );
        req.set_appid( appid );
        req.set_zoneid( zoneid );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_STARTUP_SERVER_TO_AGENT_REQ, &req );
    }

    __KF_HTTP_FUNCTION__( KFDeployServerModule::HandleKillServer )
    {
        KFJson request( data );

        auto appname = request.GetString( KFField::_app_name );
        auto apptype = request.GetString( KFField::_app_type );
        auto appid = request.GetUInt32( KFField::_app_id );
        auto zoneid = request.GetUInt32( KFField::_zone_id );

        KFMsg::S2SKillServerToAgentReq req;
        req.set_appname( appname );
        req.set_apptype( apptype );
        req.set_appid( appid );
        req.set_zoneid( zoneid );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_KILL_SERVER_TO_AGENT_REQ, &req );

        return _invalid_str;
    }

    __KF_HTTP_FUNCTION__( KFDeployServerModule::HandleUpdateServer )
    {
        KFJson request( data );

        auto scheduletime = request.GetUInt32( KFField::_schedule_time );
        if ( scheduletime == _invalid_int || scheduletime <= KFGlobal::Instance()->_real_time )
        {
            UpdateServerToAgnet( _invalid_int, data.c_str(), data.size() );
        }
        else
        {
            auto kfsetting = _kf_schedule->CreateScheduleSetting();
            kfsetting->SetTime( scheduletime );
            kfsetting->SetData( _invalid_int, data.c_str(), data.size() );
            _kf_schedule->RegisterSchedule( kfsetting, this, &KFDeployServerModule::UpdateServerToAgnet );
        }

        return _invalid_str;
    }

    void KFDeployServerModule::UpdateServerToAgnet( uint32 id, const char* data, uint32 size )
    {
        KFJson request( data, size );

        auto appname = request.GetString( KFField::_app_name );
        auto apptype = request.GetString( KFField::_app_type );
        auto appid = request.GetUInt32( KFField::_app_id );
        auto zoneid = request.GetUInt32( KFField::_zone_id );

        KFMsg::S2SUpdateServerToAgentReq req;
        req.set_appname( appname );
        req.set_apptype( apptype );
        req.set_appid( appid );
        req.set_zoneid( zoneid );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_UPDATE_SERVER_TO_AGENT_REQ, &req );
    }

    __KF_HTTP_FUNCTION__( KFDeployServerModule::HandleRestartServer )
    {
        KFJson request( data );

        auto appname = request.GetString( KFField::_app_name );
        auto apptype = request.GetString( KFField::_app_type );
        auto appid = request.GetUInt32( KFField::_app_id );
        auto zoneid = request.GetUInt32( KFField::_zone_id );
        auto delaytime = request.GetUInt32( KFField::_delay_time );

        KFMsg::S2SRestartServerToAgentReq req;
        req.set_appname( appname );
        req.set_apptype( apptype );
        req.set_appid( appid );
        req.set_zoneid( zoneid );
        req.set_delaytime( delaytime );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_START_SERVER_TO_AGENT_REQ, &req );

        return _invalid_str;
    }
}