#ifndef __KF_MATCH_GROUP_H__
#define __KF_MATCH_GROUP_H__

#include "KFMatchPlayer.h"

namespace KFrame
{
    class KFMatchQueue;

    // 匹配队伍
    class KFMatchGroup
    {
    public:
        KFMatchGroup();
        ~KFMatchGroup();

        // 初始化
        void LoadFrom( const KFMsg::PBMatchGroup* pbgroup );

        // 玩家数量
        uint32 PlayerCount();

        // 是否满了
        bool IsFull( const KFMatchSetting* kfsetting );

        // 是否存在玩家
        bool HavePlayer( uint32 playerid );

        // 删除玩家
        bool RemovePlayer( uint32 playerid );

    public:
        // 队伍id
        uint64 _group_id;

        // 指定的战场id
        uint32 _battle_server_id;

        // 战场版本
        std::string _battle_version;

        // 匹配队列
        KFMatchQueue* _kf_match_queue;

        // 匹配的玩家列表
        std::map< uint32, KFMatchPlayer* > _player_list;
    };
}

#endif