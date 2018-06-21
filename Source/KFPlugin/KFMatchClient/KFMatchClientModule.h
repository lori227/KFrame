#ifndef __KF_MATCH_CLIENT_MODULE_H__
#define __KF_MATCH_CLIENT_MODULE_H__

/************************************************************************
//    @Moudle			:    匹配客户端模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-12-20
************************************************************************/

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFZone/KFZoneInterface.h"
#include "KFMatchClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"

namespace KFrame
{
	class KFMatchClientModule : public KFMatchClientInterface
	{
	public:
		KFMatchClientModule();
		~KFMatchClientModule();

		// 加载配置
		virtual void InitModule();

		// 初始化
		virtual void BeforeRun();
		virtual void OnceRun();

		// 关闭
		virtual void BeforeShut();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		// 发送消息到Match
		virtual bool SendMessageToMatch( uint32 msgid, ::google::protobuf::Message* message );

		// 获得匹配最大人数
		virtual uint32 GetMatchMaxCount( uint32 matchid );
	protected:
		// 请求单人匹配
		__KF_MESSAGE_FUNCTION__( HandleStartMatchReq );

		// 匹配回馈消息
		__KF_MESSAGE_FUNCTION__( HanldeMatchToClientAck );

		// 取消匹配
		__KF_MESSAGE_FUNCTION__( HandleCancelMatchReq );

		// 查询匹配房间
		__KF_MESSAGE_FUNCTION__( HandleQueryMatchRoomAck );
				
	private:
		// 处理开始匹配
		uint32 ProcessStartMatch( KFEntity* player, uint32 matchid, bool allowgroup );

		// 判断所有队员准备
		bool IsAllGroupMemberPrepare( KFData* kfmemberrecord );

		// 队伍信息
		void FormatMatchGroup( KFEntity* player, KFMsg::PBMatchGroup* pbgroup );

		// 设置匹配玩家数据
		void FormatMatchPlayer( KFData* kfobject, KFMsg::PBBattlePlayer* pbplayer );
		
		// 玩家上线回调
		void OnEnterQueryMatchRoom( KFEntity* player );
	};
}



#endif