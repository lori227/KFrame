#include "KFPentaMatchState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFPentaMatchState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFPentaMatchState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFPentaMatchState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFPentaMatchState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        kfrobot->StartMatch( KFRObotEnum::Penta_Match, false );
        ++kfrobot->_loop_wait_times;
    }
}

