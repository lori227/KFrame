#include "KFBattleRoom.h"
#include "KFBattleConfig.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"

namespace KFrame
{
    KFBattleRoom::KFBattleRoom()
    {
        _match_id = 0;
        _match_proxy_id = 0;
        _match_shard_id = 0;
        _battle_room_id = 0;
        _now_player_count = 0;
        _max_player_count = 0;
        _battle_valid_time = 0;
        _status = 0;
        _req_count = 0;

    }

    KFBattleRoom::~KFBattleRoom()
    {

    }

    void KFBattleRoom::InitRoom( uint32 matchid, uint64 roomid, uint32 maxplayercount )
    {
        _match_id = matchid;
        _battle_room_id = roomid;
        _str_room_id = __KF_STRING__( _battle_room_id );
        _max_player_count = maxplayercount;
        _battle_valid_time = KFGlobal::Instance()->_game_time + _kf_battle_config->_room_valid_time;

        // 开启申请定时器
        UpdateRoomStatus( KFRoomStatus::StatusBattleRoomAlloc, 5000 );
    }

    void KFBattleRoom::UpdateRoomStatus( uint32 status, uint32 intervaltime )
    {
        _req_count = 0;
        _status = status;
        _status_timer.StopTimer();
        if ( intervaltime != 0 )
        {
            _status_timer.StartTimer( 1, intervaltime );
        }

        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] change status[%u] time[%u]!",
                            __FUNCTION__, _str_room_id.c_str(), status, intervaltime );
    }

    bool KFBattleRoom::IsBattleRoomStart()
    {
        switch ( _status )
        {
        case KFRoomStatus::StatisBattleRoomPlaying:
            return true;
            break;
        default:
            break;
        }

        return false;
    }

    bool KFBattleRoom::SendMessageToMatch( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_cluster_shard->SendMessageToClient( _match_proxy_id, _match_shard_id, msgid, message );
    }

    bool KFBattleRoom::SendMessageToBattle( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_cluster_shard->SendMessageToClient( _battle_server._proxy_id, _battle_server._server_id, msgid, message );
    }

    KFBattleCamp* KFBattleRoom::AddCamp( const KFMsg::PBBattleCamp* pbcamp )
    {
        // 已经开始, 或者人数已经满了
        if ( IsBattleRoomStart() )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] battle room[%s] is start!",
                                __FUNCTION__, _str_room_id.c_str() );
            return nullptr;
        }

        // 人数已经满了
        if ( _now_player_count + pbcamp->pbplayer_size() > _max_player_count )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] battle room[%s] is full!",
                                __FUNCTION__, _str_room_id.c_str() );
            return nullptr;
        }

        auto kfcamp = _kf_camp_list.Find( pbcamp->campid() );
        if ( kfcamp == nullptr )
        {
            kfcamp = __KF_CREATE_BATCH__( KFBattleCamp, 100 );
            kfcamp->LoadFrom( pbcamp );
            _kf_camp_list.Insert( kfcamp->_camp_id, kfcamp );

            // 玩家数量
            AddPlayerCount( kfcamp->PlayerCount() );
        }

        return kfcamp;
    }

    void KFBattleRoom::AddPlayerCount( uint32 playercount )
    {
        // 设置房间总人数
        _now_player_count += playercount;
        if ( _kf_camp_list.Size() < _kf_battle_config->_min_open_room_camp_count )
        {
            return;
        }

        // 已经分配好了战场服务器
        if ( _battle_server.IsValid() )
        {
            // 切掉换到开启战场状态
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomOpen, 6000 );
        }
    }

    bool KFBattleRoom::RemoveCamp( uint32 campid )
    {
        auto kfcamp = _kf_camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            return false;
        }

        _now_player_count -= __MIN__( _now_player_count, kfcamp->PlayerCount() );
        return true;
    }

    bool KFBattleRoom::CheckValid()
    {
        return KFGlobal::Instance()->_game_time < _battle_valid_time;
    }

    void KFBattleRoom::RunRoom()
    {
        // 刷新房间状态
        RunBattleRoomStatus();
    }

    void KFBattleRoom::RunBattleRoomStatus()
    {
        if ( !_status_timer.DoneTimer( KFGlobal::Instance()->_game_time, true ) )
        {
            return;
        }

        switch ( _status )
        {
        case KFRoomStatus::StatusBattleRoomAlloc:
        {
            // 分配战斗服务器
            AllocBatterServer();
        }
        break;
        case KFRoomStatus::StatusBattleRoomOpen:
        {
            // 通知战斗服务器开启战场`
            OpenBattleRoom();
        }
        break;
        case KFRoomStatus::StatusBattleRoomEnter:
        {
            // 玩家进入战场
            PlayerEnterBattleRoom();
        }
        break;
        case KFRoomStatus::StatisBattleRoomPlaying:
        {
            // 战场正在游戏
            BattleRoomPlaying();
        }
        break;
        default:
            break;
        }
    }

    void KFBattleRoom::AllocBatterServer()
    {
        // 分配战场
        _battle_server.Reset();
        _kf_battle_manage->AllocBattleServer( &_battle_server );
        if ( !_battle_server.IsValid() )
        {
            return KFLogger::LogLogic( KFLogger::Error, "[%s] room[%s] alloc battle server failed!",
                                       __FUNCTION__, _str_room_id.c_str() );
        }

        // 如果房间人数大于开启数量
        if ( _kf_camp_list.Size() < _kf_battle_config->_min_open_room_camp_count )
        {
            // 停止定时器
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomIdle, 0 );
        }
        else
        {
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomOpen, 6000 );
        }

        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] alloc battle[%u:%u:%s:%u] ok!",
                            __FUNCTION__, _str_room_id.c_str(), _battle_server._server_id, _battle_server._proxy_id, _battle_server._ip.c_str(), _battle_server._port );
    }

    void KFBattleRoom::OpenBattleRoom()
    {
        if ( _req_count > 10 )
        {
            return UpdateRoomStatus( KFRoomStatus::StatusBattleRoomAlloc, 5000 );
        }

        // 发送消息, 请求开启战场
        KFMsg::S2SOpenBattleRoomReq req;
        req.set_matchid( _match_id );
        req.set_roomid( _battle_room_id );
        req.set_maxplayercount( _max_player_count );
        req.set_battleshardid( KFGlobal::Instance()->_app_id );
        auto ok = SendMessageToBattle( KFMsg::S2S_OPEN_BATTLE_ROOM_REQ, &req );
        if ( ok )
        {
            ++_req_count;
            KFLogger::LogLogic( KFLogger::Debug, "[%s] open battle room[%s] req!",
                                __FUNCTION__, _str_room_id.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] open battle room[%s] req failed!",
                                __FUNCTION__, _str_room_id.c_str() );
        }
    }

    void KFBattleRoom::ConfirmOpenBattleRoom( bool opensuccess )
    {
        if ( !opensuccess )
        {
            // 开启失败, 重新分配一个房间
            _battle_server.Reset();
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomAlloc, 5000 );
        }
        else
        {
            // 玩家进入战场
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomEnter, 3000 );

            // 创建成功, 重新设置有效时间
            _battle_valid_time = KFGlobal::Instance()->_game_time + _kf_battle_config->_room_valid_time;

            // 通知匹配集群, 战场已经开启
            KFMsg::S2SOpenRoomToMatchShardReq req;
            req.set_matchid( _match_id );
            req.set_roomid( _battle_room_id );
            req.set_waittime( _kf_battle_config->_wait_enter_time );
            SendMessageToMatch( KFMsg::S2S_OPEN_ROOM_TO_MATCH_SHARD_REQ, &req );

            KFLogger::LogLogic( KFLogger::Debug, "[%s] open match room[%s] req!",
                                __FUNCTION__, _str_room_id.c_str() );
        }
    }

    void KFBattleRoom::ConfirmOpenMatchRoom()
    {

    }

    void KFBattleRoom::PlayerEnterBattleRoom()
    {
        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            kfcamp->RunEnterBattleRoom( this );
            kfcamp = _kf_camp_list.Next();
        }
    }

    bool KFBattleRoom::ConfirmEnterBattleRoom( uint32 campid, uint32 playerid )
    {
        auto kfplayer = FindBattlePlayer( campid, playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        kfplayer->ConfirmEnterBattleRoom();
        return true;
    }

    bool KFBattleRoom::LoginBattleRoom( uint32 campid, uint32 playerid )
    {
        auto kfplayer = FindBattlePlayer( campid, playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        return kfplayer->LoginBattleRoomReq( this );
    }

    bool KFBattleRoom::LeaveBattleRoom( uint32 campid, uint32 playerid )
    {
        auto kfplayer = FindBattlePlayer( campid, playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        kfplayer->LeaveBattleRoomReq( this );
        return true;
    }

    bool KFBattleRoom::CancelMatch( uint32 campid, uint32 playerid )
    {
        // 已经开始了, 不处理
        if ( _status == KFRoomStatus::StatisBattleRoomPlaying )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s:%u] room[%s] alread start!",
                                __FUNCTION_LINE__, _str_room_id.c_str() );
            return false;
        }

        auto kfcamp = _kf_camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s:%u] room[%s] can't find camp[%u]!",
                                __FUNCTION_LINE__, _str_room_id.c_str(), campid );
            return false;
        }

        // 发送到战场服务器
        if ( _status != KFRoomStatus::StatusBattleRoomOpen )
        {
            _now_player_count -= __MIN__( kfcamp->PlayerCount(), _now_player_count );
            _kf_camp_list.Remove( campid );
        }
        else
        {
            kfcamp->_kf_player_list.Remove( playerid );
            _now_player_count -= __MIN__( 1, _now_player_count );
            if ( kfcamp->PlayerCount() == 0 )
            {
                _kf_camp_list.Remove( campid );
            }

            // 发送消息到游戏房间
            KFMsg::S2SPlayerCancelMatchReq req;
            req.set_roomid( _battle_room_id );
            req.set_campid( campid );
            req.set_playerid( playerid );
            auto ok = SendMessageToBattle( KFMsg::S2S_PLAYER_CANCEL_MATCH_REQ, &req );
            if ( !ok )
            {
                KFLogger::LogLogic( KFLogger::Error, "[%s:%u] room[%s] player[%u:%u] cancel battle failed!",
                                    __FUNCTION_LINE__, _str_room_id.c_str(), campid, playerid );
            }
        }

        return true;
    }

    bool KFBattleRoom::NoticeBattleRoom( uint32 campid, uint32 playerid )
    {
        auto kfplayer = FindBattlePlayer( campid, playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        kfplayer->ConfirmNoticeBattleRoom();
        return true;
    }

    bool KFBattleRoom::QueryBattleRoom( uint32 playerid, uint32 serverid )
    {
        auto kfplayer = FindBattlePlayer( playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        return kfplayer->QueryBattleRoom( serverid );
    }

    void KFBattleRoom::UpdateBattleRoom( uint32 proxyid, uint32 serverid, const std::string& ip, uint32 port )
    {
        // 删除一个战场
        if ( _battle_server.IsValid() )
        {
            _kf_battle_manage->RemoveBattleServer( serverid, ip );
        }

        _battle_server._proxy_id = proxyid;
        _battle_server._server_id = serverid;
        _battle_server._ip = ip;
        _battle_server._port = port;

        // 如果是进入状态, 重置到开启状态, 让流程循环起来
        if ( _status == KFRoomStatus::StatusBattleRoomAlloc ||
                _status == KFRoomStatus::StatusBattleRoomEnter )
        {
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomOpen, 5000 );
        }
    }

    void KFBattleRoom::StartBattleRoom()
    {
        // 设置房间已经开始
        UpdateRoomStatus( KFRoomStatus::StatisBattleRoomPlaying, 5000 );
    }

    void KFBattleRoom::BattleRoomPlaying()
    {
        // 通知匹配集群
        KFMsg::S2STellRoomStartToMatchShardReq req;
        req.set_matchid( _match_id );
        req.set_roomid( _battle_room_id );
        SendMessageToMatch( KFMsg::S2S_TELL_ROOM_START_TO_MATCH_SHARD_REQ, &req );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] start room[%s] to match req!",
                            __FUNCTION__, _str_room_id.c_str() );
    }

    void KFBattleRoom::ConfirmStartBattleRoom()
    {
        // 停止定时器
        UpdateRoomStatus( KFRoomStatus::StatisBattleRoomPlaying, 0 );
    }

    void KFBattleRoom::FinishBattleRoom()
    {
        // 所有的玩家离开
        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            kfcamp->FinishLeaveBattleRoom();
            kfcamp = _kf_camp_list.Next();
        }

        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] battle[%u|%s:%u] finish ok!",
                            __FUNCTION__, _str_room_id.c_str(), _battle_server._server_id, _battle_server._ip.c_str(), _battle_server._port );
    }

    void KFBattleRoom::FreeBattleServer()
    {
        _kf_battle_manage->FreeBattleServer( &_battle_server );

        // 通知proxy
        _kf_cluster_shard->RemoveObjectToProxy( _battle_room_id );

        // 关闭战场到匹配集群
        KFMsg::S2STellRoomCloseToMatchShardReq req;
        req.set_matchid( _match_id );
        req.set_roomid( _battle_room_id );
        auto ok = SendMessageToMatch( KFMsg::S2S_TELL_ROOM_CLOSE_TO_MATCH_SHARD_REQ, &req );
        if ( ok )
        {
            KFLogger::LogLogic( KFLogger::Debug, "[%s] close room[%s] to match req!",
                                __FUNCTION__, _str_room_id.c_str() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] close room[%s] to match req failed!",
                                __FUNCTION__, _str_room_id.c_str() );
        }

    }

    KFBattlePlayer* KFBattleRoom::FindBattlePlayer( uint32 playerid )
    {
        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            auto kfplayer = kfcamp->_kf_player_list.Find( playerid );
            if ( kfplayer != nullptr )
            {
                return kfplayer;
            }

            kfcamp = _kf_camp_list.Next();
        }

        return nullptr;
    }

    KFBattlePlayer* KFBattleRoom::FindBattlePlayer( uint32 campid, uint32 playerid )
    {
        auto kfcamp = _kf_camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            return nullptr;
        }

        return kfcamp->_kf_player_list.Find( playerid );
    }

    bool KFBattleRoom::PlayerOnlineBattleRoom( uint32 campid, uint32 playerid, uint32 serverid )
    {
        auto kfplayer = FindBattlePlayer( campid, playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        kfplayer->OnlineBattleRoom( serverid );
        return true;
    }

    bool KFBattleRoom::BattleScoreBalance( KFMsg::PBBattleScore* pbscore )
    {
        auto kfplayer = FindBattlePlayer( pbscore->playerid() );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        return kfplayer->BattleScoreBalance( this, pbscore );
    }
}

