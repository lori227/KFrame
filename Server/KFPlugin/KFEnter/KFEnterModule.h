#ifndef __KF_ENTER_MODULE_H__
#define __KF_ENTER_MODULE_H__

/************************************************************************
//    @Module			:    角色进入游戏模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-10
************************************************************************/

#include "KFrame.h"
#include "KFEnterInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFLua/KFLuaInterface.h"
#include "KFPlayer/KFPlayerInterface.h"

namespace KFrame
{
    class KFEntity;
    class KFEnterModule : public KFEnterInterface
    {
    public:
        KFEnterModule() = default;
        ~KFEnterModule() = default;

        // 加载配置
        virtual void InitModule();
        virtual void BeforeRun();
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:

        // 进入游戏世界
        void EnterGameWorld( KFEntity* player );
    };
}



#endif