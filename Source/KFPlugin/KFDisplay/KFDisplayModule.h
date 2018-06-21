#ifndef __KF_DISPLAY_MOUDLE_H__
#define __KF_DISPLAY_MOUDLE_H__

/************************************************************************
//    @Moudle			:    结果提示系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-24
************************************************************************/

#include "KFDisplayInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFPlayer/KFPlayerInterface.h"

namespace KFrame
{
	class KFDisplayModule : public KFDisplayInterface
	{
	public:
		KFDisplayModule();
		~KFDisplayModule();

		// 初始化
		virtual void InitModule();

		// 初始化
		virtual void BeforeRun();

		// 关闭
		virtual void BeforeShut();

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
	private:
		// Gate/Proxy ==> Client
		virtual void SendToClient( uint32 playerid, uint32 result, ListString& params );

		// Game ==> Client
		virtual void SendToClient( KFEntity* player, uint32 result, ListString& params );

		// Game ==> Client
		virtual void SendToGroup( KFEntity* player, uint32 result, ListString& params );

		// Cluster ==> Game
		virtual void SendToGame( uint32 serverid, uint32 playerid, uint32 result, ListString& params );

		// Game ==> Route ==> Game
		virtual void SendToPlayer( uint32 serverid, uint32 playerid, uint32 result, ListString& params );
		virtual void SendToPlayer( KFData* kfbasic, uint32 result, ListString& params );

	};
}



#endif