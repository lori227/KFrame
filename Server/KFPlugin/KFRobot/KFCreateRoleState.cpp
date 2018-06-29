#include "KFCreateRoleState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFCreateRoleState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFCreateRoleState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFCreateRoleState::CheckState( KFRobot* kfrobot )
    {

    }

    void KFCreateRoleState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        kfrobot->CreateRole();

    }
}

