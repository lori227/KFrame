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
        virtual uint32 MoveItem( KFEntity* player, KFData* kfsourcebag, KFData* kfsourceitem, KFData* kftargetbag, uint32 targetindex ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_item_move, KFItemMoveInterface );
    ///////////////////////////////////////////////////////////////////////////
}

#endif