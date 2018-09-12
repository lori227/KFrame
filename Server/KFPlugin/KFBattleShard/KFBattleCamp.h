#ifndef __KF_BATTLE_CAMP_H__
#define __KF_BATTLE_CAMP_H__

#include "KFBattlePlayer.h"

namespace KFrame
{
    // 战场阵营
    class KFBattleCamp
    {
    public:
        KFBattleCamp();
        ~KFBattleCamp();

        // 初始化
        void LoadFrom( const KFMsg::PBBattleCamp* pbcamp );

        // 是否有玩家
        bool HavePlayer( uint32 playerid );

        // 玩家数量
        uint32 PlayerCount();

        // 进入战场房间
        void RunEnterBattleRoom( KFBattleRoom* kfroom );

        // 结束离开战场
        void FinishLeaveBattleRoom();

        // 重置战场状态
        void ResetBattleRoomStatus();

        // 发送消息到阵营
        void SendMessageToCamp( uint32 msgid, google::protobuf::Message* message );

    public:
        // 阵营id
        uint32 _camp_id;

        // 玩家列表
        KFMap< uint32, uint32, KFBattlePlayer > _kf_player_list;
    };

}

#endif