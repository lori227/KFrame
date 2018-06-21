#ifndef __KF_FSM_H__
#define __KF_FSM_H__

#include "KFrame.h"
#include "KFState.h"

namespace KFrame
{
	class KFFSM
	{
	public:
		KFFSM( KFRobot* robot );
		~KFFSM();

		// 切换状态
		void ChangeState(  uint32 state );

		// 执行
		void Run();

	protected:
		// 机器人指针
		KFRobot* _robot;

	public:
		// 当前状态
		KFState* _kf_state;
	};
}

#endif