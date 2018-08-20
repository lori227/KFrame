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
        // 检测时间道具
        __KF_TIMER_FUNCTION__( OnTimerCheckItemValidTime );

        // 使用道具
        __KF_MESSAGE_FUNCTION__( HandleUseItemReq );
    protected:
        // 添加物品
        __KF_ADD_AGENT_FUNCTION__( AddItemAgentData );

        // 判断物品
        __KF_CHECK_AGENT_FUNCTION__( CheckItemAgentData );

        // 删除物品
        __KF_REMOVE_AGENT_FUNCTION__( RemoveItemAgentData );

        // 物品数量更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnItemCountUpdateCallBack );

        // 添加物品回调
        __KF_ADD_DATA_FUNCTION__( OnAddItemCallBack );

        // 删除物品回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveItemCallBack );

    protected:

        // 添加新物品
        uint32 AddNewItemData( KFEntity* player, KFData* kfparent, KFAgent* kfagent, const KFItemSetting* itemsetting, uint32 count );

        // 添加时间叠加数据
        uint32 AddOverlayTimeItemData( KFEntity* player, KFData* kfparent, KFAgent* kfagent, const KFItemSetting* itemsetting, uint32 count );

        // 添加数量叠加数据
        uint32 AddOverlayCountItemData( KFEntity* player, KFData* kfparent, KFAgent* kfagent, const KFItemSetting* itemsetting, uint32 count );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        KFComponent* _kf_component;
    };
}



#endif