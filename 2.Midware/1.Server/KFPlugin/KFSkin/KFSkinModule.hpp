#ifndef __KF_SKIN_MOUDLE_H__
#define __KF_SKIN_MOUDLE_H__

/************************************************************************
//    @Module			:    时装包裹系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-4-2
************************************************************************/

#include "KFrame.h"
#include "KFSkinInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
    class KFComponent;
    class KFSkinModule : public KFSkinInterface
    {
    public:
        KFSkinModule() = default;
        ~KFSkinModule() = default;

        // 初始化
        virtual void InitModule();

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
        void OnEnterStartSkinTimer( KFEntity* player );
        void OnLeaveStopSkinTimer( KFEntity* player );

        // 检查启动时间定时器
        void CheckStartSkinTimer( KFEntity* player );

        // 删除超时的皮肤
        void RemoveInvalidTimeSkin( KFEntity* player );

    protected:
        // 检测时间道具
        __KF_TIMER_FUNCTION__( OnTimerCheckSkinValidTime );
    protected:
        KFComponent* _kf_component = nullptr;
    };
}



#endif