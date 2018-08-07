#include "KFItemPlugin.h"
#include "KFItemModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFItemPlugin::Install()
    {
        __REGISTER_MODULE__( KFItem );
    }

    void KFItemPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFItem );
    }

    void KFItemPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_lua, KFLuaInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}