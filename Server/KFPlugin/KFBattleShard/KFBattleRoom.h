#ifndef __KF_BATTLE_ROOM_H__
#define __KF_BATTLE_ROOM_H__

#include "KFBattleCamp.h"
#include "KFBattleManage.h"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
    enum KFRoomStatus
    {
        StatusBattleRoomIdle = 0,		// 等待状态
        StatusBattleRoomAlloc = 1,		// 分配战场房间
        StatusBattleRoomOpen = 2,		// 通知战斗服务器开启房间
        StatusBattleRoomEnter = 3,		// 玩家进入战场
        StatisBattleRoomPlaying = 4,	// 通知匹配服务器战场正式开始
    };

    // 战斗房间
    class KFBattleRoom
    {
    public:
        KFBattleRoom();
        ~KFBattleRoom();

        // 初始化
        void InitRoom( uint32 matchid, uint64 roomid, uint32 battleserverid, uint32 maxplayercount );

        // 添加阵营
        KFBattleCamp* AddCamp( const KFMsg::PBBattleCamp* pbcamp );

        // 删除阵营
        bool RemoveCamp( uint32 campid );

        // 房间逻辑
        void RunRoom();

        // 判断房间是否有效
        bool CheckValid();
        void SetValidTime();

        // 战场是否开始
        bool IsBattleRoomStart();

        // 判断是否可以开启战场房间
        bool CheckCanOpenBattleRoom();

        // 开启房间
        void ConfirmOpenBattleRoom( bool opensuccess, uint32 waittime );

        // 确认开启匹配房间
        void ConfirmOpenMatchRoom();

        // 更新游戏房间
        void UpdateBattleRoom( uint32 proxyid, uint32 serverid, const std::string& ip, uint32 port );

        // 进入游戏房间
        bool ConfirmEnterBattleRoom( uint32 campid, uint32 playerid );

        // 离开游戏房间
        bool LeaveBattleRoom( uint32 campid, uint32 playerid );

        // 取消匹配
        bool CancelMatch( uint32 campid, uint64 groupid );

        // 确认匹配结果
        bool NoticeBattleRoom( uint32 campid, uint32 playerid );

        // 登陆游戏房间
        bool LoginBattleRoom( uint32 campid, uint32 playerid );

        // 开始游戏房间
        void StartBattleRoom( uint32 maxtime );

        // 确认开启房间
        void ConfirmStartBattleRoom();

        // 查询游戏房间
        bool QueryBattleRoom( uint32 playerid, uint32 serverid );

        // 游戏房间结束
        void FinishBattleRoom();

        // 释放战场
        void FreeInValidRoom();

        // 玩家上线
        bool PlayerOnlineBattleRoom( uint32 campid, uint32 playerid, uint32 serverid );

        // 结算
        bool BattleScoreBalance( KFMsg::PBBattleScore* pbscore );

        // 发送消息到Match
        bool SendMessageToMatch( uint32 msgid, google::protobuf::Message* message );

        // 发送消息到战场
        bool SendMessageToBattle( uint32 msgid, google::protobuf::Message* message );

        // 发送消息到玩家
        void SendMessageToRoom( uint32 msgid, google::protobuf::Message* message );

    protected:
        // 查找玩家
        KFBattlePlayer* FindBattlePlayer( uint32 playerid );
        KFBattlePlayer* FindBattlePlayer( uint32 campid, uint32 playerid );

        // 更新状态
        void UpdateRoomStatus( uint32 status, uint32 intervaltime );

        // 添加玩家数量
        void AddPlayerCount( uint32 playercount );

        // 刷新状态
        void RunBattleRoomStatus();

        // 请求分配战场服务器
        void AllocBatterServer();

        // 请求开启战场
        void OpenBattleRoom();

        // 发送玩家数据到战场
        void PlayerEnterBattleRoom();

        // 房间正在游戏
        void BattleRoomPlaying();
    public:
        // 匹配模式
        uint32 _match_id;

        // 匹配proxyid
        uint32 _match_proxy_id;

        // 匹配 shardid
        uint32 _match_shard_id;

        // 房间id
        uint64 _battle_room_id;

        // 指定的战场id
        uint32 _battle_server_id;

        // 战场服务器
        KFBattleServer _battle_server;

        // 状态
        uint32 _status;

        // 总人数
        uint32 _total_player_count;

        // 阵营列表
        KFMap< uint32, uint32, KFBattleCamp > _kf_camp_list;

    protected:
        // 状态定时器
        KFClockTimer _status_timer;

        // 请求次数
        uint32 _req_count;

        // 最大玩家数量
        uint32 _max_player_count;

        // 有效的时间
        uint64 _battle_valid_time;

        // 匹配服务器已经开启
        bool _is_match_room_open;

        // 房间开启等待时间
        uint32 _battle_wait_time;

        // redis
        KFRedisDriver* _battle_redis_driver;
    };
}

#endif