#include "KFState.hpp"

namespace KFrame
{
    void KFState::EnterState( KFFsm* fsm, KFRobot* robot )
    {

    }

    bool KFState::CheckState( KFFsm* fsm, KFRobot* robot )
    {
        return true;
    }

    void KFState::RunState( KFFsm* fsm, KFRobot* robot )
    {

    }

    void KFState::LeaveState( KFFsm* fsm, KFRobot* robot )
    {
        fsm->StopTimer();
    }
}
