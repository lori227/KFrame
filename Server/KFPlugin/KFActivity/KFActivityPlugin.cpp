#include "KFActivityPlugin.h"
#include "KFActivityModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFActivityPlugin::Install()
    {
        __REGISTER_MODULE__( KFActivity );
    }

    void KFActivityPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFActivity );
    }

    void KFActivityPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
    }
}