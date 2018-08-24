#ifndef __KF_MATCH_PLAYER_H__
#define __KF_MATCH_PLAYER_H__

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFMatchShardConfig.h"

namespace KFrame
{
    // 匹配玩家
    class KFMatchRoom;
    class KFMatchSetting;

    class KFMatchPlayer
    {
    public:
        KFMatchPlayer();
        ~KFMatchPlayer() = default;

        // 加载
        void LoadFrom( const KFMsg::PBBattlePlayer* pbplayer );

        // 保存
        void SaveTo( KFMsg::PBBattlePlayer* pbenter );

        // 玩家id
        uint32 GetID() const;

        // 阵营id
        uint32 GetCampID() const;
        void SetCampID( uint32 campid );

        // 队伍id
        uint64 GetGroupID() const;
        void SetGroupID( uint64 gropuid );

        // 设置房间id
        uint64 GetRoomID() const;
        void SetRoomID( uint64 roomid );

        // 服务器id
        uint32 GetServerID() const;
        void SetServerID( uint32 serverid );

        // 发送消息
        bool SendMessageToGame( uint32 msgid, google::protobuf::Message* message );

    private:
        // 所在房间
        uint64 _room_id;

        // 游戏玩家数据
        KFMsg::PBBattlePlayer _pb_player;
    };
}

#endif