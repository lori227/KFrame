#include "KFGiftPlugin.h"
#include "KFGiftModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFGiftPlugin::Install()
    {
        __REGISTER_MODULE__( KFGift );
    }

    void KFGiftPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFGift );
    }

    void KFGiftPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_platform, KFPlatformInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }
}