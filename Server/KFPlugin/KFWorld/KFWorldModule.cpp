#include "KFWorldModule.h"
#include "KFOnlineEx.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFWorldModule::KFWorldModule()
    {
    }

    KFWorldModule::~KFWorldModule()
    {

    }

    void KFWorldModule::InitModule()
    {

    }

    void KFWorldModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFWorldModule::OnServerLostGame );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_GAME_SYNC_ONLINE_REQ, &KFWorldModule::HandleGameSyncOnlineReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_MESSAGE_REQ, &KFWorldModule::HandleTransmitMessageReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_MESSAGE_REQ, &KFWorldModule::HandleBroadcastMessageReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_WORLD_REQ, &KFWorldModule::HandlePlayerEnterWorldReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_WORLD_REQ, &KFWorldModule::HandlePlayerLeaveWorldReq );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( kickonline ), true, &KFWorldModule::HandleHttpKickOnline );
    }

    void KFWorldModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        __UNREGISTER_MESSAGE__( KFMsg::S2S_GAME_SYNC_ONLINE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_MESSAGE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_MESSAGE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_WORLD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_WORLD_REQ );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( kickonline ) );
    }

    void KFWorldModule::OnceRun()
    {
        // 把自己注册到platfrom
        KFJson sendjson;
        sendjson.SetValue( __KF_STRING__( zoneid ), _kf_zone->GetZone()->_id );
        sendjson.SetValue( __KF_STRING__( url ), _kf_http_server->GetHttpUrl() );

        auto url = _kf_ip_address->FindPlatformAddress( KFGlobal::Instance()->_app_id ) + __KF_STRING__( zonehttp );
        auto recvdata = _kf_http_client->StartSTHttpClient( url, sendjson, true );

        // 处理验证结果
        KFJson recvjson( recvdata );
        auto retcode = _kf_http_client->GetResponseCode( recvjson );
        if ( retcode == KFMsg::Success )
        {
            KF_LOG_INFO( "update http[{}] to platform ok!", _kf_http_server->GetHttpUrl() );
        }
        else
        {
            KF_LOG_ERROR( "update http[{}] to platform failed!", _kf_http_server->GetHttpUrl() );
        }
    }

    KFOnline* KFWorldModule::CreateOnline( uint32 playerid )
    {
        auto kfonline = __KF_CREATE__( KFOnlineEx );
        kfonline->SetPlayerId( playerid );

        _online_list.Insert( playerid, kfonline );
        return kfonline;
    }

    KFOnline* KFWorldModule::FindOnline( uint32 playerid )
    {
        return _online_list.Find( playerid );
    }

    bool KFWorldModule::RemoveOnline( uint32 playerid )
    {
        return _online_list.Remove( playerid );
    }

    uint32 KFWorldModule::GetOnlineCount()
    {
        return _online_list.Size();
    }

    bool KFWorldModule::SendMessageToOnline( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto kfonline = FindOnline( playerid );
        if ( kfonline == nullptr )
        {
            return false;
        }

        kfonline->SendMessageToOnline( msgid, message );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFWorldModule::OnServerLostGame )
    {
        if ( handlename != __KF_STRING__( game ) )
        {
            return;
        }

    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleGameSyncOnlineReq )
    {
        __PROTO_PARSE__( KFMsg::S2SGameSyncOnlineReq );

        auto serverid = __KF_HEAD_ID__( kfguid );
        for ( auto i = 0; i < kfmsg.playerid_size(); ++i )
        {
            auto playerid = kfmsg.playerid( i );

            auto kfonline = FindOnline( playerid );
            if ( kfonline == nullptr )
            {
                kfonline = CreateOnline( playerid );
                kfonline->SetPlayerId( playerid );
            }

            kfonline->SetGameId( serverid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleTransmitMessageReq )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitMessageReq );

        auto kfonline = FindOnline( kfmsg.playerid() );
        if ( kfonline == nullptr )
        {
            return;
        }

        auto& msgdata = kfmsg.msgdata();
        kfonline->SendMessageToOnline( kfmsg.msgid(), msgdata.data(), static_cast< uint32 >( msgdata.size() ) );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleBroadcastMessageReq )
    {
        __PROTO_PARSE__( KFMsg::S2SBroadcastMessageReq );

        _kf_tcp_server->SendNetMessage( __KF_STRING__( game ), KFMsg::S2S_BROADCAST_MESSAGE_REQ, &kfmsg );
    }

    void KFWorldModule::UpdateOnlineToPlatfrom( uint32 accountid, uint32 playerid, uint32 online )
    {
        static auto _url = _kf_ip_address->FindPlatformAddress( KFGlobal::Instance()->_app_id ) + __KF_STRING__( onlinezone );

        // 在线服务器
        KFJson sendjson;
        sendjson.SetValue< uint32 >( __KF_STRING__( zoneid ), _kf_zone->GetZone()->_id );
        sendjson.SetValue< uint32 >( __KF_STRING__( playerid ), playerid );
        sendjson.SetValue< uint32 >( __KF_STRING__( accountid ), accountid );
        sendjson.SetValue< uint32 >( __KF_STRING__( online ), online );
        _kf_http_client->StartMTHttpClient( _url, sendjson, false );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandlePlayerEnterWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerEnterWorldReq );

        auto kfonline = CreateOnline( kfmsg.playerid() );
        kfonline->SetPlayerId( kfmsg.playerid() );
        kfonline->SetGameId( __KF_HEAD_ID__( kfguid ) );
        kfonline->SetAccountId( kfmsg.accountid() );

        // 更新到账号服务器
        UpdateOnlineToPlatfrom( kfmsg.accountid(), kfmsg.playerid(), 1 );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] online count=[%u]",
                            __FUNCTION__, _online_list.Size() );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandlePlayerLeaveWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerLeaveWorldReq );

        RemoveOnline( kfmsg.playerid() );

        // 更新到账号服务器
        UpdateOnlineToPlatfrom( kfmsg.accountid(), kfmsg.playerid(), 0 );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] online count=[%u]",
                            __FUNCTION__, _online_list.Size() );
    }

    bool KFWorldModule::KickOnline( uint32 playerid, const char* function, uint32 line )
    {
        auto kfonline = FindOnline( playerid );
        if ( kfonline == nullptr )
        {
            return false;
        }

        KFLogger::LogLogic( KFLogger::Info, "[%s:%u] kick player[%u]!", function, line, playerid );

        // 发送踢出消息
        KFMsg::S2SKickGamePlayerReq req;
        req.set_playerid( playerid );
        req.set_type( KFMsg::KickEnum::LoginBeKick );
        auto ok = _kf_tcp_server->SendNetMessage( kfonline->GetGameId(), KFMsg::S2S_KICK_GAME_PLAYER_REQ, &req );
        if ( ok )
        {
            // 发送成功, 先删除在线信息, 避免gameserver掉线以后,登录不正常的问题
            RemoveOnline( playerid );
        }
        return ok;
    }

    bool KFWorldModule::SendMessageToGame( uint32 gameid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gameid, msgid, message );
    }

    __KF_HTTP_FUNCTION__( KFWorldModule::HandleHttpKickOnline )
    {
        KFJson kfjson( data );

        auto playerid = kfjson.GetUInt32( __KF_STRING__( playerid ) );
        KickOnline( playerid, __FUNCTION_LINE__ );
        return _invalid_str;
    }

}
