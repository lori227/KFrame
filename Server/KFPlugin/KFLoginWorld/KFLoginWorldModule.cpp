#include "KFLoginWorldModule.h"
#include "KFLoginGameManage.h"
#include "KFLoginProxyManage.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFLoginWorldModule::KFLoginWorldModule()
    {

    }

    KFLoginWorldModule::~KFLoginWorldModule()
    {

    }

    void KFLoginWorldModule::InitModule()
    {
    }

    void KFLoginWorldModule::BeforeRun()
    {
        _kf_tcp_server->RegisterLostFunction( this, &KFLoginWorldModule::OnServerLostGame );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_UPDATE_REQ, &KFLoginWorldModule::HandleLoginGameUpdateReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_PROXY_UPDATE_REQ, &KFLoginWorldModule::HandleLoginProxyUpdateReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_WORLD_VERIFY_REQ, &KFLoginWorldModule::HandleLoginWorldVerifyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_PROXY_REMOVE_REQ, &KFLoginWorldModule::HandleLoginProxyRemoveReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_WORLD_ACK, &KFLoginWorldModule::HandleLoginTellTokenToWorldAck );
    }

    void KFLoginWorldModule::BeforeShut()
    {
        _kf_tcp_server->UnRegisterLostFunction( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_UPDATE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_PROXY_UPDATE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_WORLD_VERIFY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_WORLD_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFLoginWorldModule::OnServerLostGame )
    {
        _kf_login_game_manage->RemoveLoginGame( handleid );
    }

    __KF_MESSAGE_FUNCTION__( KFLoginWorldModule::HandleLoginGameUpdateReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginGameUpdateReq );
        _kf_login_game_manage->UpdateLoginGame( kfmsg.gameid(), kfmsg.count() );
    }

    __KF_MESSAGE_FUNCTION__( KFLoginWorldModule::HandleLoginProxyUpdateReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginProxyUpdateReq );
        _kf_login_proxy_manage->UpdateLoginProxy( kfmsg.proxyid(), kfmsg.count(), kfmsg.ip(), kfmsg.port() );
    }

    __KF_MESSAGE_FUNCTION__( KFLoginWorldModule::HandleLoginProxyRemoveReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginProxyRemoveReq );
        _kf_login_proxy_manage->RemoveLoginProxy( kfmsg.proxyid() );
    }

    __KF_MESSAGE_FUNCTION__( KFLoginWorldModule::HandleLoginWorldVerifyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginWorldVerifyReq );
        auto loginid = __KF_HEAD_ID__( kfguid );
        auto& token = kfmsg.token();
        auto gateid = kfmsg.gateid();
        auto playerid = kfmsg.playerid();
        auto accountid = kfmsg.accountid();

        KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] verify req!",
                            __FUNCTION__, accountid, playerid );

        // 踢掉已经在线的玩家, 只有踢下线以后才能登陆
        if ( _kf_world->KickOnline( playerid, __FUNCTION_LINE__ ) )
        {
            return SendLoginWorldVerifyMessage( loginid, gateid, KFMsg::LoginAlreadyOnline, 0, 0, _invalid_str, _invalid_str, 0 );
        }

        // 选择Game服务器
        auto kflogingame = _kf_login_game_manage->HashLoginGame( playerid );
        if ( kflogingame == nullptr )
        {
            return SendLoginWorldVerifyMessage( loginid, gateid, KFMsg::CanNotFindGame, 0, 0, _invalid_str, _invalid_str, 0 );
        }

        // 选择Proxy服务器
        auto kfloginproxy = _kf_login_proxy_manage->SelectLoginProxy();
        if ( kfloginproxy == nullptr )
        {
            return SendLoginWorldVerifyMessage( loginid, gateid, KFMsg::CanNotFindProxy, 0, 0, _invalid_str, _invalid_str, 0 );
        }

        // 通知游戏服务器 保存验证信息
        KFMsg::S2SLoginTellTokenToGameReq req;
        req.set_loginid( loginid );
        req.set_gateid( gateid );
        req.set_token( kfmsg.token() );
        req.set_accountid( accountid );
        req.set_playerid( kfmsg.playerid() );
        req.set_proxyid( kfloginproxy->_proxy_id );
        auto ok = _kf_tcp_server->SendNetMessage( kflogingame->_game_id, KFMsg::S2S_LOGIN_TELL_TOKEN_TO_GAME_REQ, &req );
        if ( ok )
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] login token to game ok!",
                                __FUNCTION__, kfmsg.accountid(), kfmsg.playerid() );
        }
        else
        {
            KFLogger::LogLogin( KFLogger::Error, "[%s] accountid[%u] player[%u] login token to game failed!",
                                __FUNCTION__, kfmsg.accountid(), kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFLoginWorldModule::HandleLoginTellTokenToWorldAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginTellTokenToWorldAck );

        auto loginid = kfmsg.loginid();
        auto gateid = kfmsg.gateid();
        auto accountid = kfmsg.accountid();
        auto playerid = kfmsg.playerid();
        auto& token = kfmsg.token();
        auto& ip = kfmsg.ip();
        auto port = kfmsg.port();

        // 通知客户端
        SendLoginWorldVerifyMessage( loginid, gateid, KFMsg::Success, accountid, playerid, token, ip, port );
    }

    void KFLoginWorldModule::SendLoginWorldVerifyMessage( uint32 serverid, uint32 gateid, uint32 result, uint32 accountid, uint32 playerid, const std::string& token, const std::string& ip, uint32 prot )
    {
        KFMsg::S2SLoginWorldVerifyAck ack;
        ack.set_ip( ip );
        ack.set_port( prot );
        ack.set_token( token );
        ack.set_gateid( gateid );
        ack.set_result( result );
        ack.set_playerid( playerid );
        ack.set_accountid( accountid );
        auto ok = _kf_world->SendMessageToGame( serverid, KFMsg::S2S_LOGIN_WORLD_VERIFY_ACK, &ack );
        if ( ok )
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] login world verify result[%u] ok!",
                                __FUNCTION__, accountid, playerid, result );
        }
        else
        {
            KFLogger::LogLogin( KFLogger::Info, "[%s] accountid[%u] player[%u] login world verify result[%u] failed!",
                                __FUNCTION__, accountid, playerid, result );
        }

    }
}
