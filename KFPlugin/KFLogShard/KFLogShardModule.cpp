#include "KFLogShardModule.hpp"
#include "KFLogger/KFSpdLog.h"

namespace KFrame
{
    void KFLogShardModule::BeforeRun()
    {
        __REGISTER_HTTP__( __STRING__( address ), false, &KFLogShardModule::HandleRequestLogAddressReq );
        //////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REMOTE_LOG_TO_SERVER_REQ, &KFLogShardModule::HandleRemoteLogToServerReq );
    }

    void KFLogShardModule::BeforeShut()
    {
        __UN_HTTP__( __STRING__( address ) );
        //////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_REMOTE_LOG_TO_SERVER_REQ );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFLogShardModule::HandleRequestLogAddressReq )
    {
        __JSON_PARSE_STRING__( request, data );

        auto appname = __JSON_GET_STRING__( request, __STRING__( appname ) );
        auto apptype = __JSON_GET_STRING__( request, __STRING__( apptype ) );
        auto appid = __JSON_GET_STRING__( request, __STRING__( appid ) );

        __LOG_INFO__( "[{}:{}:{}] request address req", appname, apptype, appid );

        auto kfglobal = KFGlobal::Instance();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( appname ), kfglobal->_app_name );
        __JSON_SET_VALUE__( response, __STRING__( apptype ), kfglobal->_app_type );
        __JSON_SET_VALUE__( response, __STRING__( appid ), kfglobal->_app_id->GetId() );
        __JSON_SET_VALUE__( response, __STRING__( ip ), kfglobal->_interanet_ip );
        __JSON_SET_VALUE__( response, __STRING__( port ), kfglobal->_listen_port );
        return _kf_http_server->SendResponse( response );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLogShardModule::HandleRemoteLogToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRemoteLogToServerReq );

        auto spdlog = KFLogger::Instance()->NewLogger( kfmsg.appid(), __STRING__( remote ), kfmsg.appname(), kfmsg.apptype(), kfmsg.strappid() );
        for ( auto i = 0; i < kfmsg.logdata_size(); ++i )
        {
            auto logdata = &kfmsg.logdata( i );
            spdlog->Log( logdata->level(), logdata->content() );
        }
    }
}