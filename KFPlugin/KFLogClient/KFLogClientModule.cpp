#include "KFLogClientModule.hpp"

namespace KFrame
{
    void KFLogClientModule::BeforeRun()
    {
        __REGISTER_TCP_CLIENT_CONNECTION__( &KFLogClientModule::OnClientConnectLogServer );
    }

    void KFLogClientModule::BeforeShut()
    {
        __UN_TCP_CLIENT_CONNECTION__();
        KFLogger::Instance()->UnRegisterLogFunction( this );
    }

    void KFLogClientModule::PrepareRun()
    {
        auto url = _kf_ip_address->GetLogUrl() + __STRING__( address );

        __JSON_OBJECT_DOCUMENT__( request );
        __JSON_SET_VALUE__( request, __STRING__( appname ), KFGlobal::Instance()->_app_name );
        __JSON_SET_VALUE__( request, __STRING__( apptype ), KFGlobal::Instance()->_app_type );
        __JSON_SET_VALUE__( request, __STRING__( appid ), KFGlobal::Instance()->_app_id->ToString() );

        auto data = __JSON_SERIALIZE__( request );
        auto recv_data = _kf_http_client->STGet( url, _invalid_string );

        __JSON_PARSE_STRING__( response, recv_data );
        auto ret_code = _kf_http_client->GetCode( response );
        if ( ret_code != KFEnum::Ok )
        {
            return __LOG_ERROR__( "request log address error={}", ret_code );
        }

        auto ip = __JSON_GET_STRING__( response, __STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( response, __STRING__( port ) );
        auto app_name = __JSON_GET_STRING__( response, __STRING__( appname ) );
        auto app_type = __JSON_GET_STRING__( response, __STRING__( apptype ) );
        auto app_id = __JSON_GET_UINT64__( response, __STRING__( appid ) );
        _kf_tcp_client->StartClient( app_name, app_type, app_id, ip, port );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFLogClientModule::OnClientConnectLogServer )
    {
        if ( net_data->_name == __STRING__( log ) )
        {
            __LOG_INFO__( "remote log[{}] ok", net_data->_str_id );

            _log_server_id = net_data->_id;
            KFLogger::Instance()->RegisterLogFunction( this, &KFLogClientModule::LogRemote );
        }
    }

    void KFLogClientModule::LogRemote( uint32 level, const std::string& content )
    {
        auto log_data = __MAKE_SHARED__( KFLogData );
        log_data->_level = level;
        log_data->_content = content;

        KFLocker locker( _kf_mutex );
        _log_data_list.push_back( log_data );
    }

    void KFLogClientModule::Run()
    {
        std::list<std::shared_ptr<KFLogData>> temp_list;
        {
            KFLocker locker( _kf_mutex );
            if ( _log_data_list.empty() )
            {
                return;
            }

            temp_list.swap( _log_data_list );
        }

        auto global = KFGlobal::Instance();

        KFMsg::S2SRemoteLogToServerReq req;
        req.set_appname( global->_app_name );
        req.set_apptype( global->_app_type );
        req.set_strappid( global->_app_id->ToString() );
        req.set_appid( global->_app_id->GetId() );
        req.set_zoneid( global->_app_id->GetZoneId() );

        for ( auto log_data : temp_list )
        {
            auto pb_data = req.add_logdata();
            pb_data->set_level( log_data->_level );
            pb_data->set_content( log_data->_content );
        }

        auto ok = _kf_tcp_client->SendNetMessage( _log_server_id, KFMsg::S2S_REMOTE_LOG_TO_SERVER_REQ, &req );
        if ( !ok )
        {
            KFLogger::Instance()->UnRegisterLogFunction( this );
            __LOG_INFO__( "remote log[{}] failed", KFAppId::ToString( _log_server_id ) );
        }
    }
}



