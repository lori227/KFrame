#include "KFEnterChatState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFEnterChatState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFEnterChatState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFEnterChatState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFEnterChatState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        if ( kfrobot->_loop_wait_times >= _kf_robot_config->_next_state_cryl_time )
        {
            //超过_state_rep_time * _next_state_cryl_time秒自动切换到下一个状态
            //kfrobot->ChangeState( RobotStateEnum::SendChat );
            kfrobot->ChangeStateProxy();
            kfrobot->_loop_wait_times = 0;
            return;
        }

        kfrobot->EnterChat();
        ++kfrobot->_loop_wait_times;
    }
}

