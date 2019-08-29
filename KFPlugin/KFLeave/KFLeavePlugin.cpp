#include "KFLeavePlugin.hpp"
#include "KFLeaveModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLeavePlugin::Install()
    {
        __REGISTER_MODULE__( KFLeave );
    }

    void KFLeavePlugin::UnInstall()
    {
        __UN_MODULE__( KFLeave );
    }

    void KFLeavePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_lua, KFLuaInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
    }

    void KFLeavePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFLeaveConfig );
    }
}