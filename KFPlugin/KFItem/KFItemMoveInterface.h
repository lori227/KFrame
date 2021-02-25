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
        virtual uint32 MoveItem( EntityPtr player, DataPtr source_record, DataPtr source_item, DataPtr target_record, uint32 target_index ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_item_move, KFItemMoveInterface );
    ///////////////////////////////////////////////////////////////////////////
}

#endif