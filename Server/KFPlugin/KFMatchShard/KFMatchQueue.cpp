#include "KFMatchQueue.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
    KFMatchQueue::KFMatchQueue()
    {
        _match_id = 0;
        _kf_setting = nullptr;
        _kf_match_module = nullptr;
    }

    KFMatchQueue::~KFMatchQueue()
    {

    }

    void KFMatchQueue::StartMatch( const KFMsg::PBMatchGroup* pbgroup, bool allowgroup )
    {
        // 判断成员是否已经满
        auto playercount = static_cast< uint32 >( pbgroup->pbplayer_size() );

        // 已经满, 或者不需要组队, 直接丢入战场中
        if ( playercount >= _kf_setting->_max_group_player_count || !allowgroup )
        {
            // 找到一个正在等待的战场
            auto kfroom = FindWaitMatchRoom( playercount );
            auto kfcamp = kfroom->AddCamp( pbgroup );
        }
        else
        {
            auto kfgroup = __KF_CREATE__( KFMatchGroup );
            kfgroup->LoadFrom( pbgroup );
            kfgroup->_kf_match_queue = this;
            _kf_group_list.Insert( kfgroup->_group_id, kfgroup );
        }
    }

    KFMatchRoom* KFMatchQueue::FindWaitMatchRoom( uint32 playercount )
    {
        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            if ( kfroom->IsWaitMatch( playercount ) )
            {
                return kfroom;
            }

            kfroom = _kf_room_list.Next();
        }

        // 如果没有找到, 创建一个
        kfroom = __KF_CREATE_BATCH__( KFMatchRoom, 100 );
        kfroom->Initialize( this );
        _kf_room_list.Insert( kfroom->_battle_room_id, kfroom );
        return kfroom;
    }

    bool KFMatchQueue::CancelMatchReq( uint32 playerid )
    {
        // 在匹配队伍里查找
        auto ok = CancelMatchFromGroup( playerid );
        if ( ok )
        {
            return true;
        }

        return CancelMatchFromCamp( playerid );
    }

    bool KFMatchQueue::CancelMatchAck( uint64 roomid, uint32 campid, uint32 playerid, bool isroomopen )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        return kfroom->CancelMatchAck( campid, playerid, isroomopen );
    }

    bool KFMatchQueue::CancelMatchFromGroup( uint32 playerid )
    {
        auto kfgroup = FindMatchGroup( playerid );
        if ( kfgroup == nullptr )
        {
            return false;
        }

        return _kf_group_list.Remove( kfgroup->_group_id );
    }

    KFMatchGroup* KFMatchQueue::FindMatchGroup( uint32 playerid )
    {
        auto kfgroup = _kf_group_list.First();
        while ( kfgroup != nullptr )
        {
            if ( kfgroup->HavePlayer( playerid ) )
            {
                return kfgroup;
            }

            kfgroup = _kf_group_list.Next();
        }

        return nullptr;
    }

    bool KFMatchQueue::CancelMatchFromCamp( uint32 playerid )
    {
        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            if ( kfroom->CancelMatchReq( playerid ) )
            {
                return true;
            }

            kfroom = _kf_room_list.Next();
        }

        return false;
    }

    void KFMatchQueue::RunMatch()
    {
        // 匹配数量不足的队伍
        RunMatchGroup();

        // 等待的房间
        RunWaitRoom();
    }

    void KFMatchQueue::RunMatchGroup()
    {
        auto kfcamp = MatchGroupToCamp();
        while ( kfcamp != nullptr )
        {
            auto kfroom = FindWaitMatchRoom( kfcamp->PlayerCount() );
            kfroom->AddCamp( kfcamp );

            kfcamp = MatchGroupToCamp();
        }
    }

    KFMatchGroup* KFMatchQueue::FindGroupByCount( uint32 groupcount, std::set<KFMatchGroup*>& grouplist )
    {
        for ( auto iter : _kf_group_list._objects )
        {
            auto kfgroup = iter.second;
            if ( grouplist.find( kfgroup ) != grouplist.end() )
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

    KFMatchCamp* KFMatchQueue::MatchGroupToCamp()
    {
        bool ismatch = false;
        std::set< KFMatchGroup* > grouplist;
        for ( auto iter : _kf_group_list._objects )
        {
            auto kfgroup = iter.second;

            grouplist.clear();
            grouplist.insert( kfgroup );

            ismatch = MatchGroupList( kfgroup->PlayerCount(), grouplist );
            if ( ismatch )
            {
                break;
            }
        }

        if ( !ismatch )
        {
            return nullptr;
        }

        auto kfcamp = __KF_CREATE_BATCH__( KFMatchCamp, 100 );
        for ( auto kfgroup : grouplist )
        {
            kfcamp->AddGroup( kfgroup );
            _kf_group_list.Remove( kfgroup->_group_id, false );
        }

        return kfcamp;
    }

    bool KFMatchQueue::MatchGroupList( uint32 groupcount, std::set<KFMatchGroup*>& grouplist )
    {
        auto needcount = _kf_setting->_max_group_player_count - groupcount;

        while ( true )
        {
            // 找到合适的队伍
            auto kfgroup = FindGroupByCount( needcount, grouplist );
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
        }

        return false;
    }

    void KFMatchQueue::RunWaitRoom()
    {
        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            kfroom->RunRoom();
            kfroom = _kf_room_list.Next();
        }
    }

    bool KFMatchQueue::CreateBattleRoom( uint64 roomid, uint32 battleshardid )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        kfroom->CreateBattleRoom( battleshardid );
        return true;
    }

    bool KFMatchQueue::EnterBattleRoom( uint64 roomid, uint32 campid, bool addok )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            auto strroomid = __TO_STRING__( roomid );
            __LOG_ERROR__( KFLogEnum::Logic, "can't find room[{}:{}]", strroomid, campid );
            return false;
        }

        return kfroom->EnterBattleRoom( campid, addok );
    }

    bool KFMatchQueue::OpenBattleRoom( uint64 roomid, uint32 waittime )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        return kfroom->OpenBattleRoom( waittime );
    }

    bool KFMatchQueue::LeaveBattleRoom( uint64 roomid, uint32 campid, uint32 playerid )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        return kfroom->LeaveBattleRoom( campid, playerid );
    }

    bool KFMatchQueue::StartBattleRoom( uint64 roomid )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        kfroom->StartBattleRoom();
        _kf_room_list.Remove( roomid );
        return true;
    }

    bool KFMatchQueue::CloseBattleRoom( uint64 roomid )
    {
        auto kfroom = _kf_room_list.Find( roomid );
        if ( kfroom == nullptr )
        {
            return false;
        }

        //kfroom->CloseBattleRoom();
        _kf_room_list.Remove( roomid );
        return true;
    }

    bool KFMatchQueue::QueryBattleRoom( uint32 playerid, uint32 serverid )
    {
        auto ok = QueryMatchGroup( playerid, serverid );
        if ( ok )
        {
            return true;
        }

        return QueryMatchRoom( playerid, serverid );
    }

    bool KFMatchQueue::QueryMatchGroup( uint32 playerid, uint32 serverid )
    {
        auto kfgroup = _kf_group_list.First();
        while ( kfgroup != nullptr )
        {
            if ( kfgroup->QueryMatchGroup( playerid, serverid ) )
            {
                return true;
            }

            kfgroup = _kf_group_list.Next();
        }

        return false;
    }

    bool KFMatchQueue::QueryMatchRoom( uint32 playerid, uint32 serverid )
    {
        auto kfroom = _kf_room_list.First();
        while ( kfroom != nullptr )
        {
            if ( kfroom->QueryMatchGroup( playerid, serverid ) )
            {
                return true;
            }

            kfroom = _kf_room_list.Next();
        }

        return false;
    }
}