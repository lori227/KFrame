#include "KFLogClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFLogClientModule::BeforeRun()
    {
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFLogClientModule::OnClientConnectLogServer );
    }

    void KFLogClientModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        KFLogger::Instance()->UnRegisterLogFunction( this );
    }

    void KFLogClientModule::OnceRun()
    {
        auto url = _kf_ip_address->GetLogUrl() + __KF_STRING__( address );

        __JSON_DOCUMENT__( request );
        __JSON_SET_VALUE__( request, __KF_STRING__( appname ), KFGlobal::Instance()->_app_name );
        __JSON_SET_VALUE__( request, __KF_STRING__( apptype ), KFGlobal::Instance()->_app_type );
        __JSON_SET_VALUE__( request, __KF_STRING__( appid ), KFGlobal::Instance()->_str_app_id );

        auto data = __JSON_SERIALIZE__( request );
        auto recvdata = _kf_http_client->StartSTClient( url, _invalid_str );

        __JSON_PARSE_STRING__( response, recvdata );
        auto retcode = _kf_http_client->GetResponseCode( response );
        if ( retcode != KFEnum::Ok )
        {
            return __LOG_ERROR__( "request log address error={}!", retcode );
        }

        auto ip = __JSON_GET_STRING__( response, __KF_STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( response, __KF_STRING__( port ) );
        auto appname = __JSON_GET_STRING__( response, __KF_STRING__( appname ) );
        auto apptype = __JSON_GET_STRING__( response, __KF_STRING__( apptype ) );
        auto appid = __JSON_GET_UINT64__( response, __KF_STRING__( appid ) );
        _kf_tcp_client->StartClient( appname, apptype, appid, ip, port );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFLogClientModule::OnClientConnectLogServer )
    {
        if ( servername == __KF_STRING__( log ) )
        {
            _log_server_id = serverid;
            KFLogger::Instance()->RegisterLogFunction( this, &KFLogClientModule::LogRemote );
        }
    }

    void KFLogClientModule::LogRemote( uint32 level, const std::string& content )
    {
        auto kfdata = __KF_NEW__( KFLogData );
        kfdata->_level = level;
        kfdata->_content = content;

        KFLocker locker( _kf_mutex );
        _log_data_list.push_back( kfdata );
    }

    void KFLogClientModule::Run()
    {
        std::list< KFLogData* > templist;
        {
            KFLocker locker( _kf_mutex );
            if ( _log_data_list.empty() )
            {
                return;
            }

            templist.swap( _log_data_list );
        }

        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SRemoteLogToServerReq req;
        req.set_appname( kfglobal->_app_name );
        req.set_apptype( kfglobal->_app_type );
        req.set_strappid( kfglobal->_str_app_id );
        req.set_appid( kfglobal->_app_id._union._id );
        req.set_zoneid( kfglobal->_app_id._union._app_data._zone_id );

        for ( auto kfdata : templist )
        {
            auto pblog = req.add_logdata();
            pblog->set_level( kfdata->_level );
            pblog->set_content( kfdata->_content );

            __KF_DELETE__( KFLogData, kfdata );
        }

        auto ok = _kf_tcp_client->SendNetMessage( _log_server_id, KFMsg::S2S_REMOTE_LOG_TO_SERVER_REQ, &req );
        if ( !ok )
        {
            KFLogger::Instance()->UnRegisterLogFunction( this );
        }
    }
}



