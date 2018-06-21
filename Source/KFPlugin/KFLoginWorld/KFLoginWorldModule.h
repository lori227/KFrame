#ifndef __KF_LOGIN_WORLD_MODULE_H__
#define __KF_LOGIN_WORLD_MODULE_H__

/************************************************************************
//    @Moudle			:    登录节点模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-18
************************************************************************/

#include "KFrame.h"
#include "KFLoginWorldInterface.h"
#include "KFWorld/KFWorldInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
	class KFLoginWorldModule : public KFLoginWorldInterface
	{
	public:
		KFLoginWorldModule();
		~KFLoginWorldModule();
		
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

		// 处理删除登录
		__KF_MESSAGE_FUNCTION__( HandleLoginProxyRemoveReq );

		// 处理更新token信息
		__KF_MESSAGE_FUNCTION__ ( HandleLoginGameUpdateReq );

		// 验证token
		__KF_MESSAGE_FUNCTION__ ( HandleLoginWorldVerifyReq );

		// 更新token回馈
		__KF_MESSAGE_FUNCTION__( HandleLoginTellTokenToWorldAck );

	protected:
		// 断开连接
		__KF_SERVER_LOST_FUNCTION__( OnServerLostGame );

	private:
		// 发送验证结果消息
		void SendLoginWorldVerifyMessage( uint32 serverid, uint32 gateid, uint32 result, uint32 accountid, uint32 playerid, const std::string& token, const std::string& ip, uint32 prot );
	};
}



#endif