#include "KFLogShardModule.h"

namespace KFrame
{
    KFLogShardModule::~KFLogShardModule()
    {
        for ( auto iter : _log_list )
        {
            delete iter.second;
        }
        _log_list.clear();
    }

    void KFLogShardModule::BeforeRun()
    {
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( address ), false, &KFLogShardModule::HandleRequestLogAddressReq );
        //////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REMOTE_LOG_TO_SERVER_REQ, &KFLogShardModule::HandleRemoteLogToServerReq );
    }

    void KFLogShardModule::BeforeShut()
    {
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( address ) );
        //////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REMOTE_LOG_TO_SERVER_REQ );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFLogShardModule::HandleRequestLogAddressReq )
    {
        __JSON_PARSE_STRING__( request, data );

        auto appname = __JSON_GET_STRING__( request, __KF_STRING__( appname ) );
        auto apptype = __JSON_GET_STRING__( request, __KF_STRING__( apptype ) );
        auto appid = __JSON_GET_STRING__( request, __KF_STRING__( appid ) );

        __LOG_INFO__( "[{}:{}:{}] request address req!", appname, apptype, appid );

        auto kfglobal = KFGlobal::Instance();

        __JSON_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( appname ), kfglobal->_app_name );
        __JSON_SET_VALUE__( response, __KF_STRING__( apptype ), kfglobal->_app_type );
        __JSON_SET_VALUE__( response, __KF_STRING__( appid ), kfglobal->_app_id._union._id );
        __JSON_SET_VALUE__( response, __KF_STRING__( ip ), kfglobal->_interanet_ip );
        __JSON_SET_VALUE__( response, __KF_STRING__( port ), kfglobal->_listen_port );
        return _kf_http_server->SendResponse( response );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFSpdLog* KFLogShardModule::FindRemoteLog( uint64 appid, const std::string& appname, const std::string& apptype, const std::string& strappid )
    {

        auto iter = _log_list.find( appid );
        if ( iter == _log_list.end() )
        {
            auto spdlog = new KFSpdLog();
            std::string path = __FORMAT__( "..{}binlog", spdlog::details::os::folder_sep );
            spdlog->Initialize( path, appname, apptype, strappid );
            iter = _log_list.insert( std::make_pair( appid, spdlog ) ).first;
        }

        return iter->second;
    }

    //////////////////////////////////////////////////////////////////////////

    __KF_MESSAGE_FUNCTION__( KFLogShardModule::HandleRemoteLogToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRemoteLogToServerReq );

        auto spdlog = FindRemoteLog( kfmsg.appid(), kfmsg.appname(), kfmsg.apptype(), kfmsg.strappid() );
        for ( auto i = 0; i < kfmsg.logdata_size(); ++i )
        {
            auto logdata = &kfmsg.logdata( i );
            spdlog->Log( logdata->level(), logdata->content() );
        }
    }
}