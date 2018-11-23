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
        _last_status = 0;
        _req_count = 0;
        _battle_server_id = 0;
        _battle_valid_time = _invalid_int;
        _battle_redis_driver = nullptr;
        _is_match_room_open = false;
        _battle_wait_time = 0;
        _last_interval_time = 0;
    }

    KFBattleRoom::~KFBattleRoom()
    {
    }

    void KFBattleRoom::InitRoom( uint32 matchid, uint64 roomid, uint32 battleserverid, uint32 maxplayercount, const std::string& version )
    {
        _match_id = matchid;
        _battle_room_id = roomid;
        _max_player_count = maxplayercount;
        _battle_server_id = battleserverid;
        _battle_version = version;

        // 设置有效时间
        SetValidTime();

        // 开启申请定时器
        UpdateRoomStatus( KFRoomStatus::StatusBattleRoomAlloc, 5000 );

        _battle_redis_driver = _kf_redis->CreateExecute( __KF_STRING__( battle ) );
    }

    void KFBattleRoom::UpdateRoomStatus( uint32 status, uint32 intervaltime )
    {
        _req_count = 0;
        _last_status = _status;
        _status = status;
        _last_interval_time = intervaltime;
        _status_timer.StopTimer();
        if ( intervaltime != 0 )
        {
            _status_timer.StartTimer( 1, intervaltime );
        }

        __LOG_DEBUG__( "room[{}] change status[{}] time[{}]!", _battle_room_id, status, intervaltime );
    }

    bool KFBattleRoom::IsBattleRoomStart()
    {
        switch ( _status )
        {
        case KFRoomStatus::StatusBattleRoomPlaying:
            return true;
            break;
        default:
            break;
        }

        return false;
    }

    bool KFBattleRoom::SendMessageToMatch( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_cluster_shard->SendToClient( _match_proxy_id, _match_shard_id, msgid, message );
    }

    bool KFBattleRoom::SendMessageToBattle( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_cluster_shard->SendToClient( _battle_server._proxy_id, _battle_server._server_id, msgid, message );
    }

    void KFBattleRoom::SendMessageToRoom( uint32 msgid, google::protobuf::Message* message )
    {
        for ( auto& iter : _kf_camp_list._objects )
        {
            auto kfcamp = iter.second;
            kfcamp->SendMessageToCamp( msgid, message );
        }
    }

    KFBattleCamp* KFBattleRoom::AddCamp( const KFMsg::PBBattleCamp* pbcamp )
    {
        // 已经开始, 或者人数已经满了
        if ( IsBattleRoomStart() )
        {
            __LOG_DEBUG__( "battle room[{}] is start!", _battle_room_id );
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

    bool KFBattleRoom::CheckCanOpenBattleRoom()
    {
        static auto _min_open_room_camp_count_option = _kf_option->FindOption( __KF_STRING__( minopenroomcount ), _match_id );
        return _kf_camp_list.Size() >= _min_open_room_camp_count_option->_uint32_value;
    }

    void KFBattleRoom::AddPlayerCount( uint32 playercount )
    {
        // 设置房间总人数
        _total_player_count += playercount;
        __LOG_DEBUG__( "room[{}] playercount[{}]!", _battle_room_id, _total_player_count );
        if ( !CheckCanOpenBattleRoom() )
        {
            return;
        }

        // 已经分配好了战场服务器
        if ( _battle_server.IsValid() )
        {
            // 切掉换到开启战场状态
            if ( _status == KFRoomStatus::StatusBattleRoomIdle || _status == KFRoomStatus::StatusBattleRoomAlloc )
            {
                UpdateRoomStatus( KFRoomStatus::StatusBattleRoomOpen, 6000 );
            }
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
        _kf_camp_list.Remove( campid );
        __LOG_DEBUG__( "room[{}] playercount[{}]!", _battle_room_id, _total_player_count );
        return true;
    }


    void KFBattleRoom::SetValidTime()
    {
        static auto _room_valid_time_option = _kf_option->FindOption( __KF_STRING__( roomvalidtime ) );
        _battle_valid_time = KFGlobal::Instance()->_game_time + _room_valid_time_option->_uint32_value;
    }

    bool KFBattleRoom::CheckValid()
    {
        if ( _status == KFRoomStatus::StatusBattleRoomFinish )
        {
            return false;
        }

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
            // 分配战斗服务器
            AllocBatterServer();
            break;
        case KFRoomStatus::StatusBattleRoomOpen:
            // 通知战斗服务器开启战场`
            OpenBattleRoom();
            break;
        case KFRoomStatus::StatusBattleRoomEnter:
            // 玩家进入战场
            PlayerEnterBattleRoom();
            break;
        case KFRoomStatus::StatusBattleRoomPlaying:
            // 战场正在游戏
            BattleRoomPlaying();
            break;
        case KFRoomStatus::StatusBattleRoomDisconnect:
            BattleRoomDisconnect();
            break;
        default:
            break;
        }
    }

    void KFBattleRoom::AllocBatterServer()
    {
        // 分配战场
        _battle_server.Reset();
        _kf_battle_manage->AllocBattleServer( _battle_server_id, _battle_version, & _battle_server );
        if ( !_battle_server.IsValid() )
        {
            __LOG_ERROR__( "version[{}] room[{}] alloc battle server failed!", _battle_version, _battle_room_id );

            // 通知玩家等待时间
            auto kfresult = _battle_redis_driver->QueryMap( "zrange {} 0 0 withscores", __KF_STRING__( battletime ) );
            if ( kfresult->IsOk() )
            {
                auto waittime = std::numeric_limits<uint32>::max();
                if ( !kfresult->_value.empty() )
                {
                    auto begin = kfresult->_value.begin();
                    auto endtime = KFUtility::ToValue< uint64 >( begin->second );
                    if ( KFGlobal::Instance()->_real_time > endtime )
                    {
                        // 删除时间
                        _battle_redis_driver->Execute( "zrem {} {}", __KF_STRING__( battletime ), begin->first );
                    }
                    else
                    {
                        waittime = endtime - KFGlobal::Instance()->_real_time;
                    }
                }

                KFMsg::MsgTellMatchWaitTime tell;
                tell.set_waittime( waittime );
                SendMessageToRoom( KFMsg::MSG_TELL_MATCH_WAIT_TIME, &tell );
            }
        }
        else
        {
            __LOG_DEBUG__( "version[{}] room[{}] alloc battle[{}:{}:{}:{}] ok!", _battle_version, _battle_room_id,
                           _battle_server._server_id, _battle_server._proxy_id, _battle_server._ip, _battle_server._port );

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
        }
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
            __LOG_DEBUG__( "open battle room[{}] req!", _battle_room_id );
        }
        else
        {
            __LOG_ERROR__( "open battle room[{}] req failed!", _battle_room_id );
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
            // 创建成功, 重新设置有效时间
            SetValidTime();

            // 设置等待时间
            _is_match_room_open = false;
            _battle_wait_time = waittime;

            // 玩家进入战场
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomEnter, 3000 );
        }
    }

    void KFBattleRoom::ConfirmOpenMatchRoom()
    {
        _is_match_room_open = true;
    }

    void KFBattleRoom::PlayerEnterBattleRoom()
    {
        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            kfcamp->RunEnterBattleRoom( this );
            kfcamp = _kf_camp_list.Next();
        }

        if ( !_is_match_room_open )
        {
            // 通知匹配集群, 战场已经开启
            KFMsg::S2SOpenRoomToMatchShardReq req;
            req.set_matchid( _match_id );
            req.set_roomid( _battle_room_id );
            req.set_waittime( _battle_wait_time );
            SendMessageToMatch( KFMsg::S2S_OPEN_ROOM_TO_MATCH_SHARD_REQ, &req );

            __LOG_DEBUG__( "open match room[{}] req!", _battle_room_id );
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
        if ( _status == KFRoomStatus::StatusBattleRoomPlaying )
        {
            __LOG_ERROR__( "room[{}] is playing[{}]!", _battle_room_id, campid );
            return false;
        }

        auto kfcamp = _kf_camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            __LOG_ERROR__( "room[{}] can't find camp[{}]!", _battle_room_id, campid );
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

        return kfplayer->QueryBattleRoom( this, serverid );
    }

    bool KFBattleRoom::UpdateBattleRoom( uint32 proxyid, uint32 serverid, const std::string& ip, uint32 port, const std::string& version )
    {
        if ( _battle_server.IsValid() )
        {
            // 已经另外分配战场服务器了
            if ( serverid != _battle_server._server_id )
            {
                return false;
            }

            // 删除redis记录
            _kf_battle_manage->RemoveBattleServer( serverid, version );
        }

        _battle_server._proxy_id = proxyid;
        _battle_server._server_id = serverid;
        _battle_server._ip = ip;
        _battle_server._port = port;
        _battle_server._version = version;

        // 如果是进入状态, 重置到开启状态, 让流程循环起来
        switch ( _last_status )
        {
        case KFRoomStatus::StatusBattleRoomAlloc:
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomOpen, 5000 );
            break;
        default:
            UpdateRoomStatus( _last_status, _last_interval_time );
            break;
        }

        return true;
    }

    void KFBattleRoom::DisconnectBattleRoom()
    {
        // 如果正在进入状态, 重新分配
        switch ( _status )
        {
        case KFRoomStatus::StatusBattleRoomIdle:
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomAlloc, 5000 );
            break;
        case KFRoomStatus::StatusBattleRoomEnter:
        case KFRoomStatus::StatusBattleRoomPlaying:
            UpdateRoomStatus( KFRoomStatus::StatusBattleRoomDisconnect, 60000 );
            break;
        default:
            break;
        }
    }

    void KFBattleRoom::BattleRoomDisconnect()
    {
        __LOG_ERROR__( "room[{}] [{}] [{}:{}] disconnect finish!", _battle_room_id, KFAppID::ToString( _battle_server._server_id ), _battle_server._ip, _battle_server._port );

        // 结束房间
        FinishBattleRoom();

        // 更新到结束
        UpdateRoomStatus( KFRoomStatus::StatusBattleRoomFinish, 0 );
    }

    void KFBattleRoom::StartBattleRoom( uint32 maxtime )
    {
        // 设置房间已经开始
        UpdateRoomStatus( KFRoomStatus::StatusBattleRoomPlaying, 5000 );

        // 保存结束时间
        auto endtime = KFGlobal::Instance()->_real_time + maxtime + 10;
        _battle_redis_driver->Execute( "zadd {} {} {}", __KF_STRING__( battletime ), endtime, _battle_room_id );
    }

    void KFBattleRoom::BattleRoomPlaying()
    {
        // 通知匹配集群
        KFMsg::S2STellRoomStartToMatchShardReq req;
        req.set_matchid( _match_id );
        req.set_roomid( _battle_room_id );
        SendMessageToMatch( KFMsg::S2S_TELL_ROOM_START_TO_MATCH_SHARD_REQ, &req );

        __LOG_DEBUG__( "start room[{}] to match req!", _battle_room_id );
    }

    void KFBattleRoom::ConfirmStartBattleRoom()
    {
        // 停止定时器
        UpdateRoomStatus( KFRoomStatus::StatusBattleRoomPlaying, 0 );

        __LOG_DEBUG__( "room[{}] playercount[{}] start!", _battle_room_id, _total_player_count );
    }

    void KFBattleRoom::FinishBattleRoom()
    {
        // 所有的玩家离开
        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            kfcamp->FinishLeaveBattleRoom( this );
            kfcamp = _kf_camp_list.Next();
        }

        // 删除时间
        _battle_redis_driver->Execute( "zrem {} {}", __KF_STRING__( battletime ), _battle_room_id );

        __LOG_DEBUG__( "room[{}] battle[{}|{}:{}] finish ok!", _battle_room_id,
                       _battle_server._server_id, _battle_server._ip, _battle_server._port );
    }

    void KFBattleRoom::FreeInValidRoom()
    {
        __LOG_ERROR__( "room[{}] [{}] battle[{}:{}] invalid!", _battle_room_id, KFAppID::ToString( _battle_server._server_id ), _battle_server._ip, _battle_server._port );

        // 删除时间
        _battle_redis_driver->Execute( "zrem {} {}", __KF_STRING__( battletime ), _battle_room_id );

        if ( _battle_server.IsValid() )
        {
            _kf_battle_manage->FreeBattleServer( _battle_server._server_id, _battle_server._version );
            _battle_server.Reset();
        }

        // 通知proxy
        _kf_cluster_shard->RemoveObjectToProxy( _battle_room_id );

        // 关闭战场到匹配集群
        KFMsg::S2STellRoomCloseToMatchShardReq req;
        req.set_matchid( _match_id );
        req.set_roomid( _battle_room_id );
        auto ok = SendMessageToMatch( KFMsg::S2S_TELL_ROOM_CLOSE_TO_MATCH_SHARD_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( "close room[{}] to match req!", _battle_room_id );
        }
        else
        {
            __LOG_ERROR__( "close room[{}] to match req failed!", _battle_room_id );
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

