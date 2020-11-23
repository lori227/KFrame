#ifndef __KF_ITEM_USE_MOUDLE_H__
#define __KF_ITEM_USE_MOUDLE_H__

/************************************************************************
//    @Module			:    物品系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-22
************************************************************************/

#include "KFItemInterface.h"
#include "KFItemUseInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFConfig/KFItemConfig.hpp"
#include "KFConfig/KFItemTypeConfig.hpp"

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

    protected:
        virtual void BindCheckItemUseFunction( uint32 itemtype, const std::string& module, KFItemUseFunction& function );
        virtual void UnBindCheckItemUseFunction( uint32 itemtype, const std::string& module );

        virtual void BindItemUseFunction( uint32 itemtype, const std::string& module, KFItemUseFunction& function );
        virtual void UnBindItemUseFunction( uint32 itemtype, const std::string& module );

        // 判断道具是否能使用
        bool CheckCanUseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting );

        // 使用道具
        bool UseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting );

        // 使用扣除物品
        void UseCoseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting );

    private:
        // 检查道具是否能使用
        KFFunctionMap<uint32, KFItemUseFunction > _check_item_use_function;

        // 使用道具
        KFFunctionMap<uint32, KFItemUseFunction > _item_use_function;
    };
}

#endif