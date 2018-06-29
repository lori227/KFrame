#include "KFQueryGuestState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFQueryGuestState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFQueryGuestState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFQueryGuestState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFQueryGuestState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        // TODO：硬编码
        if ( kfrobot->_loop_wait_times >= _kf_robot_config->_next_state_cryl_time )
        {
            kfrobot->ChangeStateProxy();
            kfrobot->_loop_wait_times = 0;
            return;
        }

        kfrobot->QueryGuest();
        ++kfrobot->_loop_wait_times;
    }
}

