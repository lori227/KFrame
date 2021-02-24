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
        virtual uint32 MoveItem( EntityPtr player, DataPtr kfsourcerecord, DataPtr kfsourceitem, DataPtr kftargetrecord, uint32 targetindex );

        // 查找索引的道具
        virtual DataPtr FindIndexItem( EntityPtr player, DataPtr kfitemrecord, uint32 index );

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
        void InitItemEmptyIndexData( EntityPtr player, DataPtr kfitembag, const KFItemBagSetting* kfbagsetting );

        // 删除格子信息
        void UnInitItemEmptyIndexData( EntityPtr player, const std::string& name );

        // 清空格子信息
        void AddItemEmptyIndex( EntityPtr player, DataPtr kfitembag, DataPtr kfitem );

        // 最大索引
        uint32 GetItemMaxIndex( EntityPtr player, DataPtr kfitembag );

        // 添加最大索引
        void AddItemMaxIndex( EntityPtr player, DataPtr kfitembag, uint32 count );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 计算可以叠加物品数量
        uint32 CalcItemAddCount( uint32 sourcecount, uint32 targetcount, uint32 maxcount );

        // 判断是否能移动道具
        bool CheckItemCanMove( const KFItemSetting* setting, const std::string& sourcename, const std::string& targetname );

        // 找到可以移动的背包
        DataPtr FindCanMoveItemBag( EntityPtr player, const KFItemSetting* setting, const std::string& sourcename, const std::string& excludename );

        // 判断是否能合并
        bool CheckItemCanMerge( const KFItemSetting* kfsourcesetting, DataPtr kfsourceitem, const KFItemSetting* kftargetsetting, DataPtr kftargetitem );

        // 拆分道具
        uint32 SplitItemLogic( EntityPtr player, const KFItemSetting* kfitemsetting,
                               const KFItemBagSetting* kfsourcebagsetting, DataPtr kfsourcerecord, DataPtr kfsourceitem, uint32 splitcount,
                               const KFItemBagSetting* kftargetbagsetting, DataPtr kftargetrecord, uint32 splitindex );

        // 移动道具逻辑
        uint32 MoveItemDataLogic( EntityPtr player, const KFItemSetting* kfitemsetting,
                                  const KFItemBagSetting* kfsourcebagsetting, DataPtr kfsourcerecord, DataPtr kfsourceitem,
                                  const KFItemBagSetting* kftargetbagsetting, DataPtr kftargetrecord );
        // 移动堆叠道具数量
        void MoveItemCountLogic( EntityPtr player,  const KFItemSetting* kfitemsetting,
                                 const KFItemBagSetting* kfsourcebagsetting, DataPtr kfsourceitem, uint32 movecount,
                                 const KFItemBagSetting* kftargetbagsetting, DataPtr kftargetitem );

        // 移动道具
        uint32 MoveItemLogic( EntityPtr player, const KFItemSetting* kfitemsetting,
                              const KFItemBagSetting* kfsourcebagsetting, DataPtr kfsourcerecord, DataPtr kfsourceitem,
                              const KFItemBagSetting* kftargetbagsetting, DataPtr kftargetrecord, uint32 targetindex );

        // 交换道具
        uint32 ExchangeItemLogic( EntityPtr player,
                                  const KFItemSetting* kfsourcesetting, const KFItemBagSetting* kfsourcebagsetting, DataPtr kfsourcerecord, DataPtr kfsourceitem,
                                  const KFItemSetting* kftargetsetting, const KFItemBagSetting* kftargetbagsetting, DataPtr kftargetrecord, DataPtr kftargetitem );

        // 合并道具
        uint32 MergeItemLogic( EntityPtr player, const KFItemSetting* kfitemsetting,
                               const KFItemBagSetting* kfsourcebagsetting, DataPtr kfsourceitem, uint32 mergecount,
                               const KFItemBagSetting* kftargetbagsetting, DataPtr kftargetitem );

        // 排序道具
        void SortItem( EntityPtr player, const std::string& bagname, const std::string& tabname );
        void SortItem( EntityPtr player, const KFItemSetting* kfitemsetting, const KFItemBagSetting* kfbagsetting, KFItemTabIndex* kftabindex, DataPtr kfitemrecord, std::set<DataPtr>& itemlist );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 移动物品显示
        void MoveItemDataToShow( EntityPtr player, const KFItemSetting* setting, DataPtr kfsourcerecord, DataPtr kftargetrecord, DataPtr kfitem );
        void MoveItemDataToShow( EntityPtr player, const KFItemSetting* setting, DataPtr kfsourcerecord, DataPtr kftargetrecord, uint32 count );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 移动背包道具
        uint32 MoveBagItem( EntityPtr player, const std::string& sourcename, uint64 itemuuid, const std::string& targetname, uint32 targetindex );

        // 移动页签道具
        uint32 MoveTabItem( EntityPtr player, const std::string& bagname, const std::string& tabname, uint64 itemuuid, uint32 targetindex );
    protected:
        // 玩家组件上下文
        std::shared_ptr<KFComponent> _kf_component = nullptr;

        // 保存玩家的背包格子信息
        typedef std::pair<uint64, std::string> ItemIndexKey;
        KFMap< ItemIndexKey, KFItemBagIndex > _player_item_index;
    };
}

#endif