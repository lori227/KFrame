#include "KFGiftModule.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFGiftModule::KFGiftModule()
    {
    }

    KFGiftModule::~KFGiftModule()
    {

    }


    void KFGiftModule::BeforeRun()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_GIFT_REWARD_REQ, &KFGiftModule::HandleReceiveGiftRewardReq );
    }

    void KFGiftModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_GIFT_REWARD_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFGiftModule::HandleReceiveGiftRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReceiveGiftRewardReq );

        // 访问平台服务器, 验证token
        KFJson sendjson;
        sendjson.SetValue( __KF_STRING__( playerid ), playerid );
        sendjson.SetValue( __KF_STRING__( giftkey ), kfmsg.giftkey() );
        sendjson.SetValue( __KF_STRING__( serverid ), _kf_zone->GetZone()->_id );

        //static auto url = _kf_ip_address->FindPlatformAddress( playerid ) + __KF_STRING__( gift );
        // _kf_http_client->StartMTHttpClient( url, sendjson, false, this, &KFGiftModule::HandleReceiveGiftResult );
    }

    void KFGiftModule::HandleReceiveGiftResult( std::string& senddata, std::string& recvdata )
    {
        // 处理验证结果
        KFJson sendjson( senddata );
        KFJson recvjson( recvdata );

        uint32 playerid = sendjson.GetUInt32( __KF_STRING__( playerid ) );
        auto player = _kf_player->FindPlayer( playerid, __FUNC_LINE__ );
        if ( player == nullptr )
        {
            return;
        }

        auto retcode = _kf_http_client->GetResponseCode( recvjson );
        _kf_display->SendToClient( player, retcode );

        if ( retcode == KFMsg::Success )
        {
            auto reward = recvjson.GetString( __KF_STRING__( reward ) );

            KFAgents kfagents;
            kfagents.ParseFromString( reward, __FUNC_LINE__ );
            player->AddAgentData( &kfagents, 1.0f, true, __FUNC_LINE__ );
        }
    }
}