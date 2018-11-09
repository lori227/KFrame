#include "KFPlatformModule.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFPlatformModule::BeforeRun()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_BIND_ALIPAY_REQ, &KFPlatformModule::HandleBindAlipayReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_INVITE_SCORE_REQ, &KFPlatformModule::HandleQueryInviteScoreReq );
    }

    void KFPlatformModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_BIND_ALIPAY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_INVITE_SCORE_REQ );
    }

    const std::string& KFPlatformModule::GetPlatformApiUrl()
    {
        static auto kfoption = _kf_option->FindOption( __KF_STRING__( platapiurl ) );
        return kfoption->_str_value;
    }

    const std::string& KFPlatformModule::MakePlatformSign( uint32 nowtime )
    {
        static std::string _sign = "";
        static auto kfoption = _kf_option->FindOption( __KF_STRING__( platappkey ) );

        auto source = __FORMAT__( "{}{}", kfoption->_str_value, nowtime );
        _sign = KFCrypto::Md5Encode( source );

        return _sign;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPlatformModule::HandleBindAlipayReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgBindAlipayReq );

        static auto _api_url = GetPlatformApiUrl() + "";

    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFPlatformModule::OnBindAlipayCallBack )
    {

    }

    __KF_MESSAGE_FUNCTION__( KFPlatformModule::HandleQueryInviteScoreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryInviteScoreReq );

        static auto _api_url = GetPlatformApiUrl() + "";


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

        KFJson recvjson( recvdata );
        auto code = recvjson.GetUInt32( __KF_STRING__( code ) );
        if ( code != 0 )
        {
            return _kf_display->SendToClient( player, KFMsg::QueryInviteScoreError );
        }

        auto score = recvjson.GetUInt32( __KF_STRING__( score ) );


        KFMsg::MsgQueryInviteScoreAck ack;
        ack.set_score( score );
        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_INVITE_SCORE_ACK, &ack );
    }
}