#include "KFDelMailState.h"
#include "KFRobot.h"

namespace KFrame
{
	void KFDelMailState::EnterState( KFRobot* kfrobot )
	{
		// 启动定时器, 10秒认证一次
		kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
	}

	void KFDelMailState::LeaveState( KFRobot* kfrobot )
	{
		kfrobot->StopTimer();
	}

	void KFDelMailState::CheckState( KFRobot* kfrobot )
	{

	}

	void KFDelMailState::RunState( KFRobot* kfrobot )
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

		kfrobot->DelMail();
		++kfrobot->_loop_wait_times;
	}
}

