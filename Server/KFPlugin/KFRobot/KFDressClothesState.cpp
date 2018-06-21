#include "KFDressClothesState.h"
#include "KFRobot.h"

namespace KFrame
{
	void KFDressClothesState::EnterState( KFRobot* kfrobot )
	{
		// 启动定时器, 10秒认证一次
		kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
	}

	void KFDressClothesState::LeaveState( KFRobot* kfrobot )
	{
		kfrobot->StopTimer();
	}

	void KFDressClothesState::CheckState( KFRobot* kfrobot )
	{

	}

	void KFDressClothesState::RunState( KFRobot* kfrobot )
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

		kfrobot->DressClothes();
		++kfrobot->_loop_wait_times;
	}
}

