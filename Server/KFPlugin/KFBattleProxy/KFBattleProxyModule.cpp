#include "KFBattleProxyModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFBattleProxyModule::KFBattleProxyModule()
    {
    }

    KFBattleProxyModule::~KFBattleProxyModule()
    {
    }

    void KFBattleProxyModule::InitModule()
    {
        ///////////////////////////////////////////////////////////////////////////////
    }

    void KFBattleProxyModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFBattleProxyModule::OnServerLostHandle );
        ////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_BATTLE_SERVER_REQ, &KFBattleProxyModule::HandleRegisterBattleServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_BATTLE_PROXY_REQ, &KFBattleProxyModule::HandleCreateRoomToBattleProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_BATTLE_ROOM_REQ, &KFBattleProxyModule::HandleQueryBattleRoomReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_ROOM_START_REQ, &KFBattleProxyModule::HandleTellBattleRoomStartReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_BATTLE_ROOM_ACK, &KFBattleProxyModule::HandlePlayerEnterBattleRoomAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_ROOM_FINISH_REQ, &KFBattleProxyModule::HandleTellBattleRoomFinishReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_BATTLE_ROOM_REQ, &KFBattleProxyModule::HandlePlayerLeaveBattleRoomReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LOGIN_BATTLE_ROOM_REQ, &KFBattleProxyModule::HandlePlayerLoginBattleRoomReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_BATTLE_ROOM_SCORE_BALANCE_REQ, &KFBattleProxyModule::HandleBattleRoomScoreBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_OPEN_BATTLE_ROOM_ACK, &KFBattleProxyModule::HandleOpenBattleRoomAck );
    }

    void KFBattleProxyModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_BATTLE_SERVER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_BATTLE_PROXY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_BATTLE_ROOM_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_ROOM_START_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_BATTLE_ROOM_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_ROOM_FINISH_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_BATTLE_ROOM_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LOGIN_BATTLE_ROOM_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BATTLE_ROOM_SCORE_BALANCE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_OPEN_BATTLE_ROOM_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFBattleProxyModule::OnServerLostHandle )
    {
        if ( handlename == __KF_STRING__( battle ) && handletype == __KF_STRING__( client ) )
        {
            OnServerLostBattleServer( handleid );
        }
    }

    void KFBattleProxyModule::OnServerLostBattleServer( uint32 serverid )
    {
        __LOG_INFO__( "battle[{}] lost req!", serverid );

        auto shardid = _kf_cluster_proxy->SelectClusterShard( serverid, false );
        if ( shardid == _invalid_int )
        {
            __LOG_ERROR__( "battle[{}] can't select shard!", serverid );
            return;
        }

        KFMsg::S2SDisconnectServerToBattleShardReq req;
        req.set_serverid( serverid );
        auto ok = _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_DISCONNECT_SERVER_TO_BATTLE_SHARD_REQ, &req );
        if ( ok )
        {
            __LOG_INFO__( "battle[{}] lost ok!", serverid );
        }
        else
        {
            __LOG_ERROR__( "battle[{}] lost failed!", serverid );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleRegisterBattleServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterBattleServerReq );

        __LOG_DEBUG__( "register battle[{}|{}:{}|{}:{}] req!",
                       kfmsg.serverid(), kfmsg.ip(), kfmsg.port(), kfmsg.roomid(), kfmsg.battleshardid() );

        auto shardid = kfmsg.battleshardid();
        if ( shardid == _invalid_int )
        {
            shardid = _kf_cluster_proxy->SelectClusterShard( kfmsg.serverid(), false );
            if ( shardid == _invalid_int )
            {
                __LOG_ERROR__( "battle[{}|{}:{}] can't select shard!",
                               kfmsg.serverid(), kfmsg.ip(), kfmsg.port() );
                return;
            }
        }

        {
            KFMsg::S2SRegisterServerToBattleShardReq req;
            req.set_ip( kfmsg.ip() );
            req.set_port( kfmsg.port() );
            req.set_roomid( kfmsg.roomid() );
            req.set_serverid( kfmsg.serverid() );
            _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_REGISTER_SERVER_TO_BATTLE_SHARD_REQ, &req );
        }

        {
            // 广播给所有服务器, 有战斗服务器注册了
            KFMsg::S2STellBattleRegisterToShardReq req;
            req.set_serverid( kfmsg.serverid() );
            req.set_roomid( kfmsg.roomid() );
            req.set_ip( kfmsg.ip() );
            req.set_port( kfmsg.port() );
            _kf_cluster_proxy->SendMessageToShard( KFMsg::S2S_TELL_BATTLE_REGISTER_TO_SHARD_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleCreateRoomToBattleProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateRoomToBattleProxyReq );
        __LOG_DEBUG__( "create room[{}] req!", kfmsg.roomid() );

        auto shardid = _kf_cluster_proxy->FindDynamicShard( kfmsg.roomid() );
        if ( shardid == _invalid_int )
        {
            shardid = _kf_cluster_proxy->SelectClusterShard( kfmsg.roomid(), false );
            if ( shardid == _invalid_int )
            {
                __LOG_ERROR__( "[{}] can't select shard!", kfmsg.roomid() );
                return;
            }

            _kf_cluster_proxy->AddDynamicShard( kfmsg.roomid(), shardid );
        }

        KFMsg::S2SCreateRoomToBattleShardReq req;
        req.set_matchid( kfmsg.matchid() );
        req.set_roomid( kfmsg.roomid() );
        req.set_battleserverid( kfmsg.battleserverid() );
        req.set_matchshardid( __KF_HEAD_ID__( kfguid ) );
        req.set_maxplayercount( kfmsg.maxplayercount() );
        auto ok = _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_CREATE_ROOM_TO_BATTLE_SHARD_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( "create room[{}:{}] ok!", shardid, kfmsg.roomid() );
        }
        else
        {
            __LOG_ERROR__( "create room[{}:{}] failed!", shardid, kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleQueryBattleRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryBattleRoomReq );

        auto shardid = _kf_cluster_proxy->FindDynamicShard( kfmsg.roomid() );
        if ( shardid == _invalid_int )
        {
            KFMsg::S2SQueryBattleRoomAck ack;
            ack.set_roomid( _invalid_int );
            ack.set_matchid( _invalid_int );
            ack.set_playerid( kfmsg.playerid() );
            _kf_cluster_proxy->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_QUERY_BATTLE_ROOM_ACK, &ack );
        }
        else
        {
            KFMsg::S2SQueryRoomToBattleShardReq req;
            req.set_roomid( kfmsg.roomid() );
            req.set_playerid( kfmsg.playerid() );
            req.set_serverid( kfmsg.serverid() );
            _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_QUERY_ROOM_TO_BATTLE_SHARD_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleOpenBattleRoomAck )
    {
        __PROTO_PARSE__( KFMsg::S2SOpenBattleRoomAck );

        KFMsg::S2SOpenBattleRoomToShardAck ack;
        ack.set_roomid( kfmsg.roomid() );
        ack.set_matchid( kfmsg.matchid() );
        ack.set_result( kfmsg.result() );
        ack.set_waittime( kfmsg.waittime() );
        auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_OPEN_BATTLE_ROOM_TO_SHARD_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "open room[{}:{}] failed!", kfmsg.battleshardid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandlePlayerEnterBattleRoomAck )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerEnterBattleRoomAck );

        KFMsg::S2SPlayerEnterRoomToBattleShardAck ack;
        ack.set_roomid( kfmsg.roomid() );
        ack.set_campid( kfmsg.campid() );
        ack.set_playerid( kfmsg.playerid() );
        auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_PLAYER_ENTER_ROOM_TO_BATTLE_SHARD_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "enter room[{}:{}] failed!", kfmsg.battleshardid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleTellBattleRoomStartReq )
    {
        __PROTO_PARSE__( KFMsg::S2STellBattleRoomStartReq );

        KFMsg::S2STellRoomStartToBattleShardReq req;
        req.set_roomid( kfmsg.roomid() );
        req.set_maxtime( kfmsg.maxtime() );
        auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_TELL_ROOM_START_TO_BATTLE_SHARD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "start room[{}:{}] failed!", kfmsg.battleshardid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleTellBattleRoomFinishReq )
    {
        __PROTO_PARSE__( KFMsg::S2STellBattleRoomFinishReq );

        KFMsg::S2STellRoomFinishToBattleShardReq req;
        req.set_roomid( kfmsg.roomid() );
        req.set_serverid( kfmsg.serverid() );
        req.set_ip( kfmsg.ip() );
        auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_TELL_ROOM_FINISH_TO_BATTLE_SAHRD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "finish room[{}:{}] failed!", kfmsg.battleshardid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandlePlayerLoginBattleRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerLoginBattleRoomReq );

        KFMsg::S2SPlayerLoginRoomToBattleShardReq req;
        req.set_roomid( kfmsg.roomid() );
        req.set_campid( kfmsg.campid() );
        req.set_playerid( kfmsg.playerid() );
        auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_PLAYER_LOGIN_ROOM_TO_BATTLE_SHARD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "login room[{}:{}] failed!", kfmsg.battleshardid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandlePlayerLeaveBattleRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerLeaveBattleRoomReq );

        KFMsg::S2SPlayerLeaveRoomToBattleShardReq req;
        req.set_roomid( kfmsg.roomid() );
        req.set_campid( kfmsg.campid() );
        req.set_playerid( kfmsg.playerid() );
        auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_BATTLE_SHARD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "leave room[{}:{}] failed!", kfmsg.battleshardid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleProxyModule::HandleBattleRoomScoreBalanceReq )
    {
        __PROTO_PARSE__( KFMsg::S2SBattleRoomScoreBalanceReq );

        KFMsg::S2SBattleScoreBalanceToShardReq req;
        req.set_roomid( kfmsg.roomid() );
        req.mutable_pbscore()->CopyFrom( kfmsg.pbscore() );
        auto ok = _kf_cluster_proxy->SendMessageToShard( kfmsg.battleshardid(), KFMsg::S2S_BATTLE_SCORE_BALANCE_TO_SHARD_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "balance room[{}:{}] failed!", kfmsg.battleshardid(), kfmsg.roomid() );
        }
    }

}