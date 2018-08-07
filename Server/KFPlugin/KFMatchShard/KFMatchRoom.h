#ifndef __KF_MATCH_ROOM_H__
#define __KF_MATCH_ROOM_H__

#include "KFMatchCamp.h"

namespace KFrame
{
    // 匹配房间
    class KFMatchRoom
    {
    public:
        KFMatchRoom();
        ~KFMatchRoom();

        void Initialize( KFMatchQueue* kfmatchqueue );

        // 判断是否满了
        bool IsFull();

        // 是否正在等待匹配
        bool IsWaitMatch( uint32 playercount );

        // 取消匹配
        bool CancelMatchReq( uint32 playerid );
        bool CancelMatchAck( uint32 campid, uint32 playerid, bool isroomopen );

        // 添加阵营
        KFMatchCamp* AddCamp( const KFMsg::PBMatchGroup* pbgroup );
        void AddCamp( KFMatchCamp* kfcamp );

        // 请求分配
        void RunRoom();

        // 创建房间
        void CreateBattleRoom( uint32 battleshardid );

        // 进入游戏房间
        bool EnterBattleRoom( uint32 campid, bool addok );

        // 开启房间
        bool OpenBattleRoom( uint32 waittime );

        // 正式开始战斗
        void StartBattleRoom();

        // 离开游戏房间
        bool LeaveBattleRoom( uint32 campid, uint32 playerid );

        // 查询匹配房间
        bool QueryMatchGroup( uint32 playerid, uint32 serverid );

        // 发送消息到战场集群服务器
        bool SendMessageToBattle( uint32 msgid, google::protobuf::Message* message );

    protected:
        // 请求创建战场服务器
        void RunCreateBatterRoom();

        // 阵营进入战场
        void RunEnterBattleRoom();

        // 取消匹配
        void RunCancelBattleRoom();

        // 查找阵营
        KFMatchCamp* FindMatchCamp( uint32 playerid );

        // 拆分阵营到队伍
        void SplitCampToGroup( KFMatchCamp* kfcamp );

    public:
        // 匹配队列
        KFMatchQueue* _kf_match_queue;

        // 匹配id
        uint32 _match_id;

        // 房间id
        uint64 _battle_room_id;

        // 阵营列表
        KFMap< uint32, uint32, KFMatchCamp > _kf_camp_list;

    private:
        // 分配战场定时器
        KFClockTimer _create_timer;

        // 阵营maker
        uint32 _camp_id_maker;

        // 总人数
        uint32 _total_player_count;

        // 战场开始的时间
        uint64 _battle_start_time;

        // 战场服务器集群的分片id
        uint32 _battle_shard_id;

        // 设置不能再阵营了
        bool _is_battle_room_full;
    };
}

#endif