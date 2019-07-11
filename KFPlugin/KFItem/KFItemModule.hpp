#ifndef __KF_ITEM_MOUDLE_H__
#define __KF_ITEM_MOUDLE_H__

/************************************************************************
//    @Module			:    物品系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-22
************************************************************************/

#include "KFItemInterface.h"
#include "KFLua/KFLuaInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFOption/KFOptionInterface.h"
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
        // 拆分道具
        __KF_MESSAGE_FUNCTION__( HandleSplitItemReq );

        // 合并道具
        __KF_MESSAGE_FUNCTION__( HandleMergeItemReq );

        // 使用道具
        __KF_MESSAGE_FUNCTION__( HandleUseItemReq );

        // 移动道具(从背包到仓库 或者从仓库到背包)
        __KF_MESSAGE_FUNCTION__( HandleMoveItemReq );
    protected:
        // 判断物品
        __KF_CHECK_ADD_ELEMENT_FUNCTION__( CheckAddItemElement );

        // 添加物品
        __KF_ADD_ELEMENT_FUNCTION__( AddItemElement );

        // 判断物品
        __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( CheckRemoveItemElement );

        // 删除物品
        __KF_REMOVE_ELEMENT_FUNCTION__( RemoveItemElement );

        // 添加物品回调
        __KF_ADD_DATA_FUNCTION__( OnAddItemCallBack );

        // 物品数量更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnItemCountUpdateCallBack );

        // 删除物品回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveItemCallBack );

        // 删除时间道具
        __KF_TIMER_FUNCTION__( OnTimerRemoveTimeItem );

        // 进入游戏
        void OnEnterItemModule( KFEntity* player );

        // 离开游戏
        void OnLeaveItemModule( KFEntity* player );

        // 启动检查时间道具
        void StartItemCheckTimer( KFEntity* player );
    protected:
        // 道具栏是否满了
        bool IsItemBagFull( KFData* kfitemrecord );

        // 添加时间叠加数量
        void AddOverlayTimeItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 time );

        // 添加新物品
        void AddNewTimeItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 time );

        // 添加数量叠加数据
        uint32 AddOverlayCountItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count );

        // 添加新物品
        uint32 AddNewCountItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 调用函数lua函数
        bool CallItemLuaFunction( KFEntity* player, uint32 functiontype, const KFItemSetting* kfsetting, uint32 itemcount );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 使用道具
        bool UseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting );
        bool UseGiftBagItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting );
        bool UseScriptItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting );
    protected:
        KFComponent* _kf_component = nullptr;
    };
}

#endif