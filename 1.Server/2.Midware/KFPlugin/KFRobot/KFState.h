#ifndef __KF_STATE_H__
#define __KF_STATE_H__

#include "KFrame.h"
#include "KFStateDefine.h"
#include "KFRobot.h"

namespace KFrame
{
    class KFRobot;
    class KFState
    {
    public:
        KFState() : _state( 0 ) {}
        virtual ~KFState() {}

        // 进入状态
        virtual void EnterState( KFRobot* kfrobot ) = 0;

        // 检查状态
        virtual void CheckState( KFRobot* kfrobot ) = 0;

        // 状态逻辑
        virtual void RunState( KFRobot* kfrobot ) = 0;

        // 离开状态
        virtual void LeaveState( KFRobot* kfrobot ) = 0;

    public:
        // 状态id
        uint32 _state;
    };
}

#endif