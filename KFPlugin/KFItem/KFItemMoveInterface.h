#ifndef __KF_ITEM_MOVE_INTERFACE_H__
#define __KF_ITEM_MOVE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFItemSetting;
    //////////////////////////////////////////////////////////////////////////
    class KFItemMoveInterface : public KFModule
    {
    public:
        // 移动道具
        virtual uint32 MoveItem( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, uint32 targetindex ) = 0;

        // 查找索引的道具
        virtual KFData* FindIndexItem( KFEntity* player, KFData* kfitemrecord, uint32 index ) = 0;

        // 结算背包
        virtual void BalanceItem( KFEntity* player, const std::string& name, bool autodestory ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_item_move, KFItemMoveInterface );
    ///////////////////////////////////////////////////////////////////////////
}

#endif