#include "KFLogShardModule.hpp"
#include "KFLogger/KFSpdLog.h"

namespace KFrame
{
    void KFLogShardModule::BeforeRun()
    {
        __REGISTER_HTTP__( __STRING__( address ), false, &KFLogShardModule::HandleRequestLogAddressReq );
        //////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_REMOTE_LOG_TO_SERVER_REQ, &KFLogShardModule::HandleRemoteLogToServerReq );
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

        auto app_name = __JSON_GET_STRING__( request, __STRING__( appname ) );
        auto app_type = __JSON_GET_STRING__( request, __STRING__( apptype ) );
        auto app_id = __JSON_GET_STRING__( request, __STRING__( appid ) );

        __LOG_INFO__( "[{}:{}:{}] request address req", app_name, app_type, app_id );

        auto global = KFGlobal::Instance();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( appname ), global->_app_name );
        __JSON_SET_VALUE__( response, __STRING__( apptype ), global->_app_type );
        __JSON_SET_VALUE__( response, __STRING__( appid ), global->_app_id->GetId() );
        __JSON_SET_VALUE__( response, __STRING__( ip ), global->_internet_ip );
        __JSON_SET_VALUE__( response, __STRING__( port ), global->_listen_port );
        return _kf_http_server->SendResponse( response );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFLogShardModule::HandleRemoteLogToServerReq, KFMsg::S2SRemoteLogToServerReq )
    {
        auto filename = __FORMAT__( "{}-{}-{}.log", kfmsg->appname(), kfmsg->apptype(), kfmsg->strappid() );
        auto spd_log = KFLogger::Instance()->NewLogger( kfmsg->appid(), __STRING__( remote ), filename );
        for ( auto i = 0; i < kfmsg->logdata_size(); ++i )
        {
            auto log_data = &kfmsg->logdata( i );
            spd_log->Log( log_data->level(), log_data->content() );
        }
    }
}