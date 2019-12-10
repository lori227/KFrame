#ifndef __KF_STATE_DEFINE_H__
#define __KF_STATE_DEFINE_H__

namespace KFrame
{
    enum RobotStateEnum
    {
        StateWait = 0,		// 等待状态
        StateAuth = 1,		// 认证状态
        StateLogin = 2,		// 登陆状态
    };
}

#endif
