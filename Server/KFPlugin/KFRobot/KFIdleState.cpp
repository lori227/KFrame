#include "KFIdleState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFIdleState::EnterState( KFRobot* kfrobot )
    {
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFIdleState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFIdleState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFIdleState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        if ( kfrobot->_loop_wait_times >= _kf_robot_config->_next_state_cryl_time )
        {
            kfrobot->ChangeStateProxy();
            kfrobot->_loop_wait_times = 0;
            return;
        }
        kfrobot->DisconnectServer( 2 );
        ++kfrobot->_loop_wait_times;
    }
}

