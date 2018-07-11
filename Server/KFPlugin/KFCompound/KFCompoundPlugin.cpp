#include "KFCompoundPlugin.h"
#include "KFCompoundModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFCompoundPlugin::Install()
    {
        __REGISTER_MODULE__( KFCompound );
    }

    void KFCompoundPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFCompound );
    }

    void KFCompoundPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
    }
}