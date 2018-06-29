#include "KFDoubleMatchGpState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFDoubleMatchGpState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFDoubleMatchGpState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFDoubleMatchGpState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFDoubleMatchGpState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }


        kfrobot->StartMatch( KFRObotEnum::Double_Match, true );
        ++kfrobot->_loop_wait_times;
    }
}

