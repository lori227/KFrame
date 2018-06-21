#ifndef __KF_LOGIN_NODE_MODULE_H__
#define __KF_LOGIN_NODE_MODULE_H__

/************************************************************************
//    @Moudle			:    登录节点模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-18
************************************************************************/

#include "KFrame.h"
#include "KFLoginGameInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDataClient/KFDataClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
	class KFLoginGameModule : public KFLoginGameInterface
	{
	public:
		KFLoginGameModule();
		~KFLoginGameModule();
		
		// 加载配置
		virtual void InitModule();

		// 初始化
		virtual void BeforeRun();

		// 关闭
		virtual void BeforeShut();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
	protected:
		// 处理更新登录数量
		__KF_MESSAGE_FUNCTION__( HandleLoginProxyUpdateReq );

		// 处理登录请求
		__KF_MESSAGE_FUNCTION__ ( HandleLoginGameReq );

		// 处理加载玩家数据请求
		__KF_MESSAGE_FUNCTION__ ( HandleLoadPlayerAck );

		// 处理登出游戏
		__KF_MESSAGE_FUNCTION__ ( HandleLoginOutReq );

		// 登录Token
		__KF_MESSAGE_FUNCTION__( HandleLoginTellTokenToGameReq );
		__KF_MESSAGE_FUNCTION__( HandleLoginTellTokenToGameAck );

	protected:
		// 更新
		__KF_TIMER_FUNCTION__( OnTimerUpdateLoginGameToWorld );

	protected:
		// 连接丢失
		__KF_SERVER_LOST_FUNCTION__( OnServerLostHandle );

	private:
		// 发送登录回应消息
		void SendLoginGameMessage( uint32 result, uint32 playerid, uint32 gateid, const KFMsg::PBObject* playerdata );
	};
}



#endif