#include "KFGiftModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFGiftModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_BIND_ALIPAY_REQ, &KFGiftModule::HandleBindAlipayReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_INVITE_SCORE_REQ, &KFGiftModule::HandleQueryInviteScoreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_INVITE_SCORE_REQ, &KFGiftModule::HandleExchangeInviteScoreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_INVITE_RECORD_REQ, &KFGiftModule::HandleExChangeInviteRecordReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_GIFT_REWARD_REQ, &KFGiftModule::HandleReceiveGiftRewardReq );
    }

    void KFGiftModule::BeforeShut()
    {
        __UNREGISTER_MESSAGE__( KFMsg::MSG_BIND_ALIPAY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_INVITE_SCORE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_INVITE_SCORE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_INVITE_RECORD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_GIFT_REWARD_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFGiftModule::HandleBindAlipayReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgBindAlipayReq );

        auto apiurl = _kf_platform->MakePlatformUrl( "bindAlipay" );

        __JSON_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( accountid ), playerid );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( channel ), KFGlobal::Instance()->_app_id._union._app_data._channel_id );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( name ), kfmsg.name() );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( alipay ), kfmsg.alipay() );
        _kf_http_client->StartMTClient( this, &KFGiftModule::OnBindAlipayCallBack, apiurl, sendjson );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFGiftModule::OnBindAlipayCallBack )
    {
        __JSON_PARSE_STRING__( sendjson, senddata );
        auto playerid = __JSON_GET_UINT64__( sendjson, __KF_STRING__( accountid ) );
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        if ( recvdata.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::PlatformServerBusy );
        }

        __JSON_PARSE_STRING__( recvjson, recvdata );
        auto code = __JSON_GET_UINT32__( recvjson, __KF_STRING__( code ) );
        if ( code == 0 )
        {
            _kf_display->SendToClient( player, KFMsg::BindAlipayOk );
        }
        else
        {
            _kf_display->SendToClient( player, KFMsg::BindAlipayFailed, code );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGiftModule::HandleQueryInviteScoreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryInviteScoreReq );

        auto apiurl = _kf_platform->MakePlatformUrl( "getIntegralInfo" );

        __JSON_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( accountid ), playerid );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( channel ), KFGlobal::Instance()->_app_id._union._app_data._channel_id );
        _kf_http_client->StartMTClient( this, &KFGiftModule::OnQueryInviteScoreCallBack, apiurl, sendjson );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFGiftModule::OnQueryInviteScoreCallBack )
    {
        __JSON_PARSE_STRING__( sendjson, senddata );
        auto playerid = __JSON_GET_UINT64__( sendjson, __KF_STRING__( accountid ) );
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        if ( recvdata.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::PlatformServerBusy );
        }

        __JSON_PARSE_STRING__( recvjson, recvdata );
        auto code = __JSON_GET_UINT32__( recvjson, __KF_STRING__( code ) );
        if ( code != 0 )
        {
            return _kf_display->SendToClient( player, KFMsg::QueryInviteScoreError, code );
        }

        //auto& kfdata = recvjson[ __KF_STRING__( data ) ];
        //auto leftscore = __JSON_GET_UINT32__( kfdata, "cur_int" );
        //auto exchangescore = kfdata.GetUInt32( "pre_int" );

        //KFMsg::MsgQueryInviteScoreAck ack;
        //ack.set_leftscore( leftscore );
        //ack.set_exchangescore( exchangescore );
        //_kf_player->SendToClient( player, KFMsg::MSG_QUERY_INVITE_SCORE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGiftModule::HandleExchangeInviteScoreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExChangeInviteScoreReq );

        // 防止操作太快
        if ( _kf_player->CheckOperateFrequently( player, 3000 ) )
        {
            return;
        }

        //auto apiurl = _kf_platform->MakePlatformUrl( "integralCash" );

        //KFJson sendjson;
        //sendjson.SetValue( __KF_STRING__( accountid ), playerid );
        //sendjson.SetValue( __KF_STRING__( name ), kfmsg.name() );
        //sendjson.SetValue( __KF_STRING__( alipay ), kfmsg.alipay() );
        //sendjson.SetValue( __KF_STRING__( channel ), KFGlobal::Instance()->_app_id._union._app_data._channel_id );
        //_kf_http_client->StartMTHttpClient( this, &KFGiftModule::OnExchangeInviteScoreCallBack, apiurl, sendjson );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFGiftModule::OnExchangeInviteScoreCallBack )
    {
        //KFJson sendjson( senddata );
        //auto playerid = sendjson.GetUInt32( __KF_STRING__( accountid ) );
        //auto player = _kf_player->FindPlayer( playerid );
        //if ( player == nullptr )
        //{
        //    return;
        //}

        //if ( recvdata.empty() )
        //{
        //    return _kf_display->SendToClient( player, KFMsg::PlatformServerBusy );
        //}

        //KFJson recvjson( recvdata );
        //auto code = recvjson.GetUInt32( __KF_STRING__( code ) );
        //if ( code != 0 )
        //{
        //    return _kf_display->SendToClient( player, KFMsg::ExchangeInviteScoreError, code );
        //}

        //KFJson kfdata = recvjson[ __KF_STRING__( data ) ];
        //auto leftscore = kfdata.GetUInt32( "remain" );
        //auto exchangescore = kfdata.GetUInt32( "this_int" );

        //KFMsg::MsgExChangeInviteScoreAck ack;
        //ack.set_leftscore( leftscore );
        //ack.set_exchangescore( exchangescore );
        //_kf_player->SendToClient( player, KFMsg::MSG_EXCHANGE_INVITE_SCORE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGiftModule::HandleExChangeInviteRecordReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExChangeInviteRecordReq );

        // 防止操作太快
        if ( _kf_player->CheckOperateFrequently( player, 5000 ) )
        {
            return;
        }

        //auto apiurl = _kf_platform->MakePlatformUrl( "integralCashRecord" );

        //KFJson sendjson;
        //sendjson.SetValue( __KF_STRING__( accountid ), playerid );
        //sendjson.SetValue( __KF_STRING__( channel ), KFGlobal::Instance()->_app_id._union._app_data._channel_id );
        //_kf_http_client->StartMTHttpClient( this, &KFGiftModule::OnExChangeInviteRecordCallBack, apiurl, sendjson );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFGiftModule::OnExChangeInviteRecordCallBack )
    {
        //KFJson sendjson( senddata );
        //auto playerid = sendjson.GetUInt32( __KF_STRING__( accountid ) );
        //auto player = _kf_player->FindPlayer( playerid );
        //if ( player == nullptr )
        //{
        //    return;
        //}

        //if ( recvdata.empty() )
        //{
        //    return _kf_display->SendToClient( player, KFMsg::PlatformServerBusy );
        //}

        //KFJson recvjson( recvdata );
        //auto code = recvjson.GetUInt32( __KF_STRING__( code ) );
        //if ( code != 0 )
        //{
        //    return _kf_display->SendToClient( player, KFMsg::ExchangeInviteRecordError, code );
        //}

        //KFMsg::MsgExChangeInviteRecordAck ack;

        //KFJson kfdata = recvjson[ __KF_STRING__( data ) ];
        //for ( auto iter = kfdata.begin(); iter != kfdata.end(); ++iter )
        //{
        //    KFJson kfrecord = *iter;

        //    auto pbrecord = ack.add_record();
        //    pbrecord->set_name( kfrecord.GetString( __KF_STRING__( name ) ) );
        //    pbrecord->set_alipay( kfrecord.GetString( __KF_STRING__( alipay ) ) );
        //    pbrecord->set_score( kfrecord.GetUInt32( __KF_STRING__( money ) ) );
        //    pbrecord->set_time( kfrecord.GetUInt32( __KF_STRING__( time ) ) );
        //}

        //_kf_player->SendToClient( player, KFMsg::MSG_EXCHANGE_INVITE_RECORD_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGiftModule::HandleReceiveGiftRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReceiveGiftRewardReq );

        //auto apiurl = _kf_platform->MakePlatformUrl( "integralCashRecord" );

        //auto kfobject = player->GetData();
        //auto channel = kfobject->GetValue< uint32 >( __KF_STRING__( channel ) );

        //// 访问平台服务器, 验证token
        //KFJson sendjson;
        //sendjson.SetValue( __KF_STRING__( accountid ), playerid );
        //sendjson.SetValue( __KF_STRING__( channel ), channel );
        //sendjson.SetValue( __KF_STRING__( giftkey ), kfmsg.giftkey() );
        //_kf_http_client->StartMTHttpClient( this, &KFGiftModule::HandleReceiveGiftResult, apiurl, sendjson );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFGiftModule::HandleReceiveGiftResult )
    {
        //KFJson sendjson( senddata );
        //auto giftkey = sendjson.GetString( __KF_STRING__( giftkey ) );
        //auto playerid = sendjson.GetUInt32( __KF_STRING__( accountid ) );
        //auto player = _kf_player->FindPlayer( playerid );
        //if ( player == nullptr )
        //{
        //    return __LOG_ERROR__( "player[{}] gift[{}] off line!", playerid, giftkey );
        //}

        //if ( recvdata.empty() )
        //{
        //    return _kf_display->SendToClient( player, KFMsg::PlatformServerBusy );
        //}

        //KFJson recvjson( recvdata );
        //auto code = recvjson.GetUInt32( __KF_STRING__( code ) );
        //if ( code != 0 )
        //{
        //    return _kf_display->SendToClient( player, KFMsg::ReceiveGiftFailed, code );
        //}

        //auto reward = recvjson.GetString( __KF_STRING__( reward ) );
        //if ( reward.empty() )
        //{
        //    __LOG_ERROR__( "player[{}] gift[{}] reward empty!", playerid, giftkey );
        //}

        //KFAgents kfagents;
        //auto ok = kfagents.ParseFromString( reward, __FUNC_LINE__ );
        //if ( ok )
        //{
        //    player->AddAgentData( &kfagents, 1.0f, true, __FUNC_LINE__ );
        //    __LOG_INFO__( "player[{}] gift[{}] reward ok!", playerid, giftkey );
        //}
    }
}