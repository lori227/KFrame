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

        // 初始化
        void Initialize( KFMatchQueue* kfmatchqueue, uint32 battleserverid );

        // 是否正在等待匹配
        bool IsWaitMatch( uint32 battleserverid, uint32 playercount );

        // 添加阵营
        void AddCamp( KFMatchCamp* kfcamp );

        // 房间逻辑
        void RunRoom();

        // 请求创建战场服务器
        void CreateBatterRoomReq();

        // 创建房间
        void CreateBattleRoomAck( uint32 battleshardid );

        // 阵营进入战场
        void EnterBattleRoomReq();

        // 进入游戏房间
        bool EnterBattleRoomAck( uint32 campid, bool addok );

        // 通知房间开启
        bool TellRoomOpen( uint32 waittime );

        // 正式开始战斗
        void TellRoomStart();

        // 离开游戏房间
        bool LeaveBattleRoom( uint32 campid, uint64 groupid, uint32 playerid );

        // 取消匹配
        bool CancelMatch( uint32 campid, uint64 groupid );

        // 重置房间
        void ResetRoom();

        // 发送消息到战场集群服务器
        bool SendMessageToBattle( uint32 msgid, google::protobuf::Message* message );

    public:
        // 匹配队列
        KFMatchQueue* _kf_match_queue;

        // 匹配id
        uint32 _match_id;

        // 房间id
        uint64 _room_id;

        // 阵营列表
        KFMap< uint32, uint32, KFMatchCamp > _camp_list;

    private:
        // 分配战场定时器
        KFClockTimer _create_timer;

        // 指定的战场id
        uint32 _battle_server_id;

        // 总人数
        uint32 _room_player_count;

        // 战场开始的时间
        uint64 _battle_start_time;

        // 战场服务器集群的分片id
        uint32 _battle_shard_id;

        // 已经停止加入阵营了
        bool _is_stop_add_camp;
    };
}

#endif