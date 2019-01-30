#include "KFLeavePlugin.hpp"
#include "KFLeaveModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLeavePlugin::Install()
    {
        __REGISTER_MODULE__( KFLeave );
    }

    void KFLeavePlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLeave );
    }

    void KFLeavePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_lua, KFLuaInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
    }
}