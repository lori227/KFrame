#ifndef __KF_SKIN_MOUDLE_H__
#define __KF_SKIN_MOUDLE_H__

/************************************************************************
//    @Module			:    时装包裹系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-4-2
************************************************************************/

#include "KFSkinInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFSkinConfig.hpp"

namespace KFrame
{
    class KFSkinModule : public KFSkinInterface
    {
    public:
        KFSkinModule() = default;
        ~KFSkinModule() = default;


        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 添加皮肤
        __KF_ADD_ELEMENT_FUNCTION__( AddSkinElement );

        // 添加皮肤回调
        __KF_ADD_DATA_FUNCTION__( OnAddSkinCallBack );

        // 删除皮肤回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveSkinCallBack );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 初始化
        void OnEnterStartSkinTimer( EntityPtr player );
        void OnLeaveStopSkinTimer( EntityPtr player );

        // 检查启动时间定时器
        void CheckStartSkinTimer( EntityPtr player );

        // 删除超时的皮肤
        void RemoveInvalidTimeSkin( EntityPtr player );

    protected:
        // 检测时间道具
        __KF_TIMER_FUNCTION__( OnTimerCheckSkinValidTime );
    protected:
        // 玩家组件上下文
        std::shared_ptr<KFComponent> _kf_component = nullptr;
    };
}



#endif