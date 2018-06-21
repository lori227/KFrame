#include "KFChangeIconState.h"
#include "KFRobot.h"

namespace KFrame
{
	void KFChangeIconState::EnterState( KFRobot* kfrobot )
	{
		// 启动定时器, 10秒认证一次
		kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
	}

	void KFChangeIconState::LeaveState( KFRobot* kfrobot )
	{
		kfrobot->StopTimer();
	}

	void KFChangeIconState::CheckState( KFRobot* kfrobot )
	{

	}

	void KFChangeIconState::RunState( KFRobot* kfrobot )
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

		kfrobot->ChangeIcon();
		++kfrobot->_loop_wait_times;
	}
}

