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

        // 序列化
        void SaveTo( KFMsg::PBBattleCamp* pbcamp );

        // 添加队伍
        void AddGroup( KFMatchGroup* kfgroup );

        // 玩家离开
        bool RemoveGroup( uint64 groupid );
        bool RemovePlayer( uint64 groupid, uint32 playerid );

        // 玩家数量
        uint32 PlayerCount();

        // 进入战场房间
        void EnterBattleRoomReq( KFMatchRoom* kfroom );
        void ResetEnterBattleRoom();

        // 进入游戏房间
        void EnterBattleRoomAck( KFMatchRoom* kfroom );

    public:
        // 指定的战场id
        uint32 _battle_server_id;

        // 战场版本
        std::string _battle_version;

        // 阵营id
        uint32 _camp_id;

        // 匹配队列
        KFMatchQueue* _kf_match_queue;

        // 分配战场定时器
        KFClockTimer _enter_timer;

        // 玩家数量
        uint32 _player_count;

        // 队伍列表
        KFMap< uint64, uint64, KFMatchGroup > _group_list;
    };

}

#endif