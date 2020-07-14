#ifndef __KF_ITEM_USE_MOUDLE_H__
#define __KF_ITEM_USE_MOUDLE_H__

/************************************************************************
//    @Module			:    物品系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-22
************************************************************************/

#include "KFrameEx.h"
#include "KFItemInterface.h"
#include "KFItemUseInterface.h"
#include "KFLua/KFLuaInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFRealm/KFPVEInterface.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFExecute/KFExecuteInterface.h"

namespace KFrame
{
    class KFItemUseModule : public KFItemUseInterface
    {
    public:
        KFItemUseModule() = default;
        ~KFItemUseModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 使用道具
        __KF_MESSAGE_FUNCTION__( HandleUseItemReq );

        // 给英雄使用道具
        __KF_MESSAGE_FUNCTION__( HandleUseItemToHeroReq );

    protected:
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 使用道具
        bool UseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting );
        bool UseGiftItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting );
        bool UseDrugItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting );
        bool UseScriptItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting );

        // 判断道具是否能使用
        bool CheckCanUseItem( KFEntity* player, const KFItemSetting* kfsetting );

        // 使用扣除物品
        void UseCoseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting );
    };
}

#endif