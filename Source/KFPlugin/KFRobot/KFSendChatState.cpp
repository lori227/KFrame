#include "KFSendChatState.h"
#include "KFRobot.h"

namespace KFrame
{
	void KFSendChatState::EnterState( KFRobot* kfrobot )
	{
		// 启动定时器, 10秒认证一次
		kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
	}

	void KFSendChatState::LeaveState( KFRobot* kfrobot )
	{
		kfrobot->StopTimer();
	}

	void KFSendChatState::CheckState( KFRobot* kfrobot )
	{

	}

	void KFSendChatState::RunState( KFRobot* kfrobot )
	{
		if ( !kfrobot->DoneTimer() )
		{
			return;
		}

		// TODO：硬编码
		if ( kfrobot->_loop_wait_times >= _kf_robot_config->_next_state_cryl_time )
		{
			//超过6 * 10秒自动切换到下一个状态
			//kfrobot->ChangeState( RobotStateEnum::EnterChat );
			kfrobot->ChangeStateProxy();
			kfrobot->_loop_wait_times = 0;
			return;
		}

		kfrobot->SendChatMessage();
		++kfrobot->_loop_wait_times;
	}
}

