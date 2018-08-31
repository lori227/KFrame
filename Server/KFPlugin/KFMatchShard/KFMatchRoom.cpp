#include "KFMatchRoom.h"
#include "KFMatchQueue.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    KFMatchRoom::KFMatchRoom()
    {
        _room_id = _invalid_int;
        _battle_server_id = _invalid_int;
        _battle_shard_id = _invalid_int;
        _room_player_count = _invalid_int;
        _battle_start_time = _invalid_int;
        _is_stop_add_camp = false;
        _match_id = _invalid_int;
    }

    KFMatchRoom::~KFMatchRoom()
    {
        _camp_list.Clear();
    }

    void KFMatchRoom::Initialize( KFMatchQueue* kfmatchqueue, uint32 battleserverid )
    {
        _kf_match_queue = kfmatchqueue;
        _battle_server_id = battleserverid;
        _match_id = _kf_match_queue->_match_id;
        _room_id = KFUtility::Make64Guid( KFGlobal::Instance()->_app_id );

        // 开启创建定时器
        _create_timer.StartTimer( 1, 10000 );
    }

    void KFMatchRoom::ResetRoom()
    {
        __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] reset!", _room_id );

        _battle_shard_id = _invalid_int;
        _create_timer.StartTimer( 1, 10000 );

        // 玩家进入重置
        for ( auto& campiter : _camp_list._objects )
        {
            auto kfcamp = campiter.second;
            kfcamp->ResetEnterBattleRoom();
        }
    }

    void KFMatchRoom::RunRoom()
    {
        // 分配房间
        CreateBatterRoomReq();

        // 进入战场房间
        EnterBattleRoomReq();
    }

    bool KFMatchRoom::SendMessageToBattle( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToShard( __KF_STRING__( battle ), _battle_shard_id, msgid, message );
    }

    bool KFMatchRoom::IsFull()
    {
        return _room_player_count >= _kf_match_queue->_kf_setting->_max_player_count;
    }

    void KFMatchRoom::AddCamp( KFMatchCamp* kfcamp )
    {
        // 设置房间id
        for ( auto& giter : kfcamp->_group_list._objects )
        {
            auto kfgroup = giter.second;
            for ( auto& iter : kfgroup->_player_list )
            {
                auto kfplayer = iter.second;
                kfplayer->SetRoomID( _room_id );
            }
        }

        // 加入列表中
        _camp_list.Insert( kfcamp->_camp_id, kfcamp );

        // 玩家数量
        _room_player_count += kfcamp->PlayerCount();
        __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] playercount[{}]", _room_id, _room_player_count );
    }

    bool KFMatchRoom::IsWaitMatch( uint32 battleserverid, uint32 playercount )
    {
        // 判断是否指定战场id
        if ( _battle_server_id != battleserverid )
        {
            return false;
        }

        // 停止添加阵营, 判断人数
        if ( _is_stop_add_camp || IsFull() )
        {
            return false;
        }

        // 判断时间是否已经到了
        if ( _battle_start_time > _invalid_int )
        {
            if ( KFGlobal::Instance()->_game_time > _battle_start_time )
            {
                return false;
            }
        }

        // 最大人数限制
        if ( _room_player_count + playercount > _kf_match_queue->_kf_setting->_max_player_count )
        {
            return false;
        }

        return true;
    }

    void KFMatchRoom::CreateBatterRoomReq()
    {
        if ( !_create_timer.DoneTimer( KFGlobal::Instance()->_game_time, true ) )
        {
            return;
        }

        // 发送消息
        KFMsg::S2SCreateRoomToBattleProxyReq req;
        req.set_roomid( _room_id );
        req.set_battleserverid( _battle_server_id );
        req.set_matchid( _kf_match_queue->_match_id );
        req.set_maxplayercount( _kf_match_queue->_kf_setting->_max_player_count );
        auto ok = SendMessageToBattle( KFMsg::S2S_CREATE_ROOM_TO_BATTLE_PROXY_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Logic, "create battle room[{}] req!", _room_id );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Logic, "create battle room[{}] failed!", _room_id );
        }
    }

    void KFMatchRoom::CreateBattleRoomAck( uint32 battleshardid )
    {
        _create_timer.StopTimer();
        _battle_shard_id = battleshardid;

        __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] create battle[{}] ok!", _room_id, _battle_shard_id );
    }

    void KFMatchRoom::EnterBattleRoomReq()
    {
        // 还没有创建战场房间
        if ( _battle_shard_id == _invalid_int )
        {
            return;
        }

        // 请求把matchroom的阵营加入到battleroom中
        for ( auto& campiter : _camp_list._objects )
        {
            auto kfcamp = campiter.second;
            kfcamp->EnterBattleRoomReq( this );
        }
    }

    bool KFMatchRoom::EnterBattleRoomAck( uint32 campid, bool addok )
    {
        auto kfcamp = _camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            return false;
        }

        if ( addok )
        {
            kfcamp->EnterBattleRoomAck( this );
        }
        else
        {
            // 删除阵营
            _is_stop_add_camp = true;
            _camp_list.Remove( campid, false );

            // 找到一个新的战场
            auto kfroom = _kf_match_queue->FindWaitMatchRoom( _battle_server_id, kfcamp->PlayerCount() );
            kfroom->AddCamp( kfcamp );
        }

        return true;
    }

    bool KFMatchRoom::TellRoomOpen( uint32 waittime )
    {
        // 设置战场开始的时间
        // 战场正式开始前10秒钟, 玩家不进入该战场了
        _battle_start_time = KFGlobal::Instance()->_game_time + ( waittime * KFTimeEnum::OneSecondMicSecond ) - 10000;

        // 回复消息
        KFMsg::S2SOpenRoomToBattleShardAck ack;
        ack.set_roomid( _room_id );
        auto ok = SendMessageToBattle( KFMsg::S2S_OPEN_ROOM_TO_BATTLE_SHARD_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "match room[{}] open failed!", _room_id );
        }

        return ok;
    }

    void KFMatchRoom::TellRoomStart()
    {
        KFMsg::S2STellRoomStartToMatchShardAck ack;
        ack.set_roomid( _room_id );
        auto ok = SendMessageToBattle( KFMsg::S2S_TELL_ROOM_START_TO_BATTLE_SHARD_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "match room[{}] start failed!", _room_id );
        }
    }

    bool KFMatchRoom::LeaveBattleRoom( uint32 campid, uint64 groupid, uint32 playerid )
    {
        auto kfcamp = _camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            return false;
        }

        auto ok = kfcamp->RemovePlayer( groupid, playerid );
        if ( ok )
        {
            _room_player_count -= __MIN__( _room_player_count, 1 );
            __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] playercount[{}]", _room_id, _room_player_count );

            if ( kfcamp->PlayerCount() == 0 )
            {
                _camp_list.Remove( campid );
            }
        }

        return ok;
    }

    bool KFMatchRoom::CancelMatch( uint32 campid, uint64 groupid )
    {
        {
            // 发送消息到战斗集群
            KFMsg::S2SCancelMatchToBattleShardReq req;
            req.set_roomid( _room_id );
            req.set_campid( campid );
            req.set_groupid( groupid );
            SendMessageToBattle( KFMsg::S2S_CANCEL_MATCH_TO_BATTLE_SHARD_REQ, &req );
        }

        auto kfcamp = _camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            return false;
        }

        auto campplayercount = kfcamp->PlayerCount();
        auto ok = kfcamp->RemoveGroup( groupid );
        if ( ok )
        {
            // 将剩余的队伍, 重新加入等待队伍中
            for ( auto& iter : kfcamp->_group_list._objects )
            {
                _kf_match_queue->AddWaitGroup( iter.second );
            }
            kfcamp->_group_list.Clear( false );

            // 删除阵营
            _camp_list.Remove( campid );
            _room_player_count -= __MIN__( _room_player_count, campplayercount );
            __LOG_DEBUG__( KFLogEnum::Logic, "room[{}] playercount[{}]", _room_id, _room_player_count );
        }

        return ok;
    }
}