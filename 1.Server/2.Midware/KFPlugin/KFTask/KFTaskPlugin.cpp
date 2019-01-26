#include "KFTaskPlugin.h"
#include "KFTaskModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTaskPlugin::Install()
    {
        __REGISTER_MODULE__( KFTask );
    }

    void KFTaskPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFTask );
    }

    void KFTaskPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
    }

}