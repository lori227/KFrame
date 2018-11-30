#include "KFMatchQueue.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    KFMatchQueue::KFMatchQueue()
    {
        _match_id = 0;
        _camp_id_maker = 0;
        _kf_setting = nullptr;
    }

    uint32 KFMatchQueue::MakeCampID()
    {
        return ++_camp_id_maker;
    }

    KFMatchCamp* KFMatchQueue::CreateMatchCamp( uint64 battleserverid, const std::string& version )
    {
        auto kfcamp = __KF_CREATE__( KFMatchCamp );
        kfcamp->_camp_id = MakeCampID();
        kfcamp->_battle_server_id = battleserverid;
        kfcamp->_battle_version = version;
        kfcamp->_kf_match_queue = this;
        return kfcamp;
    }
    //////////////////////////////////////////////////////////////////////////////////
    KFMatchPlayer* KFMatchQueue::CreateMatchPlayer( const KFMsg::PBBattlePlayer* pbplayer )
    {
        auto kfplayer = _kf_player_list.Find( pbplayer->playerid() );
        if ( kfplayer == nullptr )
        {
            kfplayer = _kf_player_list.Create( pbplayer->playerid() );
        }

        kfplayer->LoadFrom( pbplayer );
        kfplayer->SetCampID( _invalid_int );
        kfplayer->SetRoomID( _invalid_int );
        return kfplayer;
    }

    KFMatchPlayer* KFMatchQueue::FindMatchPlayer( uint64 playerid )
    {
        return _kf_player_list.Find( playerid );
    }

    void KFMatchQueue::RemoveMatchPlayer( uint64 playerid )
    {
        _kf_player_list.Remove( playerid );
    }
    //////////////////////////////////////////////////////////////////////////////////
    void KFMatchQueue::RunMatch()
    {
        // 匹配数量不足的队伍
        RunMatchGroupToCamp();

        // 匹配好的房间
        RunMatchRoom();
    }

    void KFMatchQueue::RunMatchRoom()
    {
        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            kfroom->RunRoom();
            kfroom = _kf_room_list.Next();
        }
    }
    //////////////////////////////////////////////////////////////////////////////////
    void KFMatchQueue::StartMatch( const KFMsg::PBMatchGroup* pbgroup, bool allowgroup, uint64 battleserverid, const std::string& version )
    {
        auto kfgroup = __KF_CREATE__( KFMatchGroup );
        kfgroup->_kf_match_queue = this;
        kfgroup->_battle_server_id = battleserverid;
        kfgroup->_battle_version = version;
        kfgroup->LoadFrom( pbgroup );

        // 已经满, 或者不需要组队, 直接丢入战场中
        auto playercount = static_cast< uint32 >( pbgroup->pbplayer_size() );
        if ( playercount >= _kf_setting->_max_group_player_count || !allowgroup )
        {
            // 初始化阵营
            auto kfcamp = CreateMatchCamp( battleserverid, version );
            kfcamp->AddGroup( kfgroup );

            // 找到一个正在等待的战场
            auto kfroom = FindWaitMatchRoom( battleserverid, playercount, version );
            kfroom->AddCamp( kfcamp );
        }
        else
        {
            // 加入列表中, 等待符合要求的一起组队
            AddWaitGroup( kfgroup );
        }
    }

    void KFMatchQueue::AddWaitGroup( KFMatchGroup* kfgroup )
    {
        _wait_group_list.Insert( kfgroup->_group_id, kfgroup );
    }

    KFMatchRoom* KFMatchQueue::FindWaitMatchRoom( uint64 battleserverid, uint32 playercount, const std::string& version )
    {
        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            if ( kfroom->IsWaitMatch( battleserverid, playercount, version ) )
            {
                return kfroom;
            }

            kfroom = _kf_room_list.Next();
        }

        // 如果没有找到, 创建一个
        kfroom = __KF_CREATE_BATCH__( KFMatchRoom, 100 );
        kfroom->Initialize( this, battleserverid, version );
        _kf_room_list.Insert( kfroom->_room_id, kfroom );
        return kfroom;
    }

    void KFMatchQueue::RunMatchGroupToCamp()
    {
        auto kfcamp = MatchGroupToCamp();
        while ( kfcamp != nullptr )
        {
            auto kfroom = FindWaitMatchRoom( kfcamp->_battle_server_id, kfcamp->PlayerCount(), kfcamp->_battle_version );
            kfroom->AddCamp( kfcamp );

            kfcamp = MatchGroupToCamp();
        }
    }

    KFMatchCamp* KFMatchQueue::MatchGroupToCamp()
    {
        bool ismatch = false;
        uint32 battleid = _invalid_int;
        std::string battleverson = "";

        std::set< KFMatchGroup* > grouplist;
        for ( auto& iter : _wait_group_list._objects )
        {
            auto kfgroup = iter.second;

            grouplist.clear();
            grouplist.insert( kfgroup );

            // 匹配队伍列表
            ismatch = MatchGroupList( kfgroup->_battle_server_id, kfgroup->_battle_version, kfgroup->PlayerCount(), grouplist );
            if ( ismatch )
            {
                battleid = kfgroup->_battle_server_id;
                battleverson = kfgroup->_battle_version;
                break;
            }
        }

        // 没有匹配到, 返回null
        if ( !ismatch )
        {
            return nullptr;
        }

        // 创建一个阵营, 加入队伍列表
        auto kfcamp = CreateMatchCamp( battleid, battleverson );
        for ( auto kfgroup : grouplist )
        {
            kfcamp->AddGroup( kfgroup );
            _wait_group_list.Remove( kfgroup->_group_id, false );
        }

        return kfcamp;
    }

    bool KFMatchQueue::MatchGroupList( uint64 battleserverid, const std::string& version, uint32 groupcount, std::set<KFMatchGroup*>& grouplist )
    {
        // 如果 需要3个队伍, 先找3个人, 找不到就找2个人的队伍, 在找不到找1个人的队伍
        auto needcount = _kf_setting->_max_group_player_count - groupcount;

        do
        {
            // 找到数量合适的队伍
            auto kfgroup = FindGroupByCount( battleserverid, version, needcount, grouplist );
            if ( kfgroup != nullptr )
            {
                grouplist.insert( kfgroup );
                groupcount += kfgroup->PlayerCount();
                needcount -= kfgroup->PlayerCount();

                if ( groupcount >= _kf_setting->_max_group_player_count )
                {
                    return true;
                }
            }
            else
            {
                --needcount;
                if ( needcount == 0 )
                {
                    break;
                }
            }
        } while ( true );

        return false;
    }

    KFMatchGroup* KFMatchQueue::FindGroupByCount( uint64 battleserverid, const std::string& version, uint32 groupcount, std::set<KFMatchGroup*>& grouplist )
    {
        for ( auto iter : _wait_group_list._objects )
        {
            auto kfgroup = iter.second;
            // 不能是列表中的队伍
            if ( grouplist.find( kfgroup ) != grouplist.end() )
            {
                continue;
            }

            // 不同的战场id
            if ( kfgroup->_battle_server_id != battleserverid )
            {
                continue;
            }

            // 不同的版本
            if ( kfgroup->_battle_version != version )
            {
                continue;
            }

            if ( kfgroup->PlayerCount() == groupcount )
            {
                return kfgroup;
            }
        }

        return nullptr;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    bool KFMatchQueue::CreateBattleRoomAck( uint64 roomid, uint64 battleshardid )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        kfroom->CreateBattleRoomAck( battleshardid );
        return true;
    }

    bool KFMatchQueue::CampEnterBattleRoomAck( uint64 roomid, uint32 campid, bool addok )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            __LOG_ERROR__( "can't find room[{}:{}]", roomid, campid );
            return false;
        }

        return kfroom->EnterBattleRoomAck( campid, addok );
    }

    bool KFMatchQueue::TellMatchRoomOpen( uint64 roomid, uint32 waittime )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        return kfroom->TellRoomOpen( waittime );
    }

    bool KFMatchQueue::TellMatchRoomStart( uint64 roomid )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        kfroom->TellRoomStart();

        // battleroom启动后, matchroom删除, 释放资源
        _kf_room_list.Remove( roomid );
        return true;
    }

    bool KFMatchQueue::TellMatchRoomClose( uint64 roomid )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        _kf_room_list.Remove( roomid );
        return true;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    bool KFMatchQueue::CancelMatch( uint64 playerid )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        if ( kfplayer->GetRoomID() == 0 )
        {
            _wait_group_list.Remove( kfplayer->GetGroupID() );
        }
        else
        {
            auto kfroom = _kf_room_list.Find( kfplayer->GetRoomID() );
            if ( kfroom != nullptr )
            {
                kfroom->CancelMatch( kfplayer->GetCampID(), kfplayer->GetGroupID() );
            }
        }

        return true;
    }

    KFMatchGroup* KFMatchQueue::FindMatchGroup( uint64 playerid )
    {
        auto kfgroup = _wait_group_list.First();
        while ( kfgroup != nullptr )
        {
            if ( kfgroup->HavePlayer( playerid ) )
            {
                return kfgroup;
            }

            kfgroup = _wait_group_list.Next();
        }

        return nullptr;
    }

    bool KFMatchQueue::QueryMatchRoom( uint64 playerid, uint64 serverid )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        kfplayer->SetServerID( serverid );

        auto kfroom = _kf_room_list.Find( kfplayer->GetRoomID() );
        if ( kfroom != nullptr )
        {
            KFMsg::S2SPlayerOnlineToBattleShardReq req;
            req.set_playerid( playerid );
            req.set_serverid( serverid );
            req.set_roomid( kfroom->_room_id );
            req.set_campid( kfplayer->GetCampID() );
            kfroom->SendToBattle( KFMsg::S2S_PLAYER_ONLINE_TO_BATTLE_SHARD_REQ, &req );
        }

        return true;
    }

    bool KFMatchQueue::LeaveBattleRoom( uint64 roomid, uint32 campid, uint64 groupid, uint64 playerid )
    {
        auto kfplayer = _kf_player_list.Find( playerid );
        if ( kfplayer != nullptr )
        {
            roomid = kfplayer->GetRoomID();
            campid = kfplayer->GetCampID();
            groupid = kfplayer->GetGroupID();

            _kf_player_list.Remove( playerid );
        }

        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        return kfroom->LeaveBattleRoom( campid, groupid, playerid );
    }

    bool KFMatchQueue::ResetMatchRoom( uint64 roomid )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        kfroom->ResetRoom();
        return true;
    }
}