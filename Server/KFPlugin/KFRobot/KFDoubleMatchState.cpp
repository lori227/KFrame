#include "KFDoubleMatchState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFDoubleMatchState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFDoubleMatchState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFDoubleMatchState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFDoubleMatchState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        kfrobot->StartMatch( KFRObotEnum::Double_Match, false );
        ++kfrobot->_loop_wait_times;
    }
}

