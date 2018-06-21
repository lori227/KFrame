#ifndef __KF_ROBOT_MODULE_H__
#define __KF_ROBOT_MODULE_H__

/************************************************************************
//    @Moudle			:    机器人模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-21
************************************************************************/

#include "KFrame.h"
#include "KFRobotInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFRobot.h"

namespace KFrame
{
	class KFRobotModule : public KFRobotInterface
	{
	public:
		KFRobotModule();
		~KFRobotModule();

		// 初始化
		virtual void InitModule();

		// 刷新
		virtual void BeforeRun();
		virtual void OnceRun();

		// 关闭
		virtual void BeforeShut();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// 发送消息到认证服务器
		//virtual bool SendJsonMessageToAuth();	
	protected:
		// 登录机器人定时器
		__KF_TIMER_FUNCTION__( OnTimerCreateRobot );

	protected:
		// 客户端连接
		__KF_CLIENT_CONNECT_FUNCTION__( OnClientConnected );
		// 客户端断开
		__KF_CLIENT_LOST_FUNCTION__( OnClientDisconnect );

	protected:

		void Run();

		void ChangeState( const VectorString& param );

	private:
		void HandleNetMessage( const KFGuid& guid, uint32 msgid, const char* data, uint32 length );
	
	private:
		// 属性组件
		KFComponent * _kf_component;

		// 客户端链接管理
		KFNetClientEngine* _net_client;

		// 创建的数量
		uint32 _robot_count;

		// 机器人列表
		KFMap< uint32, uint32, KFRobot > _robot_list;
	protected:
		// gate服务器返回验证结果
		__KF_MESSAGE_FUNCTION__( HandleLoginVerifyAck );

		// 登录接受到玩家数据
		__KF_MESSAGE_FUNCTION__( HandleEnterGame );
		
		// 各种结果反馈
		__KF_MESSAGE_FUNCTION__( HandleResultDisplay );

		// 游戏属性更新
		__KF_MESSAGE_FUNCTION__( HandleUpdateData );

		// 游戏属性添加
		__KF_MESSAGE_FUNCTION__( HandleAddData );

		// 游戏属性移除
		__KF_MESSAGE_FUNCTION__( HandleRemoveData );


		///////////////////////////// 业务逻辑/////////////////////////////////////////////

		// 江湖聊天消息显示
		__KF_MESSAGE_FUNCTION__( HandleDisplayChatInfo );

		// 通知被踢下线
		__KF_MESSAGE_FUNCTION__( HandleBeKick );

		// 查询玩家信息
		__KF_MESSAGE_FUNCTION__( HandleQueryPlayAck );

		// 好友聊天信息显示
		__KF_MESSAGE_FUNCTION__( HandleDisplayFriendChat );

		// 查询访客信息回馈
		__KF_MESSAGE_FUNCTION__( HandleQueryGuestAck );

		// 展示奖励
		__KF_MESSAGE_FUNCTION__( HandleShowReward );

		// 取消匹配回应
		__KF_MESSAGE_FUNCTION__( HandleCancleMatch );
	};
}

#endif