#include "KFItemPlugin.hpp"
#include "KFItemModule.hpp"
#include "KFItemUseModule.hpp"
#include "KFConfig/KFConfigInterface.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFItemPlugin::Install()
    {
        __REGISTER_MODULE__( KFItem );
        __REGISTER_MODULE__( KFItemUse );
    }

    void KFItemPlugin::UnInstall()
    {
        __UN_MODULE__( KFItem );
        __UN_MODULE__( KFItemUse );
    }

    void KFItemPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_lua, KFLuaInterface );
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_hero, KFHeroInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
    }

    void KFItemPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFItemConfig );
        __KF_ADD_CONFIG__( KFItemTypeConfig );
    }

}