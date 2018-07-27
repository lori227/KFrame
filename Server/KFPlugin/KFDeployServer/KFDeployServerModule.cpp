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
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( startup ), true, &KFDeployServerModule::HandleStartupServer );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( shutdown ), true, &KFDeployServerModule::HandleShutDownServer );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( kill ), true, &KFDeployServerModule::HandleKillServer );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( download ), true, &KFDeployServerModule::HandleUpdateServer );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( restart ), true, &KFDeployServerModule::HandleRestartServer );

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
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( startup ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( shutdown ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( kill ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( download ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( restart ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_SERVER_STATUS_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_REQ );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDeployServerModule::OnceRun()
    {
        _mysql_driver = _kf_mysql->CreateExecute( __KF_STRING__( deploy ) );

        // 清空数据库中的数据
        _mysql_driver->Execute( "truncate table %s;", __KF_CHAR__( deploy ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFDeployServerModule::OnServerLostClient )
    {
        if ( handlename == __KF_STRING__( deploy ) )
        {
            _agent_list.Remove( handleid );
            _mysql_driver->Delete( __KF_STRING__( deploy ), __TO_STRING__( handleid ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleRegisterAgentToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterAgentToServerReq );

        // 删除老数据
        _agent_list.Remove( kfmsg.agentid() );
        _mysql_driver->Delete( __KF_STRING__( deploy ), __TO_STRING__( kfmsg.agentid() ) );

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
        values[ __KF_STRING__( appid ) ] = __TO_STRING__( kfserverdata->_app_id );
        values[ __KF_STRING__( appname ) ] = kfserverdata->_app_name;
        values[ __KF_STRING__( apptype ) ] = kfserverdata->_app_type;
        values[ __KF_STRING__( zoneid ) ] = __TO_STRING__( kfserverdata->_zone_id );
        values[ __KF_STRING__( process ) ] = __TO_STRING__( kfserverdata->_process_id );
        values[ __KF_STRING__( time ) ] = __TO_STRING__( kfserverdata->_startup_time );
        values[ __KF_STRING__( shutdown ) ] = __TO_STRING__( kfserverdata->_is_shutdown ? 1 : 0 );
        values[ __KF_STRING__( agentid ) ] = __TO_STRING__( kfagentdata->_agent_id );
        values[ __KF_STRING__( localip ) ] = kfagentdata->_local_ip;
        _mysql_driver->Insert( __KF_STRING__( deploy ), values );
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

        auto scheduletime = request.GetUInt32( __KF_STRING__( scheduletime ) );
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

        auto appname = request.GetString( __KF_STRING__( appname ) );
        auto apptype = request.GetString( __KF_STRING__( apptype ) );
        auto appid = request.GetUInt32( __KF_STRING__( appid ) );
        auto zoneid = request.GetUInt32( __KF_STRING__( zoneid ) );
        auto delaytime = request.GetUInt32( __KF_STRING__( delaytime ) );

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

        auto scheduletime = request.GetUInt32( __KF_STRING__( scheduletime ) );
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

        auto appname = request.GetString( __KF_STRING__( appname ) );
        auto apptype = request.GetString( __KF_STRING__( apptype ) );
        auto appid = request.GetUInt32( __KF_STRING__( appid ) );
        auto zoneid = request.GetUInt32( __KF_STRING__( zoneid ) );

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

        auto appname = request.GetString( __KF_STRING__( appname ) );
        auto apptype = request.GetString( __KF_STRING__( apptype ) );
        auto appid = request.GetUInt32( __KF_STRING__( appid ) );
        auto zoneid = request.GetUInt32( __KF_STRING__( zoneid ) );

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

        auto scheduletime = request.GetUInt32( __KF_STRING__( scheduletime ) );
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

        auto appname = request.GetString( __KF_STRING__( appname ) );
        auto apptype = request.GetString( __KF_STRING__( apptype ) );
        auto appid = request.GetUInt32( __KF_STRING__( appid ) );
        auto zoneid = request.GetUInt32( __KF_STRING__( zoneid ) );

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

        auto appname = request.GetString( __KF_STRING__( appname ) );
        auto apptype = request.GetString( __KF_STRING__( apptype ) );
        auto appid = request.GetUInt32( __KF_STRING__( appid ) );
        auto zoneid = request.GetUInt32( __KF_STRING__( zoneid ) );
        auto delaytime = request.GetUInt32( __KF_STRING__( delaytime ) );

        KFMsg::S2SRestartServerToAgentReq req;
        req.set_appname( appname );
        req.set_apptype( apptype );
        req.set_appid( appid );
        req.set_zoneid( zoneid );
        req.set_delaytime( delaytime );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_RESTART_SERVER_TO_AGENT_REQ, &req );

        return _invalid_str;
    }
}