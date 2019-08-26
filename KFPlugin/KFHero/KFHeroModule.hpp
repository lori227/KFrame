#ifndef __KF_HERO_MOUDLE_H__
#define __KF_HERO_MOUDLE_H__

/************************************************************************
//    @Moudle			:    英雄包裹系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-4-14
************************************************************************/
#include "KFHeroInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFHeroConfig.hpp"

namespace KFrame
{
    class KFHeroModule : public KFHeroInterface
    {
    public:
        KFHeroModule() = default;
        ~KFHeroModule() = default;

        // 初始化
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加物品
        __KF_ADD_ELEMENT_FUNCTION__( AddHeroElement );

        // 添加物品回调
        __KF_ADD_DATA_FUNCTION__( OnAddHeroCallBack );

        // 删除物品回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveHeroCallBack );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 检测时间道具
        __KF_TIMER_FUNCTION__( OnTimerCheckHeroValidTime );
    protected:
        // 初始化
        void OnEnterStartHeroTimer( KFEntity* player );
        void OnLeaveStopHeroTimer( KFEntity* player );

        // 检查启动时间定时器
        void CheckStartHeroTimer( KFEntity* player );

        // 删除超时的英雄
        void RemoveInvalidTimeHero( KFEntity* player );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif