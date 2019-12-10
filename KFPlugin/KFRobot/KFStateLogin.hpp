#ifndef __KF_STATE_LOGIN_H__
#define __KF_STATE_LOGIN_H__

#include "KFState.hpp"

namespace KFrame
{
    class KFStateLogin : public KFState
    {
    public:
        // 进入状态
        virtual void EnterState( KFFsm* fsm, KFRobot* robot );

        // 检查状态
        virtual bool CheckState( KFFsm* fsm, KFRobot* robot );

        // 状态逻辑
        virtual void RunState( KFFsm* fsm, KFRobot* robot );
    };
}

#endif