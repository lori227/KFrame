#ifndef __KF_FSM_H__
#define __KF_FSM_H__

#include "KFrame.h"

namespace KFrame
{
    class KFState;
    class KFRobot;
    class KFFsm
    {
    public:
        KFFsm( KFRobot* robot );
        ~KFFsm();

        // 切换状态
        void ChangeState( uint32 state );

        // 执行
        void Run();
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        // 开启定时器
        void StartTimer( uint64 starttime, uint64 intervaltime );

        // 停止定时器
        void StopTimer();

        // 定时器是否完成
        bool DoneTimer();
        //////////////////////////////////////////////////////////////////////////

    protected:
        // 机器人指针
        KFRobot* _robot;

        // 当前状态
        KFState* _state;

        // 定时器
        KFClockTimer _fsm_timer;
    };
}

#endif