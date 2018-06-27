#ifndef __KF_MATCH_PLAYER_H__
#define __KF_MATCH_PLAYER_H__

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFMatchShardConfig.h"

namespace KFrame
{
	// 匹配玩家
	class KFMatchRoom;
	class KFMatchQueue;
	class KFMatchSetting;

	__ST_CLASS__( KFMatchPlayer )
	{
	public:
		KFMatchPlayer();
		~KFMatchPlayer();

		static KFMatchPlayer* Create( const KFMsg::PBBattlePlayer* pbplayer );
		static void Release( KFMatchPlayer* kfplayer );
		
		// 加载
		void LoadFrom( const KFMsg::PBBattlePlayer* pbplayer );

		// 保存
		void SaveTo( KFMsg::PBBattlePlayer* pbenter );

		// 玩家id
		uint32 GetID() const;

		// 阵营id
		void SetCampID( uint32 campid );
		uint32 GetCampID() const;

		// 队伍id
		uint32 GetGroupID() const;
		
		// 取消匹配
		void CancelMatchReq();

		// 取消匹配
		void RunCancelMatch( KFMatchRoom* kfroom );
		
		// 查询匹配
		void QueryMatchRoom( uint32 serverid, KFMatchRoom* kfroom );

		// 发送消息
		bool SendMessageToGame( uint32 msgid, google::protobuf::Message* message );

	private:
		// 游戏玩家数据
		KFMsg::PBBattlePlayer _pb_player;

		// 取消匹配定时器
		KFClockTimer _cancel_timer;
	};
}

#endif