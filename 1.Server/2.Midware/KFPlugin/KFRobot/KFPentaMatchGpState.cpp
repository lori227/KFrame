#include "KFPentaMatchGpState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFPentaMatchGpState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFPentaMatchGpState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFPentaMatchGpState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFPentaMatchGpState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        kfrobot->StartMatch( KFRObotEnum::Penta_Match, true );
        ++kfrobot->_loop_wait_times;
    }
}

