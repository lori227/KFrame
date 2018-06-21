#ifndef __KF_ACHIEVE_MOUDLE_H__
#define __KF_ACHIEVE_MOUDLE_H__

/************************************************************************
//    @Moudle			:    成就系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-20
************************************************************************/
#include "KFrame.h"
#include "KFAchieveInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
	class KFComponent;
	class KFAchieveModule : public KFAchieveInterface
	{
	public:
		KFAchieveModule();
		~KFAchieveModule();

		// 初始化
		virtual void InitModule();

		// 初始化
		virtual void BeforeRun();

		// 关闭
		virtual void BeforeShut();

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

	protected:
		// 领取奖励
		__KF_MESSAGE_FUNCTION__( HandleReceiveAchieveRewardReq );

	private:
		// 领取成就奖励
		uint32 ReceiveAchieveReward( KFEntity* player, uint32 achieveid );
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 成就数值更新回调
		void OnAchieveValueUpdateCallBack( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue );
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 属性更新回调
		void OnUpdateDataCallBack( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue );
		void OnAddDataCallBack( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata );
		void OnRemoveDataCallBack( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata );

		// 更新成就
		void UpdateDataAchieveValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue );
		void UpdateObjectAchieveValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate );

	private:
		KFComponent * _kf_component;
	};
}



#endif