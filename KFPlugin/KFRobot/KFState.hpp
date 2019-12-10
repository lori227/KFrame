#ifndef __KF_STATE_H__
#define __KF_STATE_H__

#include "KFRobot.hpp"
#include "KFFsm.hpp"
#include "KFStateDefine.hpp"

namespace KFrame
{
    class KFState
    {
    public:
        virtual ~KFState() {}

        // 进入状态
        virtual void EnterState( KFFsm* fsm, KFRobot* robot );

        // 检查状态
        virtual bool CheckState( KFFsm* fsm, KFRobot* robot );

        // 状态逻辑
        virtual void RunState( KFFsm* fsm, KFRobot* robot );

        // 离开状态
        virtual void LeaveState( KFFsm* fsm, KFRobot* robot );

    public:
        // 状态id
        uint32 _state = 0;
    };
}

#endif