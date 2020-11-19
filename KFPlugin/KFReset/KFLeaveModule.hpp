#ifndef __KF_LEAVE_MODULE_H__
#define __KF_LEAVE_MODULE_H__

/************************************************************************
//    @Moudle			:    角色离开游戏模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-10
************************************************************************/

#include "KFLeaveInterface.h"
#include "KFLua/KFLuaInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFConfig/KFLeaveConfig.hpp"

namespace KFrame
{
    class KFLeaveModule : public KFLeaveInterface
    {
    public:
        KFLeaveModule() = default;
        ~KFLeaveModule() = default;

        virtual void BeforeRun();
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 离开游戏世界
        __KF_PLAYER_LEAVE_FUNCTION__( LeaveGameWorld );
    };
}



#endif