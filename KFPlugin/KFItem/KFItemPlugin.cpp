#include "KFItemPlugin.hpp"
#include "KFItemModule.hpp"
#include "KFItemMoveModule.hpp"
#include "KFItemUseModule.hpp"
#include "KFLoader/KFLoaderInterface.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFItemPlugin::Install()
    {
        __REGISTER_MODULE__( KFItem );
        __REGISTER_MODULE__( KFItemUse );
        __REGISTER_MODULE__( KFItemMove );
    }

    void KFItemPlugin::UnInstall()
    {
        __UN_MODULE__( KFItem );
        __UN_MODULE__( KFItemUse );
        __UN_MODULE__( KFItemMove );
    }

    void KFItemPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_item, KFItemInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_item_move, KFItemMoveInterface );
    }

    void KFItemPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFItemConfig );
        __KF_ADD_CONFIG__( KFItemTypeConfig );
        __KF_ADD_CONFIG__( KFItemBagConfig );
    }

}