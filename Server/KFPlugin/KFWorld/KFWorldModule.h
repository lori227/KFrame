#ifndef __KF_WORLD_MODULE_H__
#define __KF_WORLD_MODULE_H__

/************************************************************************
//    @Moudle			:    世界功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFrame.h"
#include "KFWorldInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFConnection/KFConnectionInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"

namespace KFrame
{
	class KFWorldModule : public KFWorldInterface
	{
	public:
		KFWorldModule();
		~KFWorldModule();

		// 初始化
		virtual void InitModule();

		// 刷新
		virtual void BeforeRun();
		virtual void OnceRun();

		// 关闭
		virtual void BeforeShut();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		// 创建代理玩家
		virtual KFOnline* CreateOnline( uint32 playerid );

		// 查找代理玩家
		virtual KFOnline* FindOnline( uint32 playerid );

		// 删除代理玩家
		virtual bool RemoveOnline( uint32 playerid );

		// 踢掉在线玩家
		virtual bool KickOnline( uint32 playerid, const char* function, uint32 line );

		// 在线玩家的总人数
		virtual uint32 GetOnlineCount();

		// 发送消息到玩家
		bool SendMessageToOnline( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message );

		// 发送消息到游戏服务器
		virtual bool SendMessageToGame( uint32 gameid, uint32 msgid, ::google::protobuf::Message* message );
		/////////////////////////////////////////////////////////////////////////////////
	protected:
		// 断开Game
		__KF_SERVER_LOST_FUNCTION__( OnServerLostGame );

	protected:
		// 在线玩家同步
		__KF_MESSAGE_FUNCTION__( HandleGameSyncOnlineReq );

		// 处理消息转发
		__KF_MESSAGE_FUNCTION__( HandleTransmitMessageReq );

		// 处理广播消息
		__KF_MESSAGE_FUNCTION__( HandleBroadcastMessageReq );

		// 玩家进入游戏
		__KF_MESSAGE_FUNCTION__( HandlePlayerEnterWorldReq );

		// 玩家离开游戏
		__KF_MESSAGE_FUNCTION__( HandlePlayerLeaveWorldReq );

	protected:
		// 处理踢人
		std::string HandleHttpKickOnline( const std::string& ip, const std::string& data );

		// 更新玩家在线状态
		void UpdateOnlineToPlatfrom( uint32 accountid, uint32 playerid, uint32 online );

	private:
		// 代理玩家列表
		KFMap< uint32, uint32, KFOnline > _online_list;
	};
}

#endif