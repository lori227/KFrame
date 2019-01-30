#ifndef __KF_LEAVE_MODULE_H__
#define __KF_LEAVE_MODULE_H__

/************************************************************************
//    @Moudle			:    角色离开游戏模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-10
************************************************************************/

#include "KFrame.h"
#include "KFLeaveInterface.h"
#include "KFLua/KFLuaInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFPlayer/KFPlayerInterface.h"

namespace KFrame
{
    class KFEntity;
    class KFLeaveModule : public KFLeaveInterface
    {
    public:
        KFLeaveModule() = default;
        ~KFLeaveModule() = default;

        // 加载配置
        virtual void InitModule();
        virtual void BeforeRun();
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:

        // 离开游戏世界
        void LeaveGameWorld( KFEntity* player );
    };
}



#endif