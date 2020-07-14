#include "KFItemPlugin.hpp"
#include "KFItemModule.hpp"
#include "KFItemMoveModule.hpp"
#include "KFItemUseModule.hpp"
#include "KFItemWeaponModule.hpp"
#include "KFItemRuneModule.hpp"
#include "KFConfig/KFConfigInterface.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFItemPlugin::Install()
    {
        __REGISTER_MODULE__( KFItem );
        __REGISTER_MODULE__( KFItemMove );
        __REGISTER_MODULE__( KFItemUse );
        __REGISTER_MODULE__( KFItemWeapon );
        __REGISTER_MODULE__( KFItemRune );
    }

    void KFItemPlugin::UnInstall()
    {
        __UN_MODULE__( KFItem );
        __UN_MODULE__( KFItemMove );
        __UN_MODULE__( KFItemUse );
        __UN_MODULE__( KFItemWeapon );
        __UN_MODULE__( KFItemRune );
    }

    void KFItemPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_lua, KFLuaInterface );
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_hero, KFHeroInterface );
        __FIND_MODULE__( _kf_item, KFItemInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_pve, KFPVEInterface );
        __FIND_MODULE__( _kf_item_move, KFItemMoveInterface );
        __FIND_MODULE__( _kf_realm, KFRealmInterface );
    }

    void KFItemPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFElementConfig );
        __KF_ADD_CONFIG__( KFWeightConfig );
        __KF_ADD_CONFIG__( KFItemConfig );
        __KF_ADD_CONFIG__( KFItemTypeConfig );
        __KF_ADD_CONFIG__( KFWeaponAffixConfig );
        __KF_ADD_CONFIG__( KFWeaponTypeConfig );
        __KF_ADD_CONFIG__( KFItemBagConfig );
    }

}