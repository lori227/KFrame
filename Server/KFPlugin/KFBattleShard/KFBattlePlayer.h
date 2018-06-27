#ifndef __KF_BATTLE_PLAYER_H__
#define __KF_BATTLE_PLAYER_H__

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
	enum KFPlayerStatus
	{
		StatusPlayerIdle = 0,		// 等待状态
		StatusEnterRoom = 1,		// 进入房间状态
		StatusNoticeRoom = 2,		// 通知房间地址状态
		StatusDisconnetRoom = 3,	// 玩家断线状态
		StatusLoginRoom = 4,		// 玩家登陆状态
		StatusPlayingRoom = 5,		// 玩家游戏状态
		StatusLeaveRoom = 6,		// 玩家离开状态
	};

	class KFBattleCamp;
	class KFBattleRoom;
	__ST_CLASS__( KFBattlePlayer )
	{
	public:
		KFBattlePlayer();
		~KFBattlePlayer();

		static KFBattlePlayer* Create( const KFMsg::PBBattlePlayer& pbplayer );
		static void Release( KFBattlePlayer* kfplayer );
		
		// 玩家id
		uint32 GetID();

		// 加载
		void LoadFrom( const KFMsg::PBBattlePlayer& pbplayer );

		// 保存
		void SaveTo( KFMsg::PBBattlePlayer* pbenter );

		// 玩家进入战场
		void RunEnterRoom( KFBattleRoom* kfroom );
		
		// 进入游戏房间
		void ConfirmEnterBattleRoom();

		// 确认通知战场房间
		void ConfirmNoticeBattleRoom();

		// 登陆游戏房间
		bool LoginBattleRoomReq( KFBattleRoom* kfroom );

		// 离开房间
		void LeaveBattleRoomReq( KFBattleRoom* kfroom );

		// 查询游戏房间
		bool QueryBattleRoom( uint32 serverid );

		// 玩家上线
		void OnlineBattleRoom( uint32 serverid );

		// 离开战场
		void FinishLeaveRoom();

		// 发送消息到客户端
		bool SendMessageToClient( uint32 msgid, google::protobuf::Message* message );

		// 结算
		bool BattleScoreBalance( KFBattleRoom* kfroom, KFMsg::PBBattleScore* pbscore );

		// 计算评分
		void ScoreBalance( KFMsg::PBBattleScore* pbscore );
	protected:
		// 玩家进入战场
		void PlayerEnterBattleRoom( KFBattleRoom* kfroom );

		// 通知房间信息
		void PlayerNoticeBattleRoom( KFBattleRoom* kfroom );

		// 发送离开消息
		void SendLeaveRoomToPlayer();
	public:
		// 玩家数据
		KFMsg::PBBattlePlayer _pb_player;

		// token数据
		std::string _token;

		// 玩家状态
		uint32 _status;

		// 通知的次数
		uint32 _notice_count;
	};
}

#endif