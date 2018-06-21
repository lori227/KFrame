#ifndef __KF_BATTLE_PROXY_MODULE_H__
#define __KF_BATTLE_PROXY_MODULE_H__

/************************************************************************
//    @Moudle			:    战斗服务器代理模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-2-22
************************************************************************/

#include "KFrame.h"
#include "KFBattleProxyInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFClusterProxy/KFClusterProxyInterface.h"

namespace KFrame
{
	class KFBattleProxyModule : public KFBattleProxyInterface
	{
	public:
		KFBattleProxyModule();
		~KFBattleProxyModule();

		// 加载配置
		virtual void InitModule();

		// 初始化
		virtual void BeforeRun();

		// 关闭
		virtual void BeforeShut();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
	protected:
		// 注册战场服务器
		__KF_MESSAGE_FUNCTION__( HandleRegisterBattleServerReq );

		// 请求开始战场
		__KF_MESSAGE_FUNCTION__( HandleCreateRoomToBattleProxyReq );

		// 查询战场房间
		__KF_MESSAGE_FUNCTION__( HandleQueryBattleRoomReq );

		// 玩家进入战场
		__KF_MESSAGE_FUNCTION__( HandlePlayerEnterBattleRoomAck );

		// 玩家登陆战场
		__KF_MESSAGE_FUNCTION__( HandlePlayerLoginBattleRoomReq );

		// 玩家离开战场
		__KF_MESSAGE_FUNCTION__( HandlePlayerLeaveBattleRoomReq );

		// 战场开启
		__KF_MESSAGE_FUNCTION__( HandleTellBattleRoomStartReq );

		// 战场结束
		__KF_MESSAGE_FUNCTION__( HandleTellBattleRoomFinishReq );

		// 战场结算
		__KF_MESSAGE_FUNCTION__( HandleBattleRoomScoreBalanceReq );

		// 战场创建
		__KF_MESSAGE_FUNCTION__( HandleOpenBattleRoomAck );

	protected:
		// 服务器断开连接
		__KF_SERVER_LOST_FUNCTION__( OnServerLostHandle );

	private:
		// 战斗服务器断开连接
		void OnServerLostBattleServer( uint32 serverid );
	};
}



#endif