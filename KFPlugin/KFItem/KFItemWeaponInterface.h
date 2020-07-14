#ifndef __KF_ITEM_WEAPON_INTERFACE_H__
#define __KF_ITEM_WEAPON_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////
    class KFItemWeaponInterface : public KFModule
    {
    public:
    };
    __KF_INTERFACE__( _kf_weapon, KFItemWeaponInterface );
}

#endif