#ifndef __KF_PROXY_MODULE_H__
#define __KF_PROXY_MODULE_H__

/************************************************************************
//    @Moudle			:    代理玩家功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFrame.h"
#include "KFProxyInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFGate/KFGateInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"


namespace KFrame
{
	class KFProxyModule : public KFProxyInterface
	{
	public:
		KFProxyModule();
		~KFProxyModule();

		// 初始化
		virtual void InitModule();

		// 刷新
		virtual void BeforeRun();

		// 关闭
		virtual void BeforeShut();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		// 创建代理玩家
		virtual KFProxy* CreateProxy( uint32 playerid );

		// 查找代理玩家
		virtual KFProxy* FindProxy( uint32 playerid );

		// 删除代理玩家
		virtual bool RemoveProxy( uint32 playerid );

		/////////////////////////////////////////////////////////////////////////////////
	protected:
		// 发送消息到客户端
		__KF_TRANSMIT_FUNCTION__( SendMessageToClient );

		// 发送消息到游戏服务器
		__KF_TRANSMIT_FUNCTION__( SendMessageToGame );

	protected:
		// 处理消息广播
		__KF_MESSAGE_FUNCTION__( HandleBroadcastMessageReq );

		// 踢出玩家
		__KF_MESSAGE_FUNCTION__( HandleKickProxyPlayerReq );

	protected:
		// 玩家掉线
		__KF_SERVER_LOST_FUNCTION__( OnPlayerDisconnection );

	private:
		// 代理玩家列表
		KFMap< uint32, uint32, KFProxy > _proxy_list;
	};
}

#endif