#ifndef __KF_TASK_MODULE_H__
#define __KF_TASK_MODULE_H__

/************************************************************************
//    @Moudle			:    任务系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-19
************************************************************************/

#include "KFrame.h"
#include "KFTaskInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFComponent;
    class KFTaskModule : public KFTaskInterface
    {
    public:
        KFTaskModule();
        ~KFTaskModule();

        // 加载配置
        virtual void InitMoudle();

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 领取奖励
        __KF_MESSAGE_FUNCTION__( HandleReceiveTaskRewardReq );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    private:
        // 领取任务奖励
        uint32 ReceiveTaskReward( KFEntity* player, uint32 taskid );

        // 任务数值更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateTaskValueCallBack );

        // 任务标记更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateTaskFlagCallBack );
    private:
        KFComponent* _kf_component;
    };
}



#endif