#include "KFQueryFriendRankState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFQueryFriendRankState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFQueryFriendRankState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFQueryFriendRankState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFQueryFriendRankState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        if ( kfrobot->_loop_wait_times >= _kf_robot_config->_next_state_cryl_time )
        {

            kfrobot->ChangeStateProxy();
            //kfrobot->ChangeState( RobotStateEnum::EnterChat );
            kfrobot->_loop_wait_times = 0;
            return;
        }

        kfrobot->QueryFriendRankList();
        ++kfrobot->_loop_wait_times;
    }
}

