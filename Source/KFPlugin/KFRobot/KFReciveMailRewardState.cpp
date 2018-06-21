#include "KFReciveMailRewardState.h"
#include "KFRobot.h"

namespace KFrame
{
	void KFReciveMailRewardState::EnterState( KFRobot* kfrobot )
	{
		// 启动定时器, 10秒认证一次
		kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
	}

	void KFReciveMailRewardState::LeaveState( KFRobot* kfrobot )
	{
		kfrobot->StopTimer();
	}

	void KFReciveMailRewardState::CheckState( KFRobot* kfrobot )
	{

	}

	void KFReciveMailRewardState::RunState( KFRobot* kfrobot )
	{
		if ( !kfrobot->DoneTimer() )
		{
			return;
		}
		// 硬编码
		if (kfrobot->_loop_wait_times >= _kf_robot_config->_next_state_cryl_time )
		{
			//kfrobot->ChangeState( RobotStateEnum::DelMail );
			kfrobot->ChangeStateProxy();
			kfrobot->_loop_wait_times = 0;
			return;
		}
		kfrobot->ReceiveMailReward();
		++kfrobot->_loop_wait_times;

	}

}

