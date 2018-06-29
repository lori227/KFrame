#include "KFStoreVersionState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFStoreVersionState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFStoreVersionState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFStoreVersionState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFStoreVersionState::RunState( KFRobot* kfrobot )
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

        kfrobot->QueryStoreVersion();
        ++kfrobot->_loop_wait_times;
    }
}

