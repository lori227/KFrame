#include "KFQueryWholeRankState.h"
#include "KFRobot.h"

namespace KFrame
{
	void KFQueryWholeRankState::EnterState( KFRobot* kfrobot )
	{
		// 启动定时器, 10秒认证一次
		kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
	}

	void KFQueryWholeRankState::LeaveState( KFRobot* kfrobot )
	{
		kfrobot->StopTimer();
	}

	void KFQueryWholeRankState::CheckState( KFRobot* kfrobot )
	{

	}

	void KFQueryWholeRankState::RunState( KFRobot* kfrobot )
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

		kfrobot->QueryWholeRankList();
		++kfrobot->_loop_wait_times;
	}
}

