#ifndef __KF_TIMER_MODULE_H__
#define __KF_TIMER_MODULE_H__

/************************************************************************
//    @Module			:    游戏定时器
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-20
************************************************************************/

#include "KFrame.h"
#include "KFTimerManage.h"
#include "KFTimerInterface.h"

namespace KFrame
{
	class KFTimerModule : public KFTimerInterface
	{
	public:
		KFTimerModule();
		~KFTimerModule();
		
		virtual void BeforeRun();
		
		// 关闭
		virtual void ShutDown();

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		// 获得定时器剩余时间
		virtual uint32 FindLeftTime( const std::string& module, uint64 objectid );

	protected:
		// 逻辑
		void Run();

		// 注册定时器
		virtual void AddLoopTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function );
		virtual void AddLimitTimer( const std::string& module, uint64 objectid, uint32 intervaltime, uint32 count, KFTimerFunction& function );
		virtual void AddDelayTimer( const std::string& module, uint64 objectid, uint32 intervaltime, KFTimerFunction& function );
			
		// 删除定时器
		virtual bool RemoveTimer( const std::string& module );
		virtual bool RemoveTimer( const std::string& module, uint64 objectid );
	};
}



#endif