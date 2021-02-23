#include "KFDeployServerModule.hpp"

namespace KFrame
{
    void KFDeployServerModule::BeforeRun()
    {
        __REGISTER_TCP_SERVER_LOST__( &KFDeployServerModule::OnServerLostClient );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP__( __STRING__( deploy ), true, &KFDeployServerModule::HandleDeployCommand );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_REGISTER_AGENT_TO_SERVER_REQ, &KFDeployServerModule::HandleRegisterAgentToServerReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DEPLOY_TOOL_EXECUTE_MYSQL_REQ, &KFDeployServerModule::HandleExecuteMySQLReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DEPLOY_TOOL_DELETE_MYSQL_REQ, &KFDeployServerModule::HandleDeleteMySQLReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DEPLOY_TOOL_QUERY_MYSQL_REQ, &KFDeployServerModule::HandleQueryMySQLReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DEPLOY_TOOL_COMMAND_REQ, &KFDeployServerModule::HandleDeployToolCommandReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DEPLOY_LOG_TO_SERVER_ACK, &KFDeployServerModule::HandleDeployLogToServerAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_DEPLOY_TOOL_QUERY_TOOL_ID_REQ, &KFDeployServerModule::HandleDeployQueryToolIdReq );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDeployServerModule::ShutDown()
    {
        __UN_DELAYED_0__();
        __UN_TCP_SERVER_LOST__();
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
        if ( net_data->_name == __STRING__( deploy ) )
        {
            if ( net_data->_type == __STRING__( agent ) )
            {
                auto agent_data = _agent_list.Find( net_data->_str_id );
                if ( agent_data != nullptr )
                {
                    UpdateAgentToDatabase( agent_data, 0u );
                    _agent_list.Remove( net_data->_str_id );
                }
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleRegisterAgentToServerReq, KFMsg::S2SRegisterAgentToServerReq )
    {
        auto agent_data = _agent_list.Create( kfmsg->agentid() );
        agent_data->_agent_id = kfmsg->agentid();
        agent_data->_local_ip = kfmsg->localip();
        agent_data->_name = kfmsg->name();
        agent_data->_type = kfmsg->type();
        agent_data->_port = kfmsg->port();
        agent_data->_service = kfmsg->service();
        UpdateAgentToDatabase( agent_data, 1u );
    }

    void KFDeployServerModule::UpdateAgentToDatabase( std::shared_ptr<KFAgentData> agent_data, uint32 status )
    {
        StringMap keys;
        keys[ __STRING__( localip ) ] = agent_data->_local_ip;

        StringMap values;
        values[ __STRING__( status ) ] = __TO_STRING__( status );
        values[ __STRING__( port ) ] = __TO_STRING__( agent_data->_port );
        values[ __STRING__( service ) ] = agent_data->_service;
        _mysql_driver->Update( __STRING__( agent ), keys, values );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleDeleteMySQLReq, KFMsg::S2SDeployToolDeleteMySQLReq )
    {
        StringMap values;
        auto pb_keys = kfmsg->keys();
        for ( auto iter = pb_keys.begin(); iter != pb_keys.end(); ++iter )
        {
            values[ iter->first ] = iter->second;
        }

        auto ok = _mysql_driver->Delete( kfmsg->table(), values );
        KFMsg::S2SDeployToolDeleteMySQLAck ack;
        ack.set_result( ok );
        ack.set_table( kfmsg->table() );
        *ack.mutable_keys() = pb_keys;
        _kf_tcp_server->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_DEPLOY_TOOL_DELETE_MYSQL_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleExecuteMySQLReq, KFMsg::S2SDeployToolExecuteMySQLReq )
    {
        StringMap values;
        auto pb_values = &kfmsg->values();
        __PROTO_TO_MAP__( pb_values, values );
        auto ok = _mysql_driver->Insert( kfmsg->table(), values );

        KFMsg::S2SDeployToolExecuteMySQLAck ack;
        ack.set_result( ok );
        ack.set_table( kfmsg->table() );
        *ack.mutable_values() = *pb_values;
        _kf_tcp_server->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_DEPLOY_TOOL_EXECUTE_MYSQL_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleQueryMySQLReq, KFMsg::S2SDeployToolQueryMySQLReq )
    {
        StringMap keys;
        auto pb_keys = &kfmsg->keys();
        __PROTO_TO_MAP__( pb_keys, keys );
        auto result = _mysql_driver->Select( kfmsg->table(), keys );

        KFMsg::S2SDeployToolQueryMySQLAck ack;
        ack.set_result( result->IsOk() );
        ack.set_table( kfmsg->table() );

        if ( result->IsOk() )
        {
            auto pb_data_list = ack.mutable_datas();
            for ( auto iter = result->_value.begin(); iter != result->_value.end(); ++iter )
            {
                auto pb_data = pb_data_list->add_data();
                auto& pb_values = *pb_data->mutable_values();
                for ( auto miter = iter->begin(); miter != iter->end(); ++miter )
                {
                    pb_values[ miter->first ] = miter->second;
                }
            }
        }

        _kf_tcp_server->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_DEPLOY_TOOL_QUERY_MYSQL_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleDeployQueryToolIdReq, KFMsg::S2SDeployToolQueryToolIdReq )
    {
        static const char* _sql = "select `id` from `tool` where `toolip`='{}';";

        // 查询id
        auto query_id_result = _mysql_driver->QueryUInt32( _sql, kfmsg->ip() );
        if ( query_id_result->IsOk() )
        {
            auto tool_id = 0u;
            if ( query_id_result->_value == 0u )
            {
                StringMap values;
                values[ "toolip" ] = kfmsg->ip();
                _mysql_driver->Insert( "tool", values );

                auto result = _mysql_driver->QueryUInt32( _sql, kfmsg->ip() );
                if ( result->_value != 0u )
                {
                    tool_id = result->_value;
                }
            }
            else
            {
                tool_id = query_id_result->_value;
            }

            if ( tool_id != 0u )
            {
                KFMsg::S2SDeployToolQueryToolIdAck ack;
                ack.set_id( tool_id );
                _kf_tcp_server->SendNetMessage( __ROUTE_SERVER_ID__, KFMsg::S2S_DEPLOY_TOOL_QUERY_TOOL_ID_ACK, &ack );
            }
        }

        _kf_tcp_server->CloseNetHandle( __ROUTE_SERVER_ID__, 1000, __FUNC_LINE__ );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleDeployLogToServerAck, KFMsg::S2SDeployLogToServerAck )
    {
        LogDeploy( kfmsg->agentid(), "{}", kfmsg->content() );
    }

    __KF_MESSAGE_FUNCTION__( KFDeployServerModule::HandleDeployToolCommandReq, KFMsg::S2SDeployToolCommandReq )
    {
        auto pb_command = &kfmsg->deploycommand();

        LogDeploy( 0, "deploy tool=[{}|{}] req", kfmsg->toolid(), kfmsg->ip() );
        LogDeploy( 0, "recv=[{}:{} | {}:{}:{}:{}]",
                   pb_command->command(), pb_command->value(),
                   pb_command->appname(), pb_command->apptype(), pb_command->zoneid(), pb_command->appid() );

        if ( pb_command->command() == __STRING__( unschedule ) )
        {
            auto id = __TO_UINT64__( pb_command->value() );
            __UN_DELAYED_1__( id );
        }
        else
        {
            auto data = kfmsg->SerializeAsString();
            if ( kfmsg->time() <= KFGlobal::Instance()->_real_time )
            {
                OnTcpDeployCommandToAgent( _invalid_int, data.data(), data.length() );
            }
            else
            {
                LogDeploy( 0, "schedule=[{}] time=[{}]", _delayed_id, KFDate::GetTimeString( kfmsg->time() ) );
                __REGISTER_DELAYED_WITH_DATA__( kfmsg->time(), ++_delayed_id, data.data(), data.length(), &KFDeployServerModule::OnTcpDeployCommandToAgent );
            }
        }
    }

    __KF_DELAYED_FUNCTION__( KFDeployServerModule::OnTcpDeployCommandToAgent )
    {
        auto msg = __MAKE_SHARED__( KFMsg::S2SDeployToolCommandReq );
        if ( KFProto::Parse( msg, data, size ) )
        {
            return;
        }

        KFMsg::S2SDeployCommandToAgentReq req;
        req.mutable_deploycommand()->CopyFrom( msg->deploycommand() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &req );

        auto pb_command = &msg->deploycommand();
        LogDeploy( 0, "distribute=[{}:{}]", pb_command->command(), pb_command->value() );
    }

    __KF_HTTP_FUNCTION__( KFDeployServerModule::HandleDeployCommand )
    {
        __JSON_PARSE_STRING__( request, data );

        _web_deploy_url.clear();
        auto log_url = __JSON_GET_STRING__( request, __STRING__( logurl ) );
        if ( !log_url.empty() )
        {
            _web_deploy_url = log_url;
        }

        LogDeploy( 0, "web deploy=[{}] command req", ip );
        LogDeploy( 0, "recv=[{}]", data );

        auto command = __JSON_GET_STRING__( request, __STRING__( command ) );
        if ( command == __STRING__( unschedule ) )
        {
            auto id = __JSON_GET_UINT64__( request, __STRING__( value ) );
            __UN_DELAYED_1__( id );
        }
        else
        {
            auto schedule_time = __JSON_GET_UINT32__( request, __STRING__( scheduletime ) );
            if ( schedule_time <= KFGlobal::Instance()->_real_time )
            {
                OnHttpDeployCommandToAgent( _invalid_int, data.c_str(), data.size() );
            }
            else
            {
                LogDeploy( 0, "schedule=[{}] time=[{}]", _delayed_id, KFDate::GetTimeString( schedule_time ) );
                __REGISTER_DELAYED_WITH_DATA__( schedule_time, ++_delayed_id, data.c_str(), data.size(), &KFDeployServerModule::OnHttpDeployCommandToAgent );
            }
        }

        return _invalid_string;
    }

    __KF_DELAYED_FUNCTION__( KFDeployServerModule::OnHttpDeployCommandToAgent )
    {
        __JSON_PARSE_CHAR__( request, data, size );

        KFMsg::S2SDeployCommandToAgentReq req;
        auto pb_deploy = req.mutable_deploycommand();
        pb_deploy->set_command( __JSON_GET_STRING__( request, __STRING__( command ) ) );
        pb_deploy->set_value( __JSON_GET_STRING__( request, __STRING__( value ) ) );
        pb_deploy->set_appname( __JSON_GET_STRING__( request, __STRING__( appname ) ) );
        pb_deploy->set_apptype( __JSON_GET_STRING__( request, __STRING__( apptype ) ) );
        pb_deploy->set_appid( __JSON_GET_STRING__( request, __STRING__( appid ) ) );
        pb_deploy->set_zoneid( __JSON_GET_UINT32__( request, __STRING__( zoneid ) ) );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_DEPLOY_COMMAND_TO_AGENT_REQ, &req );

        LogDeploy( 0, "distribute=[{}]", data );
    }

    void KFDeployServerModule::SendLogMessage( uint64 agent_id, const std::string& msg )
    {
        __LOG_INFO__( "{}", msg );

        // 广播给所有工具
        KFMsg::S2SDeployLogToToolAck ack;
        ack.set_agentid( agent_id );
        ack.set_content( msg );
        _kf_tcp_server->SendMessageToType( __STRING__( tool ), KFMsg::S2S_DEPLOY_LOG_TO_TOOL_ACK, &ack );

        // 广播给所有web工具
        if ( !_web_deploy_url.empty() )
        {
            __JSON_OBJECT_DOCUMENT__( response );
            __JSON_SET_VALUE__( response, __STRING__( msg ), msg );
            __JSON_SET_VALUE__( response, __STRING__( agent ), agent_id );
            _kf_http_client->MTGet< KFDeployServerModule >( _web_deploy_url, response );
        }
    }
}