#include "KFStorePlugin.h"
#include "KFStoreModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFStorePlugin::Install()
    {
        __REGISTER_MODULE__( KFStore );
    }

    void KFStorePlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFStore );
    }

    void KFStorePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_mail, KFMailClientInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
    }
}