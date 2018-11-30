#ifndef __KF_MATCH_QUEUE_H__
#define __KF_MATCH_QUEUE_H__

#include "KFMatchGroup.h"
#include "KFMatchRoom.h"

namespace KFrame
{
    class KFMatchQueue
    {
    public:
        KFMatchQueue();
        ~KFMatchQueue() = default;

        //////////////////////////////////////////////////////////////////////////////////////
        // 创建匹配玩家
        KFMatchPlayer* CreateMatchPlayer( const KFMsg::PBBattlePlayer* pbplayer );

        // 查找匹配玩家
        KFMatchPlayer* FindMatchPlayer( uint64 playerid );

        // 删除匹配玩家
        void RemoveMatchPlayer( uint64 playerid );

        //////////////////////////////////////////////////////////////////////////////////////
        // 创建阵营id
        uint32 MakeCampID();

        // 创建阵营
        KFMatchCamp* CreateMatchCamp( uint64 battleserverid, const std::string& version );

        // 查找正在匹配的房间
        KFMatchRoom* FindWaitMatchRoom( uint64 battleid, uint32 playercount, const std::string& version );

        // 开始匹配
        void StartMatch( const KFMsg::PBMatchGroup* pbgroup, bool allowgroup, uint64 battleserverid, const std::string& version );

        // 取消匹配
        bool CancelMatch( uint64 playerid );

        // 添加等待组队队伍
        void AddWaitGroup( KFMatchGroup* kfgroup );

        // 逻辑
        void RunMatch();

        // 开启战斗房间
        bool CreateBattleRoomAck( uint64 roomid, uint64 battleshardid );

        // 进入战斗房间
        bool CampEnterBattleRoomAck( uint64 roomid, uint32 campid, bool addok );

        // battle通知room已经准备
        bool TellMatchRoomOpen( uint64 roomid, uint32 waittime );

        // battle通知roomd已经开始游戏
        bool TellMatchRoomStart( uint64 roomid );

        // 关闭房间
        bool TellMatchRoomClose( uint64 roomid );

        // 查询房间
        bool QueryMatchRoom( uint64 playerid, uint64 serverid );

        // 离开战斗房间
        bool LeaveBattleRoom( uint64 roomid, uint32 campid, uint64 groupid, uint64 playerid );

        // 重置房间
        bool ResetMatchRoom( uint64 roomid );
    protected:

        // 等待的房间
        void RunMatchRoom();

        // 匹配队伍
        void RunMatchGroupToCamp();

        // 查找队伍
        KFMatchGroup* FindMatchGroup( uint64 playerid );
        KFMatchGroup* FindGroupByCount( uint64 battleserverid, const std::string& version, uint32 groupcount, std::set< KFMatchGroup* >& grouplist );

        // 匹配队伍到阵营
        KFMatchCamp* MatchGroupToCamp();

        // 匹配队伍列表
        bool MatchGroupList( uint64 battleserverid, const std::string& version, uint32 groupcount, std::set< KFMatchGroup* >& grouplist );

    public:
        // 匹配id
        uint32 _match_id;

        // 阵营maker
        uint32 _camp_id_maker;

        // 配置
        const KFMatchSetting* _kf_setting;

        // 等待组队的队伍列表
        KFMap< uint64, uint64, KFMatchGroup > _wait_group_list;

        // 等待匹配的房间
        KFMap< uint64, uint64, KFMatchRoom > _kf_room_list;

        // 玩家的列表
        KFMap< uint64, uint64, KFMatchPlayer > _kf_player_list;
    };
}


#endif