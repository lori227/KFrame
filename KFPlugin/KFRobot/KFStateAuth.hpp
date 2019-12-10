#ifndef __KF_STATE_AUTH_H__
#define __KF_STATE_AUTH_H__

#include "KFState.hpp"

namespace KFrame
{
    class KFStateAuth : public KFState
    {
    public:
        // 进入状态
        virtual void EnterState( KFFsm* fsm, KFRobot* robot );

        // 状态逻辑
        virtual void RunState( KFFsm* fsm, KFRobot* robot );
    };
}

#endif