#include "KFFSM.h"
#include "KFStateManage.h"

namespace KFrame
{
	KFFSM::KFFSM( KFRobot* robot )
	{
		_robot = robot;
		_kf_state = nullptr;
	}

	KFFSM::~KFFSM()
	{
		_robot = nullptr;
		_kf_state = nullptr;
	}

	void KFFSM::ChangeState( uint32 state )
	{
		if ( _kf_state != nullptr )
		{
			_kf_state->LeaveState( _robot );
			_kf_state = nullptr;
		}

		auto kfstate = _kf_state_manage->FindState( state );
		if (nullptr == kfstate)
		{
			std::cout << "ChangeState Error ! state: "<<state << std::endl;
			return;
		}


		_kf_state = kfstate;
		_kf_state->EnterState( _robot );
	}

	void KFFSM::Run()
	{
		// 检查状态
		_kf_state->CheckState( _robot );

		// 执行状态
		_kf_state->RunState( _robot );
	}
}
