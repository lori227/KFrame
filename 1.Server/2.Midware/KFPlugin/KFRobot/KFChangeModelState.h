#ifndef __KF_CHANGE_MODEL_STATE_H__
#define __KF_CHANGE_MODEL_STATE_H__

#include "KFState.h"

namespace KFrame
{
    class KFChangeModelState : public KFState
    {
    public:
        // 进入状态
        virtual void EnterState( KFRobot* kfrobot );

        // 检查状态
        virtual void CheckState( KFRobot* kfrobot );

        // 状态逻辑
        virtual void RunState( KFRobot* kfrobot );

        // 离开状态
        virtual void LeaveState( KFRobot* kfrobot );
    };
}

#endif//__KF_CHANGE_MODEL_STATE_H__