#include "KFDeployServerModule.h"
#include "KFProtocol/KFProtocol.h"
#include "KFJson.h"


namespace KFrame
{
    void KFDeployServerModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFDeployServerModule::OnServerLostClient );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( deploy ), true, &KFDeployServerModule::HandleDeployCommand );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &KFDeployServerModule::HandleRegisterAgentToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_REQ, &KFDeployServerModule::HandleGetAgentIpAddressReq );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDeployServerModule::ShutDown()
    {
        __UNREGISTER_SCHEDULE_FUNCTION__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( deploy ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_GET_AGENT_IP_ADDRESS_REQ );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDeployServerModule::OnceRun()
    {
        _mysql_driver = _kf_mysql->CreateExecute( __KF_STRING__( deploy ) );
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFDeployServerModule::OnServerLostClient )
    {
        if ( handlename == __KF_STRING__( deploy ) )
        {
            auto strhandleid = KFAppID::ToString( handleid );

            auto kfagent = _agent_list.Find( strhandleid );
            if ( kfagent != nullptr )
            {
                UpdateAgentToDatabase( kfagent, 0 );
                _agent_list.Remove( strhandleid );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleRegisterAgentToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterAgentToServerReq );

        auto kfagentdata = _agent_list.Create( kfmsg.agentid() );
        kfagentdata->_agent_id = kfmsg.agentid();
        kfagentdata->_local_ip = kfmsg.localip();
        kfagentdata->_name = kfmsg.name();
        kfagentdata->_type = kfmsg.type();
        kfagentdata->_port = kfmsg.port();

        UpdateAgentToDatabase( kfagentdata, 1u );
    }

    void KFDeployServerModule::UpdateAgentToDatabase( KFAgentData* kfagent, uint32 status )
    {
        MapString keyvalue;
        keyvalue[ __KF_STRING__( localip ) ] = kfagent->_local_ip;

        MapString updatevalue;
        updatevalue[ __KF_STRING__( agentid ) ] = kfagent->_agent_id;
        updatevalue[ __KF_STRING__( status ) ] = __TO_STRING__( status );
        updatevalue[ __KF_STRING__( port ) ] = __TO_STRING__( kfagent->_port );
        _mysql_driver->Update( __KF_STRING__( machine ), keyvalue, updatevalue );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleGetAgentIpAddressReq )
    {
        __PROTO_PARSE__( KFMsg::S2SGetAgentIpAddressReq );

        auto clientid = __KF_HEAD_ID__( kfid );
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

    __KF_HTTP_FUNCTION__( KFDeployServerModule::HandleDeployCommand )
    {
        __JSON_PARSE_STRING__( request, data );

        auto logurl = __JSON_GET_STRING__( request, __KF_STRING__( callback ) );
        auto scheduletime = __JSON_GET_UINT32__( request, __KF_STRING__( scheduletime ) );

        LogDeploy( logurl, "server recv command=[{}]", data );

        if ( scheduletime == _invalid_int || scheduletime <= KFGlobal::Instance()->_real_time )
        {
            OnDeployCommandToAgent( _invalid_int, data.c_str(), data.size() );
        }
        else
        {
            auto kfsetting = _kf_schedule->CreateScheduleSetting();
            kfsetting->SetTime( scheduletime );
            kfsetting->SetData( _invalid_int, data.c_str(), data.size() );
            __REGISTER_SCHEDULE_FUNCTION__( kfsetting, &KFDeployServerModule::OnDeployCommandToAgent );

            LogDeploy( logurl, "start schedule at [{}]", KFDate::GetTimeString( scheduletime ) );
        }

        return _invalid_str;
    }

    __KF_SCHEDULE_FUNCTION__( KFDeployServerModule::OnDeployCommandToAgent )
    {
        __JSON_PARSE_CHAR__( request, data, size );

        KFMsg::S2SDeployCommandToAgentReq req;
        auto pbdeploy = req.mutable_deploycommand();
        pbdeploy->set_command( __JSON_GET_STRING__( request, __KF_STRING__( command ) ) );
        pbdeploy->set_value( __JSON_GET_STRING__( request, __KF_STRING__( value ) ) );
        pbdeploy->set_appname( __JSON_GET_STRING__( request, __KF_STRING__( appname ) ) );
        pbdeploy->set_apptype( __JSON_GET_STRING__( request, __KF_STRING__( apptype ) ) );
        pbdeploy->set_appid( __JSON_GET_STRING__( request, __KF_STRING__( appid ) ) );
        pbdeploy->set_zoneid( __JSON_GET_UINT32__( request, __KF_STRING__( zoneid ) ) );
        pbdeploy->set_logurl( __JSON_GET_STRING__( request, __KF_STRING__( callback ) ) );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &req );

        auto logurl = __JSON_GET_STRING__( request, __KF_STRING__( callback ) );
        LogDeploy( logurl, "server distribute command=[{}]", data );
    }

    void KFDeployServerModule::SendLogMessage( const std::string& url, const std::string& msg )
    {
        __LOG_DEBUG__( "{}", msg );

        if ( !url.empty() )
        {
            __JSON_DOCUMENT__( response );
            __JSON_SET_VALUE__( response, __KF_STRING__( msg ), msg );
            _kf_http_client->StartMTHttpClient( url, response );
        }
    }
}