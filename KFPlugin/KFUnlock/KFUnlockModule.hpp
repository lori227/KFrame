#ifndef __KF_UNLOCK_MODULE_H__
#define __KF_UNLOCK_MODULE_H__

/************************************************************************
//    @Module			:    解锁功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-10-25
************************************************************************/

#include "KFUnlockInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFUnlockConfig.hpp"

namespace KFrame
{
    class KFUnlockModule : public KFUnlockInterface
    {
    public:
        KFUnlockModule() = default;
        ~KFUnlockModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 属性添加回调
        __KF_ADD_DATA_FUNCTION__( OnAddDataUnlockModule );
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDataUnlockModule );
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataUnlockModule );

        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterUnlockModule );

    protected:
        //////////////////////////////////////////////////////////////////////////////////////
        // 解锁玩家数据
        void UnlockPlayerData( KFEntity* player, const KFUnlockSetting* kfsetting );
        void UnlockPlayerData( KFEntity* player, const KFUnlockSetting* kfsetting, KFData* kfdatarecord );

        // 判断是否需要解锁
        bool CheckNeedUnlock( KFEntity* player, const KFUnlockSetting* kfsetting, KFData* kfdatarecord );

    protected:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif