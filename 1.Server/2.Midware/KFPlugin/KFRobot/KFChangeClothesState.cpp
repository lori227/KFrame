#include "KFChangeClothesState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFChangeClothesState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFChangeClothesState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFChangeClothesState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFChangeClothesState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        // TODO：硬编码
        if ( kfrobot->_loop_wait_times >= _kf_robot_config->_next_state_cryl_time )
        {

            //kfrobot->ChangeState( RobotStateEnum::SendChat );
            kfrobot->ChangeStateProxy();
            kfrobot->_loop_wait_times = 0;
            return;
        }

        kfrobot->ChangeClothes();
        ++kfrobot->_loop_wait_times;
    }
}

