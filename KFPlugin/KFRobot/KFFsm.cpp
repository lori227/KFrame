#include "KFFsm.hpp"
#include "KFStateManage.hpp"

namespace KFrame
{
    KFFsm::KFFsm( KFRobot* robot )
    {
        _robot = robot;
        _state = nullptr;

        // 默认为空闲状态
        ChangeState( KFrame::StateAuth );
    }

    KFFsm::~KFFsm()
    {
        _robot = nullptr;
        _state = nullptr;
    }

    void KFFsm::ChangeState( uint32 state )
    {
        if ( _state != nullptr )
        {
            _state->LeaveState( this, _robot );
            _state = nullptr;
        }

        _state = _kf_state_manage->FindState( state );
        _state->EnterState( this, _robot );
    }

    void KFFsm::Run()
    {
        if ( !DoneTimer() )
        {
            return;
        }

        // 检查状态
        auto ok = _state->CheckState( this, _robot );
        if ( ok )
        {
            // 执行状态
            _state->RunState( this, _robot );
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    void KFFsm::StartTimer( uint64 starttime, uint64 intervaltime )
    {
        _fsm_timer.StopTimer();
        _fsm_timer.StartTimer( starttime, intervaltime );
    }

    void KFFsm::StopTimer()
    {
        _fsm_timer.StopTimer();
    }

    bool KFFsm::DoneTimer()
    {
        if ( !_fsm_timer.IsStart() )
        {
            return true;
        }

        return _fsm_timer.DoneTimer( KFGlobal::Instance()->_game_time, true );
    }
}