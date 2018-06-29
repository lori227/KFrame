#ifndef __KF_MATCH_QUEUE_H__
#define __KF_MATCH_QUEUE_H__

#include "KFMatchGroup.h"
#include "KFMatchRoom.h"

namespace KFrame
{
    class KFMatchShardModule;
    class KFMatchQueue
    {
    public:
        KFMatchQueue();
        ~KFMatchQueue();

        // 开始匹配
        void StartMatch( const KFMsg::PBMatchGroup* pbgroup, bool allowgroup );

        // 取消匹配
        bool CancelMatchReq( uint32 playerid );
        bool CancelMatchAck( uint64 roomid, uint32 campid, uint32 playerid, bool isroomopen );

        // 逻辑
        void RunMatch();

        // 查找正在匹配的房间
        KFMatchRoom* FindWaitMatchRoom( uint32 playercount );

        // 开启战斗房间
        bool CreateBattleRoom( uint64 roomid, uint32 battleshardid );

        // 进入战斗房间
        bool EnterBattleRoom( uint64 roomid, uint32 campid, bool addok );

        // 开启战场
        bool OpenBattleRoom( uint64 roomid, uint32 waittime );

        // 离开战斗房间
        bool LeaveBattleRoom( uint64 roomid, uint32 campid, uint32 playerid );

        // 开始战斗
        bool StartBattleRoom( uint64 roomid );

        // 关闭房间
        bool CloseBattleRoom( uint64 roomid );

        // 查询房间
        bool QueryBattleRoom( uint32 playerid, uint32 serverid );

    protected:

        // 等待的房间
        void RunWaitRoom();

        // 匹配队伍
        void RunMatchGroup();

        // 取消匹配
        bool CancelMatchFromGroup( uint32 playerid );
        bool CancelMatchFromCamp( uint32 playerid );

        // 查找队伍
        KFMatchGroup* FindMatchGroup( uint32 playerid );
        KFMatchGroup* FindGroupByCount( uint32 groupcount, std::set< KFMatchGroup* >& grouplist );

        // 匹配队伍到阵营
        KFMatchCamp* MatchGroupToCamp();

        // 匹配队伍列表
        bool MatchGroupList( uint32 groupcount, std::set< KFMatchGroup* >& grouplist );

        // 查询匹配队伍
        bool QueryMatchGroup( uint32 playerid, uint32 serverid );
        bool QueryMatchRoom( uint32 playerid, uint32 serverid );

    public:
        // 匹配id
        uint32 _match_id;

        // 配置
        const KFMatchSetting* _kf_setting;

        // 匹配
        KFMatchShardModule* _kf_match_module;

        // 等待组队的队伍列表
        KFMap< uint64, uint64, KFMatchGroup > _kf_group_list;

        // 等待匹配的房间
        KFMap< uint64, uint64, KFMatchRoom > _kf_room_list;
    };
}


#endif