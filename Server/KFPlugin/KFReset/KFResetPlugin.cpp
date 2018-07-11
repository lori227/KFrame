#include "KFResetPlugin.h"
#include "KFResetModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFResetPlugin::Install()
    {
        __REGISTER_MODULE__( KFReset );
    }

    void KFResetPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFReset );
    }

    void KFResetPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
    }
}