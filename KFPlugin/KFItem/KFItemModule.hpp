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
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFConfig/KFItemConfig.hpp"
#include "KFConfig/KFItemTypeConfig.hpp"
#include "KFConfig/KFItemBagConfig.hpp"

namespace KFrame
{
    class KFItemModule : public KFItemInterface
    {
    public:
        KFItemModule() = default;
        ~KFItemModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 背包格子最大数量
        virtual uint32 GetItemBagMaxCount( EntityPtr player, DataPtr item_record );

        // 创建一个道具( 可传入一个存在的kfitem, 直接返回 )
        virtual DataPtr CreateItem( EntityPtr player, uint32 item_id, DataPtr item_data, const char* function, uint32 line );

        // 获取能添加物品数量
        virtual uint32 GetCanAddItemCount( EntityPtr player, uint32 item_id, uint32 count );

        // 查找道具  返回<背包指针, 道具指针>
        virtual std::tuple<DataPtr, DataPtr> FindItem( EntityPtr player, uint64 item_uuid ) ;

        // 获得背包空格子数量
        virtual uint32 GetItemEmptyCount( EntityPtr player, DataPtr item_record ) ;

        // 判断包裹是否满了
        virtual bool CheckItemBagFull( EntityPtr player, DataPtr item_record );

        // 获得包裹内道具数量( max_count 最多查找数量, 优化用 )
        virtual uint32 GetItemCount( EntityPtr player, DataPtr item_record, uint32 item_id, uint32 max_count = __MAX_UINT32__ );

        // 删除包裹内道具
        virtual void RemoveItem( EntityPtr player, DataPtr item_record, uint32 item_id, uint32 item_count );
        virtual uint32 RemoveItemCount( EntityPtr player, DataPtr item_data, uint32 item_count );

        // 获得背包属性
        virtual DataPtr FindItemBag( EntityPtr player, uint32 item_id );
        virtual DataPtr FindItemBag( EntityPtr player, DataPtr item_data );
    protected:
        // 删除道具
        __KF_MESSAGE_FUNCTION__( HandleRemoveItemReq, KFMsg::MsgRemoveItemReq );

        // 删除道具数量
        __KF_MESSAGE_FUNCTION__( HandleRemoveItemCountReq, KFMsg::MsgRemoveItemCountReq );

        // 出售道具
        __KF_MESSAGE_FUNCTION__( HandleSellItemReq, KFMsg::MsgSellItemReq );

    protected:
        // 获得道具数量
        __KF_GET_CONFIG_VALUE_FUNCTION__( GetItemTotalCount );

        // 判断是否能添加物品
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

        // 物品时间更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnItemTimeUpdateCallBack );

        // 删除时间道具
        __KF_TIMER_FUNCTION__( OnTimerRemoveTimeItem );

        // 进入游戏
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterItemModule );

        // 离开游戏
        __KF_PLAYER_LEAVE_FUNCTION__( OnLeaveItemModule );

    protected:
        virtual void BindItemInitFunction( uint32 item_type, KFModule* module, KFItemInitFunction& function );
        virtual void UnBindItemInitFunction( uint32 item_type, KFModule* module );

    protected:
        // 获得背包
        DataPtr FindFinalItemBag( EntityPtr player, DataPtr item_record, std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32 item_count );

        //背包是否满了
        bool CheckItemBagFull( EntityPtr player, DataPtr item_record, std::shared_ptr<const KFItemSetting> setting, uint32 item_count );

        // 添加时间叠加数量
        void AddOverlayTimeItem( EntityPtr player, DataPtr item_record, KFElementResult* element_result,
                                 std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32 count, uint32 time );

        // 添加数量叠加道具
        void AddOverlayCountItem( EntityPtr player, DataPtr item_record, KFElementResult* element_result,
                                  std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32 count );

        // 不能叠加的道具
        void AddNotOverlayItem( EntityPtr player, DataPtr item_record, KFElementResult* element_result,
                                std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32 count );

        // 添加新物品
        DataPtr AddNewItemData( EntityPtr player, DataPtr item_record,
                                std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32& count, uint32 time );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 调用道具初始化函数
        void CallItemInitFunction( EntityPtr player, DataPtr item_data, std::shared_ptr<const KFItemSetting> item_setting );

        // 启动检查时间道具
        void StartItemCheckTimer( EntityPtr player, DataPtr item_data );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 计算道具添加数量
        uint32 CalcItemAddCount( uint32 source_count, uint32 target_count, uint32 max_count );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加道具结果
        void AddItemResult( KFElementResult* element_result, uint32 item_id, DataPtr item_data );
        void AddItemResult( KFElementResult* element_result, const std::string& item_name, uint32 item_id, uint32 count );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 如果不满足item_count, 返回空列表
        const std::list<DataPtr>& FindRemoveItemList( EntityPtr player, DataPtr item_record, std::shared_ptr<const KFItemSetting> setting, uint32 item_count );

    protected:
        // 玩家组件上下文
        std::shared_ptr<KFComponent> _component = nullptr;

        // 初始化
        KFMap<uint32, KFMapModuleFunction<KFModule*, KFItemInitFunction>> _item_init_function;
    };
}

#endif