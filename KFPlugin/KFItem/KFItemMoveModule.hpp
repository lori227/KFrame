#ifndef __KF_ITEM_MOVE_MOUDLE_H__
#define __KF_ITEM_MOVE_MOUDLE_H__

/************************************************************************
//    @Module			:    物品移动模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2020-2-28
************************************************************************/
#include "KFItemIndex.hpp"
#include "KFItemInterface.h"
#include "KFItemMoveInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFConfig/KFItemConfig.hpp"
#include "KFConfig/KFItemTypeConfig.hpp"
#include "KFConfig/KFItemBagConfig.hpp"

namespace KFrame
{
    class KFItemMoveModule : public KFItemMoveInterface
    {
    public:
        KFItemMoveModule() = default;
        ~KFItemMoveModule() = default;

        // 逻辑
        virtual void BeforeRun();
        virtual void AfterLoad();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 移动道具
        virtual uint32 MoveItem( EntityPtr player, DataPtr source_record, DataPtr source_item, DataPtr target_record, uint32 target_index );

        // 查找索引的道具
        virtual DataPtr FindIndexItem( EntityPtr player, DataPtr item_record, uint32 index );

    protected:
        // 移动道具(从背包到仓库 或者从仓库到背包)
        __KF_MESSAGE_FUNCTION__( HandleMoveItemReq, KFMsg::MsgMoveItemReq );
        __KF_MESSAGE_FUNCTION__( HandleMoveAllItemReq, KFMsg::MsgMoveAllItemReq );

        // 整理背包
        __KF_MESSAGE_FUNCTION__( HandleSortItemReq, KFMsg::MsgSortItemReq );

    protected:
        // 进入游戏
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterItemMoveModule );

        // 离开游戏
        __KF_PLAYER_LEAVE_FUNCTION__( OnLeaveItemMoveModule );

        // 添加物品回调
        __KF_ADD_DATA_FUNCTION__( OnAddItemMoveLogic );

        // 删除物品回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveItemMoveLogic );

        // 更新道具索引回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateItemTabIndexCallBack );
        ///////////////////////////////////////////////////////////////////////////
    protected:
        // 初始化道具格子信息
        void InitItemEmptyIndexData( EntityPtr player, DataPtr item_record, std::shared_ptr<const KFItemBagSetting> item_bag_setting );

        // 删除格子信息
        void UnInitItemEmptyIndexData( EntityPtr player, const std::string& name );

        // 清空格子信息
        void AddItemEmptyIndex( EntityPtr player, DataPtr item_record, DataPtr item_data );

        // 最大索引
        uint32 GetItemMaxIndex( EntityPtr player, DataPtr item_record );

        // 添加最大索引
        void AddItemMaxIndex( EntityPtr player, DataPtr item_record, uint32 count );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 计算可以叠加物品数量
        uint32 CalcItemAddCount( uint32 source_count, uint32 target_count, uint32 max_count );

        // 判断是否能移动道具
        bool CheckItemCanMove( std::shared_ptr<const KFItemSetting> setting, const std::string& source_name, const std::string& target_name );

        // 找到可以移动的背包
        DataPtr FindCanMoveItemBag( EntityPtr player, std::shared_ptr<const KFItemSetting> setting, const std::string& source_name, const std::string& exclude_name );

        // 判断是否能合并
        bool CheckItemCanMerge( std::shared_ptr<const KFItemSetting> source_item_setting, DataPtr source_item, std::shared_ptr<const KFItemSetting> target_item_setting, DataPtr target_item );

        // 拆分道具
        uint32 SplitItemLogic( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting,
                               std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_record, DataPtr source_item, uint32 split_count,
                               std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_record, uint32 split_index );

        // 移动道具逻辑
        uint32 MoveItemDataLogic( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting,
                                  std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_record, DataPtr source_item,
                                  std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_record );
        // 移动堆叠道具数量
        void MoveItemCountLogic( EntityPtr player,  std::shared_ptr<const KFItemSetting> item_setting,
                                 std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_item, uint32 move_count,
                                 std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_item );

        // 移动道具
        uint32 MoveItemLogic( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting,
                              std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_record, DataPtr source_item,
                              std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_record, uint32 target_index );

        // 交换道具
        uint32 ExchangeItemLogic( EntityPtr player,
                                  std::shared_ptr<const KFItemSetting> source_item_setting, std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_record, DataPtr source_item,
                                  std::shared_ptr<const KFItemSetting> target_item_setting, std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_record, DataPtr target_item );

        // 合并道具
        uint32 MergeItemLogic( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting,
                               std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_item, uint32 merge_count,
                               std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_item );

        // 排序道具
        void SortItem( EntityPtr player, const std::string& bag_name, const std::string& tab_name );
        void SortItem( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemBagSetting> item_bag_setting, std::shared_ptr<KFItemTabIndex> tab_index, DataPtr item_record, std::set<DataPtr>& item_list );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 移动物品显示
        void MoveItemDataToShow( EntityPtr player, std::shared_ptr<const KFItemSetting> setting, DataPtr source_record, DataPtr target_record, DataPtr item_data );
        void MoveItemDataToShow( EntityPtr player, std::shared_ptr<const KFItemSetting> setting, DataPtr source_record, DataPtr target_record, uint32 count );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 移动背包道具
        uint32 MoveBagItem( EntityPtr player, const std::string& source_name, uint64 item_uuid, const std::string& target_name, uint32 target_index );

        // 移动页签道具
        uint32 MoveTabItem( EntityPtr player, const std::string& bag_name, const std::string& tab_name, uint64 item_uuid, uint32 target_index );
    protected:
        // 玩家组件上下文
        std::shared_ptr<KFComponent> _component = nullptr;

        // 保存玩家的背包格子信息
        typedef std::pair<uint64, std::string> ItemIndexKey;
        KFMap<ItemIndexKey, KFItemBagIndex> _player_item_index;
    };
}

#endif