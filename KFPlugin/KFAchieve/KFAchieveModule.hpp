#ifndef __KF_ACHIEVE_MOUDLE_H__
#define __KF_ACHIEVE_MOUDLE_H__

/************************************************************************
//    @Module			:    成就系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-20
************************************************************************/
#include "KFAchieveInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFAchieveConfig.hpp"

namespace KFrame
{
    class KFAchieveModule : public KFAchieveInterface
    {
    public:
        KFAchieveModule() = default;
        ~KFAchieveModule() = default;


        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 领取奖励
        __KF_MESSAGE_FUNCTION__( HandleAchieveRewardReq );

    private:
        // 领取成就奖励
        uint32 ReceiveAchieveReward( KFEntity* player, uint32 achieveid );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 属性更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataAchieveModule );
        __KF_ADD_DATA_FUNCTION__( OnAddDataAchieveModule );
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDataAchieveModule );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterAchieveModule );

    private:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif