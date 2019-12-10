#ifndef __KF_TASK_MODULE_H__
#define __KF_TASK_MODULE_H__

/************************************************************************
//    @Module			:    任务系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-19
************************************************************************/

#include "KFTaskInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFTaskConfig.hpp"

namespace KFrame
{
    class KFTaskModule : public KFTaskInterface
    {
    public:
        KFTaskModule() = default;
        ~KFTaskModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 领取奖励
        __KF_MESSAGE_FUNCTION__( HandleTaskRewardReq );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    private:
        // 属性回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataCallBack );
        __KF_ADD_DATA_FUNCTION__( OnAddDataCallBack );
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDataCallBack );

        // 任务标记更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateTaskValueCallBack );
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateTaskStatusCallBack );

        // 更新任务
        void UpdateObjectTaskValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate );
        void UpdateDataTaskValue( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 nowvalue );

        // 领取任务奖励
        uint32 ReceiveTaskReward( KFEntity* player, uint32 taskid );
    private:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif