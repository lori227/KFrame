#ifndef __KF_CLOTHES_MOUDLE_H__
#define __KF_CLOTHES_MOUDLE_H__

/************************************************************************
//    @Moudle			:    时装包裹系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-4-2
************************************************************************/

#include "KFrame.h"
#include "KFClothesInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFMailClient/KFMailClientInterface.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
    class KFComponent;
    class KFClothesModule : public KFClothesInterface
    {
    public:
        KFClothesModule();
        ~KFClothesModule();

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
        void AddClothesAgentData( KFEntity* player, KFAgent* kfagent, float multiple, const char* function, uint32 line );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 衣服数量回调
        void OnClothesCountUpdateCallBack( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue );

        // 添加物品回调
        void OnAddClothesCallBack( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata );

        // 删除物品回调
        void OnRemoveClothesCallBack( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 初始化
        void OnEnterStartClothesTimer( KFEntity* player );
        void OnLeaveStopClothesTimer( KFEntity* player );

        // 检查启动时间定时器
        void CheckStartClothesTimer( KFEntity* player );

        // 删除超时的时装
        void RemoveValidTimeoutClothes( KFEntity* player );

    protected:
        // 检测时间道具
        __KF_TIMER_FUNCTION__( OnTimerCheckClothesValidTime );

    protected:
        // 处理穿戴时装
        __KF_MESSAGE_FUNCTION__( HandleDressClothesReq );

        // 设置模型的默认时装id
        __KF_MESSAGE_FUNCTION__( HandleSetModelDefaultClothesReq );

        // 改变模型
        __KF_MESSAGE_FUNCTION__( HandleChangeModelReq );
    protected:
        KFComponent* _kf_component;
    };
}



#endif