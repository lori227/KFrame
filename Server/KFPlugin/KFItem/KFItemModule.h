#ifndef __KF_ITEM_MOUDLE_H__
#define __KF_ITEM_MOUDLE_H__

/************************************************************************
//    @Moudle			:    物品系统
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

namespace KFrame
{
    class KFComponent;
    class KFItemSetting;
    class KFItemModule : public KFItemInterface
    {
    public:
        KFItemModule();
        ~KFItemModule();

        // 初始化
        virtual void InitModule();

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 添加物品
        void AddItemAgentData( KFEntity* player, KFAgent* kfagent, float multiple,  const char* function, uint32 line );

        // 判断物品
        bool CheckItemAgentData( KFEntity* player, KFAgent* kfagent, const char* function, uint32 line );

        // 删除物品
        void RemoveItemAgentData( KFEntity* player, KFAgent* kfagent, const char* function, uint32 line );

        // 添加新物品
        uint32 AddNewItemData( KFEntity* player, KFData* kfparent, KFAgent* kfagent, const KFItemSetting* itemsetting, uint32 count );

        // 添加时间叠加数据
        uint32 AddOverlayTimeItemData( KFEntity* player, KFData* kfparent, KFAgent* kfagent, const KFItemSetting* itemsetting, uint32 count );

        // 添加数量叠加数据
        uint32 AddOverlayCountItemData( KFEntity* player, KFData* kfparent, KFAgent* kfagent, const KFItemSetting* itemsetting, uint32 count );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 物品数量回调
        void OnItemCountUpdateCallBack( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue );

        // 添加物品回调
        void OnAddItemCallBack( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata );

        // 删除物品回调
        void OnRemoveItemCallBack( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata );

        // 开启物品有效时间
        void OnAddItemDataStartValidTime( KFEntity* player, KFData* kfdata, uint64 key, const KFItemSetting* itemsetting );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 调用函数lua函数
        void CallItemLuaFunction( KFEntity* player, const KFItemSetting* itemsetting, uint32 functiontype );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化
        void OnEnterStartItemTimer( KFEntity* player );
        void OnLeaveStopItemTimer( KFEntity* player );

        // 开启物品超时定时器
        void CheckStartItemTimer( KFEntity* player );

        // 删除过期道具
        void RemoveValidTimeoutItem( KFEntity* player );


        // 赋值新物品
        KFData* MoveItem( KFEntity* player, KFData* kfsource, KFData* kftarget, uint32 itemcount );

    protected:
        // 检测时间道具
        __KF_TIMER_FUNCTION__( OnTimerCheckItemValidTime );

    protected:
        // 使用道具
        __KF_MESSAGE_FUNCTION__( HandleUseItemReq );

    protected:
        KFComponent* _kf_component;
    };
}



#endif