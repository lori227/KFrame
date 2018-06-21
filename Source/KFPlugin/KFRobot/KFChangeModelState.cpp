#include "KFChangeModelState.h"
#include "KFRobot.h"

namespace KFrame
{
	void KFChangeModelState::EnterState( KFRobot* kfrobot )
	{
		// 启动定时器, 10秒认证一次
		kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
	}

	void KFChangeModelState::LeaveState( KFRobot* kfrobot )
	{
		kfrobot->StopTimer();
	}

	void KFChangeModelState::CheckState( KFRobot* kfrobot )
	{

	}

	void KFChangeModelState::RunState( KFRobot* kfrobot )
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

		kfrobot->ChangeModel();
		++kfrobot->_loop_wait_times;
	}
}

