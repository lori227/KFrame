#include "KFPauseState.h"
#include "KFRobot.h"

namespace KFrame
{
	void KFPauseState::EnterState( KFRobot* kfrobot )
	{
		kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
	}

	void KFPauseState::LeaveState( KFRobot* kfrobot )
	{
		kfrobot->StopTimer();
	}

	void KFPauseState::CheckState( KFRobot* kfrobot )
	{

	}

	void KFPauseState::RunState( KFRobot* kfrobot )
	{
		if ( !kfrobot->DoneTimer() )
		{
			return;
		}
	}
}

