#ifndef __KF_ITEM_MOUDLE_H__
#define __KF_ITEM_MOUDLE_H__

/************************************************************************
//    @Module			:    物品系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-22
************************************************************************/

#include "KFrame.h"
#include "KFItemInterface.h"
#include "KFLua/KFLuaInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFItemConfig.hpp"

namespace KFrame
{
    class KFComponent;
    class KFItemModule : public KFItemInterface
    {
    public:
        KFItemModule() = default;
        ~KFItemModule() = default;

        // 初始化
        virtual void InitModule();

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
        // 添加物品
        __KF_ADD_ELEMENT_FUNCTION__( AddItemElement );

        // 判断物品
        __KF_CHECK_ELEMENT_FUNCTION__( CheckItemElement );

        // 删除物品
        __KF_REMOVE_ELEMENT_FUNCTION__( RemoveItemElement );

        // 物品数量更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnItemCountUpdateCallBack );

        // 删除物品回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveItemCallBack );

    protected:

        // 添加新物品
        uint64 AddNewItemData( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* itemsetting, uint64 count );

        // 添加数量叠加数据
        uint64 AddOverlayCountItemData( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* itemsetting, uint64 count );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 调用函数lua函数
        void CallItemLuaFunction( KFEntity* player, const KFItemSetting* itemsetting, uint32 functiontype );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // 赋值新物品
        KFData* MoveItem( KFEntity* player, KFData* kfsource, KFData* kftarget, uint64 itemcount );

    protected:
        KFComponent* _kf_component{ nullptr };
    };
}



#endif