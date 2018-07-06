#include "KFBattleShardModule.h"
#include "KFProtocol/KFProtocol.h"
#include "KFBattleManage.h"
#include "KFBattleConfig.h"

namespace KFrame
{
    static KFRedisDriver* _kf_battle_driver = nullptr;
#define __BATTLE_REDIS_DRIVER__ _kf_battle_driver;

    KFBattleShardModule::KFBattleShardModule()
    {
    }

    KFBattleShardModule::~KFBattleShardModule()
    {
    }

    void KFBattleShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_battle_config, true );
    }

    void KFBattleShardModule::BeforeRun()
    {
        __REGISTER_RUN_FUNCTION__( &KFBattleShardModule::Run );
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFBattleShardModule::OnServerDiscoverBattleProxy );

        _kf_battle_config->LoadRewardConfig( _kf_battle_config->_reward_file.c_str() );
        ///////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_SERVER_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandleRegisterServerToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_REGISTER_TO_SHARD_REQ, &KFBattleShardModule::HandleTellBattleRegisterToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DISCONNECT_SERVER_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandleDisconnectServerToBattleServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandleCreateRoomToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_OPEN_BATTLE_ROOM_TO_SHARD_ACK, &KFBattleShardModule::HandleOpenBattleRoomToShardAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_ROOM_TO_BATTLE_SHARD_ACK, &KFBattleShardModule::HandlePlayerEnterRoomToBattleShardAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandlePlayerLeaveRoomToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_START_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandleTellRoomStartToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_FINISH_TO_BATTLE_SAHRD_REQ, &KFBattleShardModule::HandleTellRoomFinishToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_NOTICE_MATCH_ROOM_ACK, &KFBattleShardModule::HandleNoticeMatchRoomAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandleQueryRoomToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_CAMP_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandleAddCampToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandleCancelMatchToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_OPEN_ROOM_TO_BATTLE_SHARD_ACK, &KFBattleShardModule::HandleOpenRoomToBattleShardAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LOGIN_ROOM_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandlePlayerLoginRoomToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_START_TO_BATTLE_SHARD_ACK, &KFBattleShardModule::HandleTellRoomStartToBattleShardAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ONLINE_TO_BATTLE_SHARD_REQ, &KFBattleShardModule::HandlePlayerOnlineToBattleShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_BATTLE_SCORE_ACK, &KFBattleShardModule::HandlePlayerBattleScoreAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_BATTLE_SCORE_BALANCE_TO_SHARD_REQ, &KFBattleShardModule::HandleBattleScoreBalanceToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ONLINE_QUERY_BATTLE_SCORE_REQ, &KFBattleShardModule::HandleOnlieQueryBattleScoreReq );
    }

    void KFBattleShardModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_SERVER_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_BATTLE_REGISTER_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DISCONNECT_SERVER_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_OPEN_BATTLE_ROOM_TO_SHARD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_ROOM_TO_BATTLE_SHARD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_START_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_FINISH_TO_BATTLE_SAHRD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_NOTICE_MATCH_ROOM_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_CAMP_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_OPEN_ROOM_TO_BATTLE_SHARD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LOGIN_ROOM_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_START_TO_BATTLE_SHARD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ONLINE_TO_BATTLE_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_BATTLE_SCORE_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BATTLE_SCORE_BALANCE_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ONLINE_QUERY_BATTLE_SCORE_REQ );
    }

    void KFBattleShardModule::OnceRun()
    {
        _kf_battle_manage->Initialize();
        _kf_battle_driver = _kf_redis->CreateExecute( KFField::_battle );
    }

    void KFBattleShardModule::Run()
    {
        std::set< KFBattleRoom* > removelist;

        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            kfroom->RunRoom();

            // 无效了
            if ( !kfroom->CheckValid() )
            {
                removelist.insert( kfroom );
            }

            kfroom = _kf_room_list.Next();
        }

        // 删除无效的战场
        for ( auto kfroom : removelist )
        {
            kfroom->FreeBattleServer();
            _kf_room_list.Remove( kfroom->_battle_room_id );
        }
    }

    void KFBattleShardModule::ShutDown()
    {
        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            kfroom->FreeBattleServer();
            kfroom = _kf_room_list.Next();
        }

        __UNREGISTER_RUN_FUNCTION__();
    }

    void KFBattleShardModule::RemoveBattleRoom( KFBattleRoom* kfroom )
    {
        // 通知proxy
        _kf_cluster_shard->RemoveObjectToProxy( kfroom->_battle_room_id );

        // 结束战场
        kfroom->FinishBattleRoom();

        // 删除房间
        _kf_room_list.Remove( kfroom->_battle_room_id );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_DISCOVER_FUNCTION__( KFBattleShardModule::OnServerDiscoverBattleProxy )
    {
        // 把房间的列表同步到proxy服务器
        std::list< uint64 > roomlist;

        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            roomlist.push_back( kfroom->_battle_room_id );
            kfroom = _kf_room_list.Next();
        }

        _kf_cluster_shard->AddObjectToProxy( handleid, roomlist );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleRegisterServerToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterServerToBattleShardReq );
        auto proxyid = __KF_HEAD_ID__( kfguid );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] register battle[%u|%s:%u|%s] req!",
                            __FUNCTION__, kfmsg.serverid(), kfmsg.ip().c_str(), kfmsg.port(), strroomid.c_str() );
        if ( kfmsg.serverid() == _invalid_int || kfmsg.ip().empty() || kfmsg.port() == _invalid_int )
        {
            return;
        }

        _kf_battle_manage->RegisterBattleServer( kfmsg.serverid(), proxyid, kfmsg.ip(), kfmsg.port() );

        KFMsg::S2SRegisterBattleServerAck ack;
        ack.set_serverid( kfmsg.serverid() );
        ack.set_result( KFMsg::Success );
        _kf_cluster_shard->SendMessageToClient( proxyid, kfmsg.serverid(), KFMsg::S2S_REGISTER_BATTLE_SERVER_ACK, &ack );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] register battle[%u|%s:%u|%s] ok!",
                            __FUNCTION__, kfmsg.serverid(), kfmsg.ip().c_str(), kfmsg.port(), strroomid.c_str() );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleTellBattleRegisterToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2STellBattleRegisterToShardReq );
        auto proxyid = __KF_HEAD_ID__( kfguid );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] battle[%u|%s:%u] update room[%s] req!",
                            __FUNCTION__, kfmsg.serverid(), kfmsg.ip().c_str(), kfmsg.port(), strroomid.c_str() );

        if ( kfmsg.roomid() != _invalid_int )
        {
            // roomid不为0 断线重连
            auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
            if ( kfroom != nullptr )
            {
                kfroom->UpdateBattleRoom( proxyid, kfmsg.serverid(), kfmsg.ip(), kfmsg.port() );
            }
        }
        else
        {
            // roomid为0, 宕机重启
            auto kfroom = FindBattleRoomByServerId( kfmsg.serverid() );
            if ( kfroom != nullptr )
            {
                if ( kfroom->IsBattleRoomStart() )
                {
                    RemoveBattleRoom( kfroom );
                }
                else
                {
                    kfroom->UpdateBattleRoom( proxyid, kfmsg.serverid(), kfmsg.ip(), kfmsg.port() );
                }
            }
        }

        KFLogger::LogLogic( KFLogger::Debug, "[%s] battle[%u|%s:%u] update room[%s] ok!",
                            __FUNCTION__, kfmsg.serverid(), kfmsg.ip().c_str(), kfmsg.port(), strroomid.c_str() );
    }

    KFBattleRoom* KFBattleShardModule::FindBattleRoomByServerId( uint32 serverid )
    {
        if ( serverid == _invalid_int )
        {
            return nullptr;
        }

        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            if ( kfroom->_battle_server._server_id == serverid )
            {
                return kfroom;
            }

            kfroom = _kf_room_list.Next();
        }

        return nullptr;
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleDisconnectServerToBattleServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDisconnectServerToBattleShardReq );

        _kf_battle_manage->UnRegisterBattleServer( kfmsg.serverid() );

        KFLogger::LogLogic( KFLogger::Error, "[%s] battle[%u] disconnect!",
                            __FUNCTION__, kfmsg.serverid() );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleCreateRoomToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateRoomToBattleShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        // 创建房间
        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            kfroom = __KF_CREATE_BATCH__( KFBattleRoom, 100 );
            kfroom->InitRoom( kfmsg.matchid(), kfmsg.roomid(), kfmsg.maxplayercount() );
            _kf_room_list.Insert( kfmsg.roomid(), kfroom );
        }

        // 设置路由信息
        kfroom->_match_proxy_id = __KF_HEAD_ID__( kfguid );
        kfroom->_match_shard_id = kfmsg.matchshardid();

        {
            // 回复确认消息
            KFMsg::S2SCreateRoomToMatchShardAck ack;
            ack.set_matchid( kfmsg.matchid() );
            ack.set_roomid( kfmsg.roomid() );
            ack.set_battleshardid( KFGlobal::Instance()->_app_id );
            kfroom->SendMessageToMatch( KFMsg::S2S_CREATE_ROOM_TO_MATCH_SHARD_ACK, &ack );
        }

        // 广播给所有的proxy
        _kf_cluster_shard->AddObjectToProxy( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] matchid[%u] room[%s] create ok!",
                            __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleOpenBattleRoomToShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SOpenBattleRoomToShardAck );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] open room[%s] result[%u]!",
                            __FUNCTION__, strroomid.c_str(), kfmsg.result() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        if ( kfmsg.result() == KFMsg::BattleEnum::OpenSuccess )
        {
            kfroom->ConfirmOpenBattleRoom( true );
        }
        else
        {
            kfroom->ConfirmOpenBattleRoom( false );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleOpenRoomToBattleShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SOpenRoomToBattleShardAck );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] open match room[%s] ack!",
                            __FUNCTION__, strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        kfroom->ConfirmOpenMatchRoom();

        KFLogger::LogLogic( KFLogger::Debug, "[%s] open match room[%s] ok!",
                            __FUNCTION__, strroomid.c_str() );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleAddCampToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddCampToBattleShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] add camp[%u] req!",
                            __FUNCTION__, strroomid.c_str(), kfmsg.pbcamp().campid() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        auto kfcamp = kfroom->AddCamp( &kfmsg.pbcamp() );
        if ( kfcamp != nullptr )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] add camp[%u] ok!",
                                __FUNCTION__, strroomid.c_str(), kfmsg.pbcamp().campid() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] room[%s] add camp[%u] failed!",
                                __FUNCTION__, strroomid.c_str(), kfmsg.pbcamp().campid() );
        }

        KFMsg::S2SAddCampToMatchShardAck ack;
        ack.set_matchid( kfroom->_match_id );
        ack.set_roomid( kfroom->_battle_room_id );
        ack.set_campid( kfmsg.pbcamp().campid() );
        ack.set_addok( kfcamp != nullptr );
        auto ok = kfroom->SendMessageToMatch( KFMsg::S2S_ADD_CAMP_TO_MATCH_SHARD_ACK, &ack );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] add camp[%u] send ok!",
                                __FUNCTION__, strroomid.c_str(), kfmsg.pbcamp().campid() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] add camp[%u] send failed!",
                                __FUNCTION__, strroomid.c_str(), kfmsg.pbcamp().campid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleCancelMatchToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCancelMatchToBattleShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] player[%u:%u] cancel match req!",
                            __FUNCTION__, strroomid.c_str(), kfmsg.campid(), kfmsg.playerid() );

        bool isopen = true;
        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom != nullptr )
        {
            isopen = ( kfroom->_status >= KFRoomStatus::StatusBattleRoomOpen );
            auto ok = kfroom->CancelMatch( kfmsg.campid(), kfmsg.playerid() );
            if ( ok )
            {
                KFLogger::LogLogic( KFLogger::Error, "[%s] [%u:%u] room[%s] cancel match ok!",
                                    __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
            }
            else
            {
                KFLogger::LogLogic( KFLogger::Error, "[%s] [%u:%u] room[%s] cancel match failed!",
                                    __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
            }
        }

        // 发送消息到集群
        KFMsg::S2SCancelMatchToMatchShardAck ack;
        ack.set_isroomopen( isopen );
        ack.set_matchid( kfmsg.matchid() );
        ack.set_roomid( kfmsg.roomid() );
        ack.set_campid( kfmsg.campid() );
        ack.set_playerid( kfmsg.playerid() );
        _kf_cluster_shard->SendMessageToClient( __KF_HEAD_ID__( kfguid ), kfmsg.matchshardid(), KFMsg::S2S_CANCEL_MATCH_TO_MATCH_SHARD_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandlePlayerEnterRoomToBattleShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerEnterRoomToBattleShardAck );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%u] enter room[%s] ack!",
                            __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        auto ok = kfroom->ConfirmEnterBattleRoom( kfmsg.campid(), kfmsg.playerid() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%u] enter room[%s] ok!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] player[%u:%u] enter room[%s] failed!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandlePlayerLeaveRoomToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerLeaveRoomToBattleShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%u] leave room[%s] req!",
                            __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        auto ok = kfroom->LeaveBattleRoom( kfmsg.campid(), kfmsg.playerid() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%u] leave room[%s] ok!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] [%u:%u] leave battle room[%s] failed!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }

        // 回复给战斗服务器
        KFMsg::S2SPlayerLeaveBattleRoomAck ack;
        ack.set_roomid( kfmsg.roomid() );
        ack.set_campid( kfmsg.campid() );
        ack.set_playerid( kfmsg.playerid() );
        kfroom->SendMessageToBattle( KFMsg::S2S_PLAYER_LEAVE_BATTLE_ROOM_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleNoticeMatchRoomAck )
    {
        __PROTO_PARSE__( KFMsg::S2SNoticeMatchRoomAck )
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] [%u:%u] notice battle room[%s] ack!",
                            __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        auto ok = kfroom->NoticeBattleRoom( kfmsg.campid(), kfmsg.playerid() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] [%u:%u] notice battle room[%s] ok!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] [%u:%u] notice battle room[%s] failed!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandlePlayerLoginRoomToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerLoginRoomToBattleShardReq );

        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] login room[%s] req!",
                            __FUNCTION__, kfmsg.playerid(), strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        auto ok = kfroom->LoginBattleRoom( kfmsg.campid(), kfmsg.playerid() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] [%u:%u] login battle room[%s] ok!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] [%u:%u] login battle room[%s] failed!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleQueryRoomToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRoomToBattleShardReq );

        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] query room[%s] req!",
                            __FUNCTION__, kfmsg.playerid(), strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Debug, "[%s] can't find room[%s]",
                                       __FUNCTION__, strroomid.c_str() );
        }

        auto ok = kfroom->QueryBattleRoom( kfmsg.playerid(), kfmsg.serverid() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] query room[%s] ok!",
                                __FUNCTION__, kfmsg.playerid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] query room[%s] failed!",
                                __FUNCTION__, kfmsg.playerid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleTellRoomStartToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2STellRoomStartToBattleShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] start room[%s] req!",
                            __FUNCTION__, strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        kfroom->StartBattleRoom();

        // 回复给战斗服务器
        KFMsg::S2STellBattleRoomStartAck ack;
        ack.set_roomid( kfmsg.roomid() );
        kfroom->SendMessageToBattle( KFMsg::S2S_TELL_BATTLE_ROOM_START_ACK, &ack );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] start ok!",
                            __FUNCTION__, strroomid.c_str() );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleTellRoomStartToBattleShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2STellRoomStartToMatchShardAck );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] start room[%s] ack!",
                            __FUNCTION__, strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        kfroom->ConfirmStartBattleRoom();

        KFLogger::LogLogic( KFLogger::Debug, "[%s] start room[%s] ok!",
                            __FUNCTION__, strroomid.c_str() );
    }


    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleTellRoomFinishToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2STellRoomFinishToBattleShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] finish req!",
                            __FUNCTION__, strroomid.c_str() );

        // _kf_battle_manage->FreeBattleServer( kfmsg.serverid(), kfmsg.ip() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom != nullptr )
        {
            RemoveBattleRoom( kfroom );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                __FUNCTION__, strroomid.c_str() );
        }

        KFMsg::S2STellBattleRoomFinishAck ack;
        ack.set_roomid( kfmsg.roomid() );
        auto ok = _kf_cluster_shard->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_TELL_BATTLE_ROOM_FINISH_ACK, &ack );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] finish ok!",
                                __FUNCTION__, strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] finish failed!",
                                __FUNCTION__, strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandlePlayerOnlineToBattleShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerOnlineToBattleShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%u] online room[%s] req!",
                            __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Debug, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        auto ok = kfroom->PlayerOnlineBattleRoom( kfmsg.campid(), kfmsg.playerid(), kfmsg.serverid() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%u] online room[%s] ok!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] player[%u:%u] online room[%s] failed!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandlePlayerBattleScoreAck )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerBattleScoreAck );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%s] balance ack req!",
                            __FUNCTION__, kfmsg.playerid(), strroomid.c_str() );

        auto redisdriver = __BATTLE_REDIS_DRIVER__;
        auto ok = redisdriver->VoidExecute( "hdel %s:%u %s", KFField::_score.c_str(), kfmsg.playerid(), strroomid.c_str() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%s] balance ack ok!",
                                __FUNCTION__, kfmsg.playerid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%s] balance ack failed!",
                                __FUNCTION__, kfmsg.playerid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleBattleScoreBalanceToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SBattleScoreBalanceToShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] battle[%s] balance req!",
                            __FUNCTION__, strroomid.c_str() );

        auto kfroom = _kf_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find room[%s]!",
                                       __FUNCTION__, strroomid.c_str() );
        }

        auto pbscore = kfmsg.mutable_pbscore();
        auto ok = kfroom->BattleScoreBalance( pbscore );
        if ( ok )
        {
            // 保存到数据库
            auto redisdriver = __BATTLE_REDIS_DRIVER__;

            auto pbdata = pbscore->SerializeAsString();
            redisdriver->VoidExecute( "hset %s:%u %s %s",
                                      KFField::_score.c_str(), pbscore->playerid(), strroomid.c_str(), pbdata.c_str() );

            KFLogger::LogLogic( KFLogger::Debug, "[%s] battlr[%s] balance ok!",
                                __FUNCTION__, strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] battlr[%s] balance failed!",
                                __FUNCTION__, strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleShardModule::HandleOnlieQueryBattleScoreReq )
    {
        __PROTO_PARSE__( KFMsg::S2SOnlieQueryBattleScoreReq );
        auto redisdriver = __BATTLE_REDIS_DRIVER__;

        // 保存guid, 后面sendmessage 会改变kfguid的值
        auto guid = kfguid;

        MapString mapdata;
        redisdriver->MapExecute( mapdata, "hgetall %s:%u", KFField::_score.c_str(), kfmsg.playerid() );
        if ( mapdata.empty() )
        {
            return;
        }

        for ( auto iter : mapdata )
        {
            KFMsg::S2SPlayerBattleScoreReq req;
            req.set_playerid( kfmsg.playerid() );
            req.set_roomid( KFUtility::ToValue< uint64 >( iter.first ) );
            req.mutable_pbscore()->ParseFromString( iter.second );
            _kf_cluster_shard->SendMessageToClient( guid, KFMsg::S2S_PLAYER_BATTLE_SCORE_REQ, &req );
        }
    }
}