#ifndef __KF_STATE_WAIT_H__
#define __KF_STATE_WAIT_H__

#include "KFState.hpp"

namespace KFrame
{
    class KFStateWait : public KFState
    {
    public:
        // 检查状态
        virtual bool CheckState( KFFsm* fsm, KFRobot* robot );
    };
}

#endif