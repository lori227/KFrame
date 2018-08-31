#include "KFBattleRoom.h"
#include "KFBattleConfig.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFOption/KFOptionInterface.h"

namespace KFrame
{
    KFBattleRoom::KFBattleRoom()
    {
        _match_id = 0;
        _match_proxy_id = 0;
        _match_shard_id = 0;
        _battle_room_id = 0;
        _total_player_count = 0;
        _max_player_count = 0;
        _status = 0;
        _req_count = 0;
        _battle_server_id = 0;
        _battle_valid_time = _invalid_int;
    }

    KFBattleRoom::~KFBattleRoom()
    {

    }

    void KFBattleRoom::InitRoom( uint32 matchid, uint64 roomid, uint32 battleserverid, uint32 maxplayercount )
    {
        _match_id = matchid;
        _battle_room_id = roomid;
        _max_player_count = maxplayercount;
        _battle_server_id = battleserverid;

        // 设置有效时间
        SetValidTime();

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

        __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] change status[{}] time[{}]!", _battle_room_id, status, intervaltime );
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
            __LOG_DEBUG__( KFLogEnum::Logic, "battle room[{}] is start!", _battle_room_id );
            return nullptr;
        }

        //// 人数已经满了
        //if ( _total_player_count + pbcamp->pbplayer_size() > _max_player_count )
        //{
        //    __LOG_ERROR__( KFLogEnum::Logic, "battle room[{}] is full!", _battle_room_id );
        //    return nullptr;
        //}

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

    bool KFBattleRoom::CheckCanOpenBattleRoom()
    {
        static auto _min_open_room_camp_count = _kf_option->GetValue< uint32 >( "minopenroomcount" );
        return _kf_camp_list.Size() >= _min_open_room_camp_count;
    }

    void KFBattleRoom::AddPlayerCount( uint32 playercount )
    {
        // 设置房间总人数
        _total_player_count += playercount;
        __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] playercount[{}]!", _battle_room_id, _total_player_count );
        if ( !CheckCanOpenBattleRoom() )
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

        _total_player_count -= __MIN__( _total_player_count, kfcamp->PlayerCount() );
        __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] playercount[{}]!", _battle_room_id, _total_player_count );
        return true;
    }


    void KFBattleRoom::SetValidTime()
    {
        static auto _room_valid_time = _kf_option->GetValue< uint32 >( "roomvalidtime" ) * KFTimeEnum::OneSecondMicSecond;
        _battle_valid_time = KFGlobal::Instance()->_game_time + _room_valid_time;
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
        _kf_battle_manage->AllocBattleServer( _battle_server_id, &_battle_server );
        if ( !_battle_server.IsValid() )
        {
            return __LOG_ERROR__( KFLogEnum::Logic, "room[{}] alloc battle server failed!", _battle_room_id );
        }

        // 如果房间人数大于开启数量
        if ( CheckCanOpenBattleRoom() )
        {
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomOpen, 6000 );
        }
        else
        {
            // 停止定时器, 等待玩家加入
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomIdle, 0 );
        }

        __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] alloc battle[{}:{}:{}:{}] ok!", _battle_room_id,
                       _battle_server._server_id, _battle_server._proxy_id, _battle_server._ip, _battle_server._port );
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
            __LOG_DEBUG__( KFLogEnum::Logic, "open battle room[{}] req!", _battle_room_id );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Logic, "open battle room[{}] req failed!", _battle_room_id );
        }
    }

    void KFBattleRoom::ConfirmOpenBattleRoom( bool opensuccess, uint32 waittime )
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
            SetValidTime();

            // 通知匹配集群, 战场已经开启
            KFMsg::S2SOpenRoomToMatchShardReq req;
            req.set_matchid( _match_id );
            req.set_roomid( _battle_room_id );
            req.set_waittime( waittime );
            SendMessageToMatch( KFMsg::S2S_OPEN_ROOM_TO_MATCH_SHARD_REQ, &req );

            __LOG_DEBUG__( KFLogEnum::Logic, "open match room[{}] req!", _battle_room_id );
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

    bool KFBattleRoom::CancelMatch( uint32 campid, uint64 groupid )
    {
        if ( _status == KFRoomStatus::StatisBattleRoomPlaying )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "room[{}] is playing[{}]!", _battle_room_id, campid );
            return false;
        }

        auto kfcamp = _kf_camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "room[{}] can't find camp[{}]!", _battle_room_id, campid );
            return false;
        }

        // 通知战场
        if ( _battle_server._server_id != _invalid_int )
        {
            for ( auto& iter : kfcamp->_kf_player_list._objects )
            {
                auto kfplayer = iter.second;

                KFMsg::S2SPlayerCancelMatchReq req;
                req.set_roomid( _battle_room_id );
                req.set_campid( campid );
                req.set_playerid( kfplayer->GetID() );
                SendMessageToBattle( KFMsg::S2S_PLAYER_CANCEL_MATCH_REQ, &req );
            }
        }

        // 删除阵营
        _total_player_count -= __MIN__( kfcamp->PlayerCount(), _total_player_count );
        _kf_camp_list.Remove( campid );

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
        if ( _status == KFRoomStatus::StatusBattleRoomAlloc || _status == KFRoomStatus::StatusBattleRoomEnter )
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

        __LOG_DEBUG__( KFLogEnum::Logic, "start room[{}] to match req!", _battle_room_id );
    }

    void KFBattleRoom::ConfirmStartBattleRoom()
    {
        // 停止定时器
        UpdateRoomStatus( KFRoomStatus::StatisBattleRoomPlaying, 0 );

        __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] playercount[{}] start!", _battle_room_id, _total_player_count );
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

        __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] battle[{}|{}:{}] finish ok!", _battle_room_id,
                       _battle_server._server_id, _battle_server._ip, _battle_server._port );
    }

    void KFBattleRoom::FreeInValidRoom()
    {
        __LOG_ERROR__( KFLogEnum::Logic, "room[{}] battle[{}:{}] invalid!", _battle_room_id,
                       _battle_server._server_id, _battle_server._ip );

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
            __LOG_DEBUG__( KFLogEnum::Logic, "close room[{}] to match req!", _battle_room_id );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Logic, "close room[{}] to match req failed!", _battle_room_id );
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

