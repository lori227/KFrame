#include "KFDeployServerModule.hpp"

namespace KFrame
{
    void KFDeployServerModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST__( &KFDeployServerModule::OnServerLostClient );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP__( __STRING__( deploy ), true, &KFDeployServerModule::HandleDeployCommand );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &KFDeployServerModule::HandleRegisterAgentToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_EXECUTE_MYSQL_REQ, &KFDeployServerModule::HandleExecuteMySQLReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_DELETE_MYSQL_REQ, &KFDeployServerModule::HandleDeleteMySQLReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_QUERY_MYSQL_REQ, &KFDeployServerModule::HandleQueryMySQLReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_COMMAND_REQ, &KFDeployServerModule::HandleDeployToolCommandReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_LOG_TO_SERVER_ACK, &KFDeployServerModule::HandleDeployLogToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_QUERY_TOOL_ID_REQ, &KFDeployServerModule::HandleDeployQueryToolIdReq );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDeployServerModule::ShutDown()
    {
        __UN_SCHEDULE__();
        __UN_SERVER_LOST__();
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_HTTP__( __STRING__( deploy ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_EXECUTE_MYSQL_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_QUERY_MYSQL_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_DELETE_MYSQL_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_COMMAND_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEPLOY_LOG_TO_SERVER_ACK );
        __UN_MESSAGE__( KFMsg::S2S_DEPLOY_TOOL_QUERY_TOOL_ID_REQ );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDeployServerModule::PrepareRun()
    {
        _mysql_driver = _kf_mysql->Create( __STRING__( deploy ) );
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFDeployServerModule::OnServerLostClient )
    {
        if ( netdata->_name == __STRING__( deploy ) )
        {
            if ( netdata->_type == __STRING__( agent ) )
            {
                auto kfagent = _agent_list.Find( netdata->_str_id );
                if ( kfagent != nullptr )
                {
                    UpdateAgentToDatabase( kfagent, 0u );
                    _agent_list.Remove( netdata->_str_id );
                }
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
        kfagentdata->_service = kfmsg.service();

        UpdateAgentToDatabase( kfagentdata, 1u );
    }

    void KFDeployServerModule::UpdateAgentToDatabase( KFAgentData* kfagent, uint32 status )
    {
        StringMap keyvalue;
        keyvalue[ __STRING__( localip ) ] = kfagent->_local_ip;

        StringMap updatevalue;
        updatevalue[ __STRING__( status ) ] = __TO_STRING__( status );
        updatevalue[ __STRING__( port ) ] = __TO_STRING__( kfagent->_port );
        updatevalue[ __STRING__( service ) ] = kfagent->_service;
        _mysql_driver->Update( __STRING__( agent ), keyvalue, updatevalue );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleDeleteMySQLReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployToolDeleteMySQLReq );

        StringMap values;
        auto pbkeys = kfmsg.keys();
        for ( auto iter = pbkeys.begin(); iter != pbkeys.end(); ++iter )
        {
            values[ iter->first ] = iter->second;
        }

        auto ok = _mysql_driver->Delete( kfmsg.table(), values );
        KFMsg::S2SDeployToolDeleteMySQLAck ack;
        ack.set_result( ok );
        ack.set_table( kfmsg.table() );
        *ack.mutable_keys() = pbkeys;
        _kf_tcp_server->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_DEPLOY_TOOL_DELETE_MYSQL_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleExecuteMySQLReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployToolExecuteMySQLReq );

        StringMap values;
        auto pbvalues = kfmsg.values();
        for ( auto iter = pbvalues.begin(); iter != pbvalues.end(); ++iter )
        {
            values[ iter->first ] = iter->second;
        }

        auto ok = _mysql_driver->Insert( kfmsg.table(), values );

        KFMsg::S2SDeployToolExecuteMySQLAck ack;
        ack.set_result( ok );
        ack.set_table( kfmsg.table() );
        *ack.mutable_values() = pbvalues;
        _kf_tcp_server->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_DEPLOY_TOOL_EXECUTE_MYSQL_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleQueryMySQLReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployToolQueryMySQLReq );

        StringMap keys;
        auto pbkeys = kfmsg.keys();
        for ( auto iter = pbkeys.begin(); iter != pbkeys.end(); ++iter )
        {
            keys[ iter->first ] = iter->second;
        }

        auto kfresult = _mysql_driver->Select( kfmsg.table(), keys );

        KFMsg::S2SDeployToolQueryMySQLAck ack;
        ack.set_result( kfresult->IsOk() );
        ack.set_table( kfmsg.table() );

        if ( kfresult->IsOk() )
        {
            auto pbdatas = ack.mutable_datas();
            for ( auto iter = kfresult->_value.begin(); iter != kfresult->_value.end(); ++iter )
            {
                auto pbdata = pbdatas->add_data();
                auto& pbvalues = *pbdata->mutable_values();
                for ( auto miter  = iter->begin(); miter != iter->end(); ++miter )
                {
                    pbvalues[ miter->first ] = miter->second;
                }
            }
        }

        _kf_tcp_server->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_DEPLOY_TOOL_QUERY_MYSQL_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleDeployQueryToolIdReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployToolQueryToolIdReq );

        static const char* _sql = "select `id` from `tool` where `toolip`='{}';";

        // 查询id
        auto kfqueryid = _mysql_driver->QueryUInt32( _sql, kfmsg.ip() );
        if ( kfqueryid->IsOk() )
        {
            auto toolid = 0u;
            if ( kfqueryid->_value == 0u )
            {
                StringMap values;
                values[ "toolip" ] = kfmsg.ip();
                _mysql_driver->Insert( "tool", values );

                auto kfresult = _mysql_driver->QueryUInt32( _sql, kfmsg.ip() );
                if ( kfresult->_value != 0u )
                {
                    toolid = kfresult->_value;
                }
            }
            else
            {
                toolid = kfqueryid->_value;
            }

            if ( toolid != 0u )
            {
                KFMsg::S2SDeployToolQueryToolIdAck ack;
                ack.set_id( toolid );
                _kf_tcp_server->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_DEPLOY_TOOL_QUERY_TOOL_ID_ACK, &ack );
            }
        }

        _kf_tcp_server->CloseNetHandle( __ROUTE_SERVER_ID__, 1000, __FUNC_LINE__ );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleDeployLogToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployLogToServerAck );

        LogDeploy( kfmsg.agentid(), "{}", kfmsg.content() );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleDeployToolCommandReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeployToolCommandReq );

        auto pbcommand = &kfmsg.deploycommand();

        LogDeploy( 0, "deploy tool=[{}|{}] req!", kfmsg.toolid(), kfmsg.ip() );
        LogDeploy( 0, "recv=[{}:{} | {}:{}:{}:{}]",
                   pbcommand->command(), pbcommand->value(),
                   pbcommand->appname(), pbcommand->apptype(), pbcommand->zoneid(), pbcommand->appid() );

        if ( pbcommand->command() == __STRING__( unschedule ) )
        {
            auto id = KFUtility::ToValue< uint64 >( pbcommand->value() );
            __UN_SCHEDULE_OBJECT__( id );
        }
        else
        {
            if ( kfmsg.time() <= KFGlobal::Instance()->_real_time )
            {
                OnTcpDeployCommandToAgent( _invalid_int, data, length );
            }
            else
            {
                auto kfsetting = _kf_schedule->CreateScheduleSetting();
                kfsetting->SetTime( kfmsg.time() );
                kfsetting->SetData( ++_schedule_id, data, length );
                __REGISTER_SCHEDULE__( kfsetting, &KFDeployServerModule::OnTcpDeployCommandToAgent );

                LogDeploy( 0, "schedule=[{}] time=[{}]", _schedule_id, KFDate::GetTimeString( kfmsg.time() ) );
            }
        }
    }

    __KF_SCHEDULE_FUNCTION__( KFDeployServerModule::OnTcpDeployCommandToAgent )
    {
        auto length = size;
        __PROTO_PARSE__( KFMsg::S2SDeployToolCommandReq );

        auto pbcommand = &kfmsg.deploycommand();

        KFMsg::S2SDeployCommandToAgentReq req;
        req.mutable_deploycommand()->CopyFrom( kfmsg.deploycommand() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &req );

        LogDeploy( 0, "distribute=[{}:{}]", pbcommand->command(), pbcommand->value() );
    }

    __KF_HTTP_FUNCTION__( KFDeployServerModule::HandleDeployCommand )
    {
        __JSON_PARSE_STRING__( request, data );

        _web_deploy_url.clear();
        auto logurl = __JSON_GET_STRING__( request, __STRING__( logurl ) );
        if ( !logurl.empty() )
        {
            _web_deploy_url = logurl;
        }

        LogDeploy( 0, "web deploy=[{}] command req!", ip );
        LogDeploy( 0, "recv=[{}]", data );

        auto command = __JSON_GET_STRING__( request, __STRING__( command ) );
        if ( command == __STRING__( unschedule ) )
        {
            auto id = __JSON_GET_UINT64__( request, __STRING__( value ) );
            __UN_SCHEDULE_OBJECT__( id );
        }
        else
        {
            auto scheduletime = __JSON_GET_UINT32__( request, __STRING__( scheduletime ) );
            if ( scheduletime <= KFGlobal::Instance()->_real_time )
            {
                OnHttpDeployCommandToAgent( _invalid_int, data.c_str(), data.size() );
            }
            else
            {
                auto kfsetting = _kf_schedule->CreateScheduleSetting();
                kfsetting->SetTime( scheduletime );
                kfsetting->SetData( ++_schedule_id, data.c_str(), data.size() );
                __REGISTER_SCHEDULE__( kfsetting, &KFDeployServerModule::OnHttpDeployCommandToAgent );

                LogDeploy( 0, "schedule=[{}] time=[{}]", _schedule_id, KFDate::GetTimeString( scheduletime ) );
            }
        }

        return _invalid_string;
    }

    __KF_SCHEDULE_FUNCTION__( KFDeployServerModule::OnHttpDeployCommandToAgent )
    {
        __JSON_PARSE_CHAR__( request, data, size );

        KFMsg::S2SDeployCommandToAgentReq req;
        auto pbdeploy = req.mutable_deploycommand();
        pbdeploy->set_command( __JSON_GET_STRING__( request, __STRING__( command ) ) );
        pbdeploy->set_value( __JSON_GET_STRING__( request, __STRING__( value ) ) );
        pbdeploy->set_appname( __JSON_GET_STRING__( request, __STRING__( appname ) ) );
        pbdeploy->set_apptype( __JSON_GET_STRING__( request, __STRING__( apptype ) ) );
        pbdeploy->set_appid( __JSON_GET_STRING__( request, __STRING__( appid ) ) );
        pbdeploy->set_zoneid( __JSON_GET_UINT32__( request, __STRING__( zoneid ) ) );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &req );

        LogDeploy( 0, "distribute=[{}]", data );
    }

    void KFDeployServerModule::SendLogMessage( uint64 agentid, const std::string& msg )
    {
        __LOG_DEBUG__( "{}", msg );

        // 广播给所有工具
        KFMsg::S2SDeployLogToToolAck ack;
        ack.set_agentid( agentid );
        ack.set_content( msg );
        _kf_tcp_server->SendMessageToType( __STRING__( tool ), KFMsg::S2S_DEPLOY_LOG_TO_TOOL_ACK, &ack );

        // 广播给所有web工具
        if ( !_web_deploy_url.empty() )
        {
            __JSON_OBJECT_DOCUMENT__( response );
            __JSON_SET_VALUE__( response, __STRING__( msg ), msg );
            __JSON_SET_VALUE__( response, __STRING__( agent ), agentid );
            _kf_http_client->MTGet< KFDeployServerModule >( _web_deploy_url, response );
        }
    }
}