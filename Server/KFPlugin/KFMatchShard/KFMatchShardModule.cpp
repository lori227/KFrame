#include "KFMatchShardModule.h"

namespace KFrame
{
    void KFMatchShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_match_config, false );
    }

    void KFMatchShardModule::BeforeRun()
    {
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFMatchShardModule::OnClientConnectMatchMaster );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_MATCH_TO_SHARD_REQ, &KFMatchShardModule::HandleMatchToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_SHARD_REQ, &KFMatchShardModule::HandleCancelMatchToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_MATCH_SHARD_ACK, &KFMatchShardModule::HandleCreateRoomToMatchShardAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_START_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandleTellRoomStartToMatchShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_CAMP_TO_MATCH_SHARD_ACK, &KFMatchShardModule::HandleAddCampToMatchShardAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandlePlayerLeaveRoomToMatchShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandleQueryRoomToMatchShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_OPEN_ROOM_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandleOpenRoomToMatchShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_CLOSE_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandleTellRoomCloseToMatchShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_RESET_MATCH_ROOM_REQ, &KFMatchShardModule::HandleResetMatchRoomReq );
    }

    void KFMatchShardModule::OnceRun()
    {
        _battle_redis_driver = _kf_redis->Create( __KF_STRING__( battle ) );
        __REGISTER_LOOP_TIMER__( 0, 10000, &KFMatchShardModule::OnTimerQueryBattleVersion );
    }

    void KFMatchShardModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __KF_REMOVE_CONFIG__( _kf_match_config );
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_MATCH_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_MATCH_SHARD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_START_TO_MATCH_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_CAMP_TO_MATCH_SHARD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_MATCH_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_MATCH_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_OPEN_ROOM_TO_MATCH_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_CLOSE_TO_MATCH_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_RESET_MATCH_ROOM_REQ );
    }

    void KFMatchShardModule::Run()
    {
        for ( auto& iter : _kf_match_queue._objects )
        {
            auto kfmatchqueue = iter.second;

            kfmatchqueue->RunMatch();
        }
    }

    KFMatchQueue* KFMatchShardModule::FindMatchQueue( uint32 matchid, const char* function, uint32 line )
    {
        auto kfmatchqueue = _kf_match_queue.Find( matchid );
        if ( kfmatchqueue == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "can't find match queue[{}]!", matchid );
        }

        return kfmatchqueue;
    }

    __KF_CLIENT_CONNECT_FUNCTION__( KFMatchShardModule::OnClientConnectMatchMaster )
    {
        if ( servername == KFGlobal::Instance()->_app_name && servertype == __KF_STRING__( master ) )
        {
            std::set< uint64 > objectlist;

            for ( auto& iter : _kf_match_config->_kf_match_setting._objects )
            {
                auto kfsetting = iter.second;
                objectlist.insert( kfsetting->_match_id );
            }

            _kf_cluster_shard->AllocObjectToMaster( objectlist );
        }
    }

    __KF_TIMER_FUNCTION__( KFMatchShardModule::OnTimerQueryBattleVersion )
    {
        // 查询战斗服版本号
        auto kflist = _battle_redis_driver->QueryList( "smembers {}", __KF_STRING__( battleversionlist ) );
        if ( !kflist->IsOk() )
        {
            return;
        }

        _battle_version_list.clear();
        for ( auto& strversion : kflist->_value )
        {
            _battle_version_list.insert( strversion );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleMatchToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SMatchToShardReq );

        auto kfsetting = _kf_match_config->FindMatchSetting( kfmsg.matchid() );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR__( "can't find match setting[{}]!", kfmsg.matchid() );
        }

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            kfmatchqueue = _kf_match_queue.Create( kfmsg.matchid() );
            kfmatchqueue->_match_id = kfmsg.matchid();
            kfmatchqueue->_kf_setting = kfsetting;
        }

        // 先取消匹配
        auto pbgroup = &kfmsg.pbgroup();
        kfmatchqueue->CancelMatch( kfmsg.playerid() );

        auto result = KFMsg::MatchRequestSuccess;
        if ( _battle_version_list.empty() )
        {
            result = KFMsg::BattleNotPrepare;
        }
        else
        {
            KFVersion kfverson( kfmsg.version() );
            if ( _battle_version_list.count( kfverson._battle_version ) == 0u )
            {
                result = KFMsg::BattleNotCompatibility;
            }
            else
            {
                // 开始匹配
                kfmatchqueue->StartMatch( pbgroup, kfmsg.allowgroup(), kfmsg.battleserverid(), kfverson._battle_version );
            }
        }

        KFMsg::S2SMatchToClientAck ack;
        ack.set_matchid( kfmsg.matchid() );
        ack.set_playerid( kfmsg.playerid() );
        ack.set_result( result );
        _kf_cluster_shard->SendToClient( kfmsg.serverid(), KFMsg::S2S_MATCH_TO_CLIENT_ACK, &ack );

        __LOG_DEBUG__( "group[{}] match battleserverid[{}] allowgroup[{}] version[{}] result[{}]!",
                       pbgroup->groupid(), kfmsg.battleserverid(), kfmsg.allowgroup() ? 1 : 0, kfmsg.version(), result );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleQueryRoomToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRoomToMatchShardReq );
        __LOG_DEBUG__( "player[{}] query match[{}] req!", kfmsg.playerid(), kfmsg.matchid() );

        auto matchid = _invalid_int;
        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue != nullptr )
        {
            if ( kfmatchqueue->QueryMatchRoom( kfmsg.playerid(), kfmsg.serverid() ) )
            {
                matchid = kfmsg.matchid();
            }
        }

        // 查询匹配
        KFMsg::S2SQueryMatchRoomAck ack;
        ack.set_matchid( matchid );
        ack.set_playerid( kfmsg.playerid() );
        auto ok = _kf_cluster_shard->SendToClient( __KF_HEAD_ID__( kfid ), kfmsg.serverid(), KFMsg::S2S_QUERY_MATCH_ROOM_ACK, &ack );
        if ( ok )
        {
            __LOG_DEBUG__( "player[{}] query match[{}] ok!", kfmsg.playerid(), kfmsg.matchid() );
        }
        else
        {
            __LOG_ERROR__( "player[{}] query match[{}] failed!", kfmsg.playerid(), kfmsg.matchid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleCancelMatchToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCancelMatchToShardReq );

        __LOG_DEBUG__( "player[{}] cancel match[{}] req!", kfmsg.playerid(), kfmsg.matchid() );
        auto kfmatchqueue = FindMatchQueue( kfmsg.matchid(), __FUNC_LINE__ );
        if ( kfmatchqueue == nullptr )
        {
            return;
        }

        auto ok = kfmatchqueue->CancelMatch( kfmsg.playerid() );
        if ( !ok )
        {
            __LOG_DEBUG__( "player[{}] cancel match[{}] failed!", kfmsg.playerid(), kfmsg.matchid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleCreateRoomToMatchShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateRoomToMatchShardAck );
        __LOG_DEBUG__( "create room[{}:{}] ack!", kfmsg.matchid(), kfmsg.roomid() );

        auto kfmatchqueue = FindMatchQueue( kfmsg.matchid(), __FUNC_LINE__ );
        if ( kfmatchqueue == nullptr )
        {
            return;
        }

        auto ok = kfmatchqueue->CreateBattleRoomAck( kfmsg.roomid(), kfmsg.battleshardid() );
        if ( ok )
        {
            __LOG_DEBUG__( "create battle room[{}:{}] ok!", kfmsg.matchid(), kfmsg.roomid() );
        }
        else
        {
            __LOG_ERROR__( "create battle room[{}:{}] failed!", kfmsg.matchid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleAddCampToMatchShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SAddCampToMatchShardAck );

        __LOG_DEBUG__( "room[{}:{}] add camp[{}] result[{}] ack!",
                       kfmsg.matchid(), kfmsg.roomid(), kfmsg.campid(), ( kfmsg.addok() ? 1 : 0 ) );

        auto kfmatchqueue = FindMatchQueue( kfmsg.matchid(), __FUNC_LINE__ );
        if ( kfmatchqueue == nullptr )
        {
            return;
        }

        auto ok = kfmatchqueue->CampEnterBattleRoomAck( kfmsg.roomid(), kfmsg.campid(), kfmsg.addok() );
        if ( ok )
        {
            __LOG_DEBUG__( "room[{}:{}] add camp[{}] ok!", kfmsg.matchid(), kfmsg.roomid(), kfmsg.campid() );
        }
        else
        {
            __LOG_ERROR__( "room[{}:{}] add camp[{}] failed!", kfmsg.matchid(), kfmsg.roomid(), kfmsg.campid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleResetMatchRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SResetMatchRoomReq );

        __LOG_DEBUG__( "room[{}:{}] reset req!", kfmsg.matchid(), kfmsg.roomid() );

        auto kfmatchqueue = FindMatchQueue( kfmsg.matchid(), __FUNC_LINE__ );
        if ( kfmatchqueue == nullptr )
        {
            return;
        }

        auto ok = kfmatchqueue->ResetMatchRoom( kfmsg.roomid() );
        if ( ok )
        {
            __LOG_DEBUG__( "room[{}:{}] reset ok!", kfmsg.matchid(), kfmsg.roomid() );
        }
        else
        {
            __LOG_ERROR__( "room[{}:{}] reset failed!", kfmsg.matchid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleOpenRoomToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SOpenRoomToMatchShardReq );
        __LOG_DEBUG__( "room[{}:{}] open waittime[{}] req!", kfmsg.matchid(), kfmsg.roomid(), kfmsg.waittime() );

        auto kfmatchqueue = FindMatchQueue( kfmsg.matchid(), __FUNC_LINE__ );
        if ( kfmatchqueue == nullptr )
        {
            return;
        }

        auto ok = kfmatchqueue->TellMatchRoomOpen( kfmsg.roomid(), kfmsg.waittime() );
        if ( ok )
        {
            __LOG_DEBUG__( "room[{}:{}] open ok!", kfmsg.matchid(), kfmsg.roomid() );
        }
        else
        {
            __LOG_ERROR__( "room[{}:{}] open failed!", kfmsg.matchid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandlePlayerLeaveRoomToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerLeaveRoomToMatchShardReq );
        __LOG_DEBUG__( "[{}:{}] leave room[{}:{}] req!", kfmsg.campid(), kfmsg.playerid(), kfmsg.matchid(), kfmsg.roomid() );

        auto kfmatchqueue = FindMatchQueue( kfmsg.matchid(), __FUNC_LINE__ );
        if ( kfmatchqueue == nullptr )
        {
            return;
        }

        auto ok = kfmatchqueue->LeaveBattleRoom( kfmsg.roomid(), kfmsg.campid(), kfmsg.groupid(), kfmsg.playerid() );
        if ( ok )
        {
            __LOG_DEBUG__( "[{}:{}] leave room[{}:{}] ok!", kfmsg.campid(), kfmsg.playerid(), kfmsg.matchid(), kfmsg.roomid() );
        }
        else
        {
            __LOG_DEBUG__( "[{}:{}] leave room[{}:{}] failed!", kfmsg.campid(), kfmsg.playerid(), kfmsg.matchid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleTellRoomStartToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2STellRoomStartToMatchShardReq );
        __LOG_DEBUG__( "start room[{}:{}] req!", kfmsg.matchid(), kfmsg.roomid() );

        auto kfmatchqueue = FindMatchQueue( kfmsg.matchid(), __FUNC_LINE__ );
        if ( kfmatchqueue == nullptr )
        {
            return;
        }

        auto ok = kfmatchqueue->TellMatchRoomStart( kfmsg.roomid() );
        if ( ok )
        {
            __LOG_DEBUG__( "start room[{}:{}] ok!", kfmsg.matchid(), kfmsg.roomid() );
        }
        else
        {
            __LOG_ERROR__( "start room[{}:{}] failed!", kfmsg.matchid(), kfmsg.roomid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleTellRoomCloseToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2STellRoomCloseToMatchShardReq );

        auto kfmatchqueue = FindMatchQueue( kfmsg.matchid(), __FUNC_LINE__ );
        if ( kfmatchqueue == nullptr )
        {
            return;
        }

        kfmatchqueue->TellMatchRoomClose( kfmsg.roomid() );
    }

}