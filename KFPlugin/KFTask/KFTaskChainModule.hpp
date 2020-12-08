#ifndef __KF_TASK_CHAIN_MOUDLE_H__
#define __KF_TASK_CHAIN_MOUDLE_H__

/************************************************************************
//    @Module			:    任务链系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-10-31
************************************************************************/

#include "KFTaskInterface.h"
#include "KFTaskChainInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFConfig/KFTaskChainConfig.hpp"
#include "KFConfig/KFWeightConfig.hpp"

namespace KFrame
{
    class KFTaskChainModule : public KFTaskChainInterface
    {
    public:
        KFTaskChainModule() = default;
        ~KFTaskChainModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 判断是否是任务链任务
        bool IsTaskChain( KFEntity* player, KFData* kftask );

        // 清除任务链
        void CleanTaskChain( KFEntity* player, uint32 chainid );

        // 开启任务链
        bool OpenTaskChain( KFEntity* player, uint32 chainid, uint32 chainindex, uint32 validtime );
    protected:
        // 添加任务链
        __KF_ADD_ELEMENT_FUNCTION__( AddTaskChainElement );

        // 删除任务
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveTaskTaskChainModule );

        // 开启任务链数据
        bool OpenTaskChainLogicData( KFEntity* player, const TaskChainData* taskchaindata, uint32 chainid, uint32 chainindex, uint32 validtime );

        // 开启附加任务链
        void OpenExtendChain( KFEntity* player, uint32 chainid, const TaskChainData* taskchaindata );

        // 完成任务链任务
        void FinishTaskChain( KFEntity* player, KFData* kftask );
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}

#endif