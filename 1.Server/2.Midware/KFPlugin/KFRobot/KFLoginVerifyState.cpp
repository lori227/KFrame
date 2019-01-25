#include "KFLoginVerifyState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFLoginVerifyState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFLoginVerifyState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFLoginVerifyState::CheckState( KFRobot* kfrobot )
    {
        if ( !kfrobot->_connect_ip.empty() && 0 != kfrobot->_connect_port )
        {
            kfrobot->ChangeState( RobotStateEnum::LoginGame );
        }
    }

    void KFLoginVerifyState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }
        if ( kfrobot->_state != 1000 )
        {
            //创建连接
            //kfrobot->ConnectServer( kfrobot->_gate_server_type, kfrobot->_robot_id,
            // kfrobot->_gate_name, kfrobot->_gate_ip, atoi( kfrobot->_gate_port.c_str() ) );
        }

    }
}