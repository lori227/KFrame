#include "KFSingleMatchState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFSingleMatchState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFSingleMatchState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFSingleMatchState::CheckState( KFRobot* kfrobot )
    {
    }

    void KFSingleMatchState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        if ( _invalid_int == kfrobot->_matchid )
        {
            kfrobot->SingleMatch();
        }
    }
}

