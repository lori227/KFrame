#include "KFMatchShardModule.h"

namespace KFrame
{
    KFMatchShardModule::KFMatchShardModule()
    {
    }

    KFMatchShardModule::~KFMatchShardModule()
    {
    }

    void KFMatchShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_match_config, false );
    }

    void KFMatchShardModule::BeforeRun()
    {
        __REGISTER_RUN_FUNCTION__( &KFMatchShardModule::Run );
        _kf_tcp_server->RegisterDiscoverFunction( this, &KFMatchShardModule::OnServerDiscoverMatchProxy );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_MATCH_TO_SHARD_REQ, &KFMatchShardModule::HandleMatchToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_SHARD_REQ, &KFMatchShardModule::HandleCancelMatchToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_MATCH_SHARD_ACK, &KFMatchShardModule::HandleCreateRoomToMatchShardAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_START_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandleTellRoomStartToMatchShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_CAMP_TO_MATCH_SHARD_ACK, &KFMatchShardModule::HandleAddCampToMatchShardAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandlePlayerLeaveRoomToMatchShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandleQueryRoomToMatchShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_OPEN_ROOM_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandleOpenRoomToMatchShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_MATCH_SHARD_ACK, &KFMatchShardModule::HandleCancelMatchToMatchShardAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_CLOSE_TO_MATCH_SHARD_REQ, &KFMatchShardModule::HandleTellRoomCloseToMatchShardReq );
    }

    void KFMatchShardModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_RUN_FUNCTION__();
        _kf_tcp_server->UnRegisterDiscoverFunction( this );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_MATCH_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_MATCH_SHARD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_START_TO_MATCH_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_CAMP_TO_MATCH_SHARD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_MATCH_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_MATCH_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_OPEN_ROOM_TO_MATCH_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_MATCH_SHARD_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_ROOM_CLOSE_TO_MATCH_SHARD_REQ );
    }

    void KFMatchShardModule::Run()
    {
        for ( auto& iter : _kf_match_queue._objects )
        {
            auto kfmatchqueue = iter.second;

            kfmatchqueue->RunMatch();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_DISCOVER_FUNCTION__( KFMatchShardModule::OnServerDiscoverMatchProxy )
    {
        KFMsg::S2SRegisterMatchReq req;

        for ( auto& iter : _kf_match_config->_kf_match_setting._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->IsOpen( KFGlobal::Instance()->_app_id ) )
            {
                req.add_matchid( kfsetting->_match_id );
            }
        }

        _kf_cluster_shard->SendMessageToProxy( handleid, KFMsg::S2S_REGISTER_MATCH_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleMatchToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SMatchToShardReq );

        auto kfsetting = _kf_match_config->FindMatchSetting( kfmsg.matchid() );
        if ( kfsetting == nullptr )
        {
            return KFLogger::LogSystem( KFLogger::Error, "[%s] can't find matchsetting[%u]!",
                                        __FUNCTION__, kfmsg.matchid() );
        }

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            kfmatchqueue = _kf_match_queue.Create( kfmsg.matchid() );
            kfmatchqueue->_match_id = kfmsg.matchid();
            kfmatchqueue->_kf_setting = kfsetting;
            kfmatchqueue->_kf_match_module = this;
        }

        auto pbgroup = &kfmsg.pbgroup();
        kfmatchqueue->StartMatch( pbgroup, kfmsg.allowgroup() );

        KFMsg::S2SMatchToClientAck ack;
        ack.set_matchid( kfmsg.matchid() );
        ack.set_playerid( kfmsg.playerid() );
        ack.set_result( KFMsg::MatchRequestSuccess );
        _kf_cluster_shard->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_MATCH_TO_CLIENT_ACK, &ack );

        auto strgroupid = __KF_STRING__( pbgroup->groupid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] match allowgroup[%u]!",
                            __FUNCTION__, strgroupid.c_str(), kfmsg.allowgroup() ? 1 : 0 );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleQueryRoomToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRoomToMatchShardReq );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] query match[%u] req!",
                            __FUNCTION__, kfmsg.playerid(), kfmsg.matchid() );

        auto proxyid = __KF_HEAD_ID__( kfguid );
        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find match queue[%u]!",
                                       __FUNCTION__, kfmsg.matchid() );
        }

        // 查询匹配
        KFMsg::S2SQueryMatchRoomAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_matchid( _invalid_int );
        if ( kfmatchqueue->QueryBattleRoom( kfmsg.playerid(), kfmsg.serverid() ) )
        {
            ack.set_matchid( kfmsg.matchid() );
        }

        auto ok = _kf_cluster_shard->SendMessageToClient( proxyid, kfmsg.serverid(), KFMsg::S2S_QUERY_MATCH_ROOM_ACK, &ack );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] query match[%u] ok!",
                                __FUNCTION__, kfmsg.playerid(), kfmsg.matchid() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] query match[%u] failed!",
                                __FUNCTION__, kfmsg.playerid(), kfmsg.matchid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleCancelMatchToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCancelMatchToShardReq );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] cancel match[%u] req!",
                            __FUNCTION__, kfmsg.playerid(), kfmsg.matchid() );

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find match queue[%u]!",
                                       __FUNCTION__, kfmsg.matchid() );
        }

        auto ok = kfmatchqueue->CancelMatchReq( kfmsg.playerid() );
        if ( !ok )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] player[%u] cancel match[%u] failed!",
                                __FUNCTION__, kfmsg.playerid(), kfmsg.matchid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleCancelMatchToMatchShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SCancelMatchToMatchShardAck );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%u:%s] player[%u:%u] cancel match ack!",
                            __FUNCTION__, kfmsg.matchid(), strroomid.c_str(), kfmsg.campid(), kfmsg.playerid() );

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find match queue[%u]!",
                                       __FUNCTION__, kfmsg.matchid() );
        }

        auto ok = kfmatchqueue->CancelMatchAck( kfmsg.roomid(), kfmsg.campid(), kfmsg.playerid(), kfmsg.isroomopen() );
        if ( !ok )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] room[%u:%s] player[%u:%u] cancel match failed!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str(), kfmsg.campid(), kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleCreateRoomToMatchShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateRoomToMatchShardAck );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] create room[%u:%s] ack!",
                            __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find match queue[%u]!",
                                       __FUNCTION__, kfmsg.matchid() );
        }

        auto result = kfmatchqueue->CreateBattleRoom( kfmsg.roomid(), kfmsg.battleshardid() );
        if ( !result )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] create battle room[%u:%s] failed!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleAddCampToMatchShardAck )
    {
        __PROTO_PARSE__( KFMsg::S2SAddCampToMatchShardAck );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%u:%s] add camp[%u] result[%u] ack!",
                            __FUNCTION__, kfmsg.matchid(), strroomid.c_str(), kfmsg.campid(), ( kfmsg.addok() ? 1 : 0 ) );

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find match queue[%u]!",
                                       __FUNCTION__, kfmsg.matchid() );
        }

        auto ok = kfmatchqueue->EnterBattleRoom( kfmsg.roomid(), kfmsg.campid(), kfmsg.addok() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%u:%s] add camp[%u] ok!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str(), kfmsg.campid() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] room[%u:%s] add camp[%u] failed!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str(), kfmsg.campid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleOpenRoomToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SOpenRoomToMatchShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%u:%s] open req!",
                            __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] room[%s] can't find match queue[%u]!",
                                       __FUNCTION__, strroomid.c_str(), kfmsg.matchid() );
        }

        auto ok = kfmatchqueue->OpenBattleRoom( kfmsg.roomid(), kfmsg.waittime() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%u:%s] open ok!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] room[%u:%s] open failed!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandlePlayerLeaveRoomToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerLeaveRoomToMatchShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] [%u:%u] leave room[%u:%s] req!",
                            __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), kfmsg.matchid(), strroomid.c_str() );

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find match queue[%u]!",
                                       __FUNCTION__, kfmsg.matchid() );
        }

        auto ok = kfmatchqueue->LeaveBattleRoom( kfmsg.roomid(), kfmsg.campid(), kfmsg.playerid() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] [%u:%u] leave room[%u:%s] ok!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), kfmsg.matchid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] [%u:%u] leave room[%u:%s] failed!",
                                __FUNCTION__, kfmsg.campid(), kfmsg.playerid(), kfmsg.matchid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleTellRoomStartToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2STellRoomStartToMatchShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Debug, "[%s] start room[%u:%s] req!",
                            __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find match queue[%u]!",
                                       __FUNCTION__, kfmsg.matchid() );
        }

        auto ok = kfmatchqueue->StartBattleRoom( kfmsg.roomid() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] start room[%u:%s] ok!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] start room[%u:%s] failed!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleTellRoomCloseToMatchShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2STellRoomCloseToMatchShardReq );
        auto strroomid = __KF_STRING__( kfmsg.roomid() );
        KFLogger::LogLogic( KFLogger::Error, "[%s] close room[%u:%s] req!",
                            __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );

        auto kfmatchqueue = _kf_match_queue.Find( kfmsg.matchid() );
        if ( kfmatchqueue == nullptr )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] can't find match queue[%u]!",
                                       __FUNCTION__, kfmsg.matchid() );
        }

        auto ok = kfmatchqueue->CloseBattleRoom( kfmsg.roomid() );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] close room[%u:%s] ok!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] start room[%u:%s] failed!",
                                __FUNCTION__, kfmsg.matchid(), strroomid.c_str() );
        }
    }

}