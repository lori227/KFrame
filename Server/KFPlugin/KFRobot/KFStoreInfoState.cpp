#include "KFStoreInfoState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFStoreInfoState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFStoreInfoState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFStoreInfoState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFStoreInfoState::RunState( KFRobot* kfrobot )
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

        kfrobot->QueryStoreInfo();
        ++kfrobot->_loop_wait_times;
    }
}

