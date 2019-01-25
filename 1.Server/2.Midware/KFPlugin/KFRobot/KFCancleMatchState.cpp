#include "KFCancleMatchState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFCancleMatchState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFCancleMatchState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFCancleMatchState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFCancleMatchState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        if ( _invalid_int != kfrobot->_matchid )
        {
            kfrobot->CancleMatch();
        }

    }
}

