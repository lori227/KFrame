#ifndef __KF_MATCH_GROUP_H__
#define __KF_MATCH_GROUP_H__

#include "KFMatchPlayer.h"

namespace KFrame
{
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

		// 查询匹配房间
		bool QueryMatchGroup( uint32 playerid, uint32 gameid );
	public:
		// 匹配队列
		KFMatchQueue* _kf_match_queue;

		// 队伍id
		uint64 _group_id;

		// 匹配的玩家列表
		KFMap< uint32, uint32, KFMatchPlayer > _kf_player_list;
	};
}

#endif