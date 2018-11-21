#include "KFPlatformModule.h"
#include "KFJson.h"
#include "KFHttp/KFHttpURL.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFPlatformModule::BeforeRun()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_BIND_ALIPAY_REQ, &KFPlatformModule::HandleBindAlipayReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_INVITE_SCORE_REQ, &KFPlatformModule::HandleQueryInviteScoreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_INVITE_SCORE_REQ, &KFPlatformModule::HandleExchangeInviteScoreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_INVITE_RECORD_REQ, &KFPlatformModule::HandleExChangeInviteRecordReq );
    }

    void KFPlatformModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_BIND_ALIPAY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_INVITE_SCORE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_INVITE_SCORE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_INVITE_RECORD_REQ );
    }

    const std::string& KFPlatformModule::GetPlatformApiUrl()
    {
        static auto kfoption = _kf_option->FindOption( __KF_STRING__( platapiurl ) );
        return kfoption->_str_value;
    }

    const std::string& KFPlatformModule::MakePlatformSign( uint32 nowtime )
    {
        static std::string _sign = "";
        static auto _app_key = _kf_option->GetString( __KF_STRING__( platappkey ) );

        auto source = __FORMAT__( "{}{}", _app_key, nowtime );
        _sign = KFCrypto::Md5Encode( source );

        return _sign;
    }

    const std::string& KFPlatformModule::MakePlatformUrl( const std::string& path )
    {
        static auto _api_url = GetPlatformApiUrl();
        static auto _app_id = _kf_option->GetString( __KF_STRING__( platappid ) );

        MapString params;
        params[ "appid" ] = _app_id;
        params[ "timestamp" ] = KFUtility::ToString( KFGlobal::Instance()->_real_time );
        params[ "sig" ] = MakePlatformSign( KFGlobal::Instance()->_real_time );

        static std::string _total_url = "";
        _total_url = _api_url + path + KFHttpURL::EncodeParams( params );
        return _total_url;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPlatformModule::HandleBindAlipayReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgBindAlipayReq );

        auto apiurl = MakePlatformUrl( "bindAlipay" );

        KFJson sendjson;
        sendjson.SetValue( __KF_STRING__( accountid ), playerid );
        sendjson.SetValue( __KF_STRING__( channel ), KFGlobal::Instance()->_app_channel );
        sendjson.SetValue( __KF_STRING__( name ), kfmsg.name() );
        sendjson.SetValue( __KF_STRING__( alipay ), kfmsg.alipay() );
        _kf_http_client->StartMTHttpClient( this, &KFPlatformModule::OnBindAlipayCallBack, apiurl, sendjson );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFPlatformModule::OnBindAlipayCallBack )
    {
        KFJson sendjson( senddata );
        auto playerid = sendjson.GetUInt32( __KF_STRING__( accountid ) );
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        if ( recvdata.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::PlatformServerBusy );
        }

        KFJson recvjson( recvdata );
        auto code = recvjson.GetUInt32( __KF_STRING__( code ) );
        if ( code == 0 )
        {
            _kf_display->SendToClient( player, KFMsg::BindAlipayOk );
        }
        else
        {
            _kf_display->SendToClient( player, KFMsg::BindAlipayFailed, code );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPlatformModule::HandleQueryInviteScoreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryInviteScoreReq );

        auto apiurl = MakePlatformUrl( "getIntegralInfo" );

        KFJson sendjson;
        sendjson.SetValue( __KF_STRING__( accountid ), playerid );
        sendjson.SetValue( __KF_STRING__( channel ), KFGlobal::Instance()->_app_channel );
        _kf_http_client->StartMTHttpClient( this, &KFPlatformModule::OnQueryInviteScoreCallBack, apiurl, sendjson );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFPlatformModule::OnQueryInviteScoreCallBack )
    {
        KFJson sendjson( senddata );
        auto playerid = sendjson.GetUInt32( __KF_STRING__( accountid ) );
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        if ( recvdata.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::PlatformServerBusy );
        }

        KFJson recvjson( recvdata );
        auto code = recvjson.GetUInt32( __KF_STRING__( code ) );
        if ( code != 0 )
        {
            return _kf_display->SendToClient( player, KFMsg::QueryInviteScoreError, code );
        }

        KFJson kfdata = recvjson[ __KF_STRING__( data ) ];
        auto leftscore = kfdata.GetUInt32( "cur_int" );
        auto exchangescore = kfdata.GetUInt32( "pre_int" );

        KFMsg::MsgQueryInviteScoreAck ack;
        ack.set_leftscore( leftscore );
        ack.set_exchangescore( exchangescore );
        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_INVITE_SCORE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPlatformModule::HandleExchangeInviteScoreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExChangeInviteScoreReq );

        // 防止操作太快
        if ( _kf_player->CheckOperateFrequently( player, 3000 ) )
        {
            return;
        }

        auto apiurl = MakePlatformUrl( "integralCash" );

        KFJson sendjson;
        sendjson.SetValue( __KF_STRING__( accountid ), playerid );
        sendjson.SetValue( __KF_STRING__( name ), kfmsg.name() );
        sendjson.SetValue( __KF_STRING__( alipay ), kfmsg.alipay() );
        sendjson.SetValue( __KF_STRING__( channel ), KFGlobal::Instance()->_app_channel );
        _kf_http_client->StartMTHttpClient( this, &KFPlatformModule::OnExchangeInviteScoreCallBack, apiurl, sendjson );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFPlatformModule::OnExchangeInviteScoreCallBack )
    {
        KFJson sendjson( senddata );
        auto playerid = sendjson.GetUInt32( __KF_STRING__( accountid ) );
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        if ( recvdata.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::PlatformServerBusy );
        }

        KFJson recvjson( recvdata );
        auto code = recvjson.GetUInt32( __KF_STRING__( code ) );
        if ( code != 0 )
        {
            return _kf_display->SendToClient( player, KFMsg::ExchangeInviteScoreError, code );
        }

        KFJson kfdata = recvjson[ __KF_STRING__( data ) ];
        auto leftscore = kfdata.GetUInt32( "remain" );
        auto exchangescore = kfdata.GetUInt32( "this_int" );

        KFMsg::MsgExChangeInviteScoreAck ack;
        ack.set_leftscore( leftscore );
        ack.set_exchangescore( exchangescore );
        _kf_player->SendToClient( player, KFMsg::MSG_EXCHANGE_INVITE_SCORE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPlatformModule::HandleExChangeInviteRecordReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExChangeInviteRecordReq );

        // 防止操作太快
        if ( _kf_player->CheckOperateFrequently( player, 5000 ) )
        {
            return;
        }

        auto apiurl = MakePlatformUrl( "integralCashRecord" );

        KFJson sendjson;
        sendjson.SetValue( __KF_STRING__( accountid ), playerid );
        sendjson.SetValue( __KF_STRING__( channel ), KFGlobal::Instance()->_app_channel );
        _kf_http_client->StartMTHttpClient( this, &KFPlatformModule::OnExChangeInviteRecordCallBack, apiurl, sendjson );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFPlatformModule::OnExChangeInviteRecordCallBack )
    {
        KFJson sendjson( senddata );
        auto playerid = sendjson.GetUInt32( __KF_STRING__( accountid ) );
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        if ( recvdata.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::PlatformServerBusy );
        }

        KFJson recvjson( recvdata );
        auto code = recvjson.GetUInt32( __KF_STRING__( code ) );
        if ( code != 0 )
        {
            return _kf_display->SendToClient( player, KFMsg::ExchangeInviteRecordError, code );
        }

        KFMsg::MsgExChangeInviteRecordAck ack;

        KFJson kfdata = recvjson[ __KF_STRING__( data ) ];
        for ( auto iter = kfdata.begin(); iter != kfdata.end(); ++iter )
        {
            KFJson kfrecord = *iter;

            auto pbrecord = ack.add_record();
            pbrecord->set_name( kfrecord.GetString( __KF_STRING__( name ) ) );
            pbrecord->set_alipay( kfrecord.GetString( __KF_STRING__( alipay ) ) );
            pbrecord->set_score( kfrecord.GetUInt32( __KF_STRING__( money ) ) );
            pbrecord->set_time( kfrecord.GetUInt32( __KF_STRING__( time ) ) );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_EXCHANGE_INVITE_RECORD_ACK, &ack );
    }
}