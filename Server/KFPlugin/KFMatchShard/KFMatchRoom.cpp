#include "KFMatchRoom.h"
#include "KFMatchQueue.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    KFMatchRoom::KFMatchRoom()
    {
        _battle_room_id = _invalid_int;
        _battle_shard_id = _invalid_int;
        _total_player_count = _invalid_int;
        _battle_start_time = _invalid_int;
        _camp_id_maker = _invalid_int;
        _is_battle_room_full = false;
        _match_id = _invalid_int;
    }

    KFMatchRoom::~KFMatchRoom()
    {

    }

    void KFMatchRoom::Initialize( KFMatchQueue* kfmatchqueue )
    {
        _kf_match_queue = kfmatchqueue;
        _match_id = _kf_match_queue->_match_id;
        _battle_room_id = KFUtility::Make64Guid( KFGlobal::Instance()->_app_id );
        _str_room_id = __TO_STRING__( _battle_room_id );

        // 开启创建定时器
        _create_timer.StartTimer( 1, 10000 );
    }

    bool KFMatchRoom::SendMessageToBattle( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToShard( __KF_STRING__( battle ), _battle_shard_id, msgid, message );
    }

    void KFMatchRoom::RunRoom()
    {
        // 分配房间
        RunCreateBatterRoom();

        // 进入战场房间
        RunEnterBattleRoom();

        // 取消匹配
        RunCancelBattleRoom();
    }

    void KFMatchRoom::RunCancelBattleRoom()
    {
        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            kfcamp->RunCancelMatch( this );
            kfcamp = _kf_camp_list.Next();
        }
    }

    void KFMatchRoom::RunCreateBatterRoom()
    {
        if ( !_create_timer.DoneTimer( KFGlobal::Instance()->_game_time, true ) )
        {
            return;
        }

        // 发送消息
        KFMsg::S2SCreateRoomToBattleProxyReq req;
        req.set_roomid( _battle_room_id );
        req.set_matchid( _kf_match_queue->_match_id );
        req.set_maxplayercount( _kf_match_queue->_kf_setting->_max_player_count );
        SendMessageToBattle( KFMsg::S2S_CREATE_ROOM_TO_BATTLE_PROXY_REQ, &req );

        KFLogger::LogLogic( KFLogger::Debug, "[%s] create battle room[%s] req",
                            __FUNCTION__, _str_room_id.c_str() );
    }

    void KFMatchRoom::CreateBattleRoom( uint32 battleshardid )
    {
        _create_timer.StopTimer();
        _battle_shard_id = battleshardid;

        KFLogger::LogLogic( KFLogger::Debug, "[%s] room[%s] create battle[%u] ok!",
                            __FUNCTION__, _str_room_id.c_str(), _battle_shard_id );
    }

    bool KFMatchRoom::IsFull()
    {
        return _total_player_count >= _kf_match_queue->_kf_setting->_max_player_count;
    }

    KFMatchCamp* KFMatchRoom::AddCamp( const KFMsg::PBMatchGroup* pbgroup )
    {
        auto kfcamp = __KF_CREATE_BATCH__( KFMatchCamp, 100 );
        kfcamp->LoadFrom( pbgroup );
        AddCamp( kfcamp );

        return kfcamp;
    }

    void KFMatchRoom::AddCamp( KFMatchCamp* kfcamp )
    {
        kfcamp->_camp_id = ++_camp_id_maker;
        kfcamp->_kf_match_queue = _kf_match_queue;
        for ( auto iter : kfcamp->_kf_player_list._objects )
        {
            auto kfplayer = iter.second;
            kfplayer->SetCampID( kfcamp->_camp_id );
        }

        _kf_camp_list.Insert( kfcamp->_camp_id, kfcamp );

        // 玩家数量
        _total_player_count += kfcamp->PlayerCount();
    }

    bool KFMatchRoom::IsWaitMatch( uint32 playercount )
    {
        // 判断人数
        if ( _is_battle_room_full || IsFull() )
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
        if ( _total_player_count + playercount > _kf_match_queue->_kf_setting->_max_player_count )
        {
            return false;
        }

        return true;
    }

    bool KFMatchRoom::CancelMatchReq( uint32 playerid )
    {
        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            if ( kfcamp->CancelMatchReq( playerid ) )
            {
                return true;
            }

            kfcamp = _kf_camp_list.Next();
        }

        return false;
    }

    bool KFMatchRoom::CancelMatchAck( uint32 campid, uint32 playerid, bool isroomopen )
    {
        auto kfcamp = _kf_camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            return false;
        }

        auto playercount = kfcamp->CancelMatch( playerid, isroomopen );
        if ( playercount == _invalid_int )
        {
            return false;
        }

        _total_player_count -= __MIN__( playercount, _total_player_count );
        if ( kfcamp->PlayerCount() == _invalid_int )
        {
            _kf_camp_list.Remove( kfcamp->_camp_id );
        }
        else
        {
            // 把阵营拆分成队伍, 然后加入队伍匹配列表中
            if ( !isroomopen )
            {
                SplitCampToGroup( kfcamp );
                _kf_camp_list.Remove( kfcamp->_camp_id );
            }
        }

        return true;
    }

    void KFMatchRoom::SplitCampToGroup( KFMatchCamp* kfcamp )
    {
        std::map< uint32, std::list< KFMatchPlayer* > > splitgrouplist;

        // 切分成队伍列表
        auto kfplayer = kfcamp->_kf_player_list.First();
        while ( kfplayer != nullptr )
        {
            splitgrouplist[ kfplayer->GetGroupID() ].push_back( kfplayer );
            kfplayer = kfcamp->_kf_player_list.Next();
        }

        // 设置成匹配队伍
        for ( auto iter : splitgrouplist )
        {
            auto kfgroup = __KF_CREATE_BATCH__( KFMatchGroup, 100 );
            kfgroup->_group_id = iter.first;
            kfgroup->_kf_match_queue = _kf_match_queue;
            _kf_match_queue->_kf_group_list.Insert( kfgroup->_group_id, kfgroup );

            auto& playerlist = iter.second;
            for ( auto kftemp : playerlist )
            {
                kfcamp->_kf_player_list.Remove( kftemp->GetID(), false );
                kfgroup->_kf_player_list.Insert( kftemp->GetID(), kftemp );
            }
        }
    }

    KFMatchCamp* KFMatchRoom::FindMatchCamp( uint32 playerid )
    {
        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            if ( kfcamp->HavePlayer( playerid ) )
            {
                return kfcamp;
            }

            kfcamp = _kf_camp_list.Next();
        }

        return nullptr;
    }

    void KFMatchRoom::RunEnterBattleRoom()
    {
        // 还没有创建战场房间
        if ( _battle_shard_id == _invalid_int )
        {
            return;
        }

        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            kfcamp->EnterBattleRoomReq( this );
            kfcamp = _kf_camp_list.Next();
        }
    }

    bool KFMatchRoom::EnterBattleRoom( uint32 campid, bool addok )
    {
        auto kfcamp = _kf_camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            return false;
        }

        if ( addok )
        {
            return kfcamp->EnterBattleRoomAck();
        }

        // 删除阵营
        _is_battle_room_full = true;
        _kf_camp_list.Remove( campid, false );

        // 找到一个新的战场
        auto kfroom = _kf_match_queue->FindWaitMatchRoom( kfcamp->PlayerCount() );
        kfroom->AddCamp( kfcamp );
        return true;
    }

    bool KFMatchRoom::OpenBattleRoom( uint32 waittime )
    {
        // 设置战场开始的时间
        // 战场正式开始前10秒钟, 玩家不进入该战场了
        _battle_start_time = KFGlobal::Instance()->_game_time + waittime - 10000;

        // 回复消息
        KFMsg::S2SOpenRoomToBattleShardAck ack;
        ack.set_roomid( _battle_room_id );
        auto ok = SendMessageToBattle( KFMsg::S2S_OPEN_ROOM_TO_BATTLE_SHARD_ACK, &ack );
        if ( !ok )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] open room[%s] ack!",
                                __FUNCTION__, _str_room_id.c_str() );
        }

        return ok;
    }

    void KFMatchRoom::StartBattleRoom()
    {
        KFMsg::S2STellRoomStartToMatchShardAck ack;
        ack.set_roomid( _battle_room_id );
        auto ok = SendMessageToBattle( KFMsg::S2S_TELL_ROOM_START_TO_BATTLE_SHARD_ACK, &ack );
        if ( !ok )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] start room[%s] failed!",
                                __FUNCTION__, _str_room_id.c_str() );
        }
    }

    bool KFMatchRoom::LeaveBattleRoom( uint32 campid, uint32 playerid )
    {
        auto kfcamp = _kf_camp_list.Find( campid );
        if ( kfcamp == nullptr )
        {
            return false;
        }

        auto result = kfcamp->LeaveBattleRoom( playerid );
        if ( result )
        {
            _total_player_count -= __MIN__( _total_player_count, 1 );
            if ( kfcamp->PlayerCount() == 0 )
            {
                _kf_camp_list.Remove( campid );
            }
        }

        return result;
    }

    bool KFMatchRoom::QueryMatchGroup( uint32 playerid, uint32 serverid )
    {
        auto kfcamp = _kf_camp_list.First();
        while ( kfcamp != nullptr )
        {
            if ( kfcamp->QueryMatchGroup( playerid, serverid, this ) )
            {
                return true;
            }

            kfcamp = _kf_camp_list.Next();
        }

        return false;
    }

}

