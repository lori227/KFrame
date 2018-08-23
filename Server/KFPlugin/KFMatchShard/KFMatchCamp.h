#ifndef __KF_MATCH_CAMP_H__
#define __KF_MATCH_CAMP_H__

#include "KFMatchGroup.h"

namespace KFrame
{
    // 匹配阵营
    class KFMatchCamp
    {
    public:
        KFMatchCamp();
        ~KFMatchCamp();

        // 初始化
        void LoadFrom( const KFMsg::PBMatchGroup* pbgroup );

        // 序列化
        void SaveTo( KFMsg::PBBattleCamp* pbcamp );

        // 添加队伍
        void AddGroup( KFMatchGroup* kfgroup );

        // 玩家数量
        uint32 PlayerCount();

        // 是否有玩家
        bool HavePlayer( uint32 playerid );

        // 取消匹配
        bool CancelMatchReq( uint32 playerid );

        // 执行取消匹配
        void RunCancelMatch( KFMatchRoom* kfroom );

        // 取消匹配
        uint32 CancelMatch( uint32 playerid, bool isroomopen );

        // 进入战场房间
        void EnterBattleRoomReq( KFMatchRoom* kfroom );

        // 进入游戏房间
        bool EnterBattleRoomAck();

        // 玩家离开
        bool LeaveBattleRoom( uint32 plyerid );

        // 查询房间
        bool QueryMatchGroup( uint32 playerid, uint32 serverid, KFMatchRoom* kfroom );
    public:
        // 匹配队列
        KFMatchQueue* _kf_match_queue;

        // 阵营id
        uint32 _camp_id;

        // 房间id
        uint64 _room_id;

        // 分配战场定时器
        KFClockTimer _enter_timer;

        // 玩家列表
        KFMap< uint32, uint32, KFMatchPlayer > _kf_player_list;
    };

}

#endif