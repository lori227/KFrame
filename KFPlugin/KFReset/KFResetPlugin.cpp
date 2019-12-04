#include "KFResetPlugin.hpp"
#include "KFResetModule.hpp"
#include "KFEnterModule.hpp"
#include "KFLeaveModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFResetPlugin::Install()
    {
        __REGISTER_MODULE__( KFReset );
        __REGISTER_MODULE__( KFEnter );
        __REGISTER_MODULE__( KFLeave );
    }

    void KFResetPlugin::UnInstall()
    {
        __UN_MODULE__( KFReset );
        __UN_MODULE__( KFEnter );
        __UN_MODULE__( KFLeave );
    }

    void KFResetPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_lua, KFLuaInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
    }

    void KFResetPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFTimeConfig );
        __KF_ADD_CONFIG__( KFResetConfig );
        __KF_ADD_CONFIG__( KFEnterConfig );
        __KF_ADD_CONFIG__( KFLeaveConfig );
    }

}