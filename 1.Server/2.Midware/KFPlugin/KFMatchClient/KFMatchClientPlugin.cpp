#include "KFMatchClientPlugin.h"
#include "KFMatchClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMatchClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFMatchClient );
    }

    void KFMatchClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFMatchClient );
    }

    void KFMatchClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_zone, KFZoneInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_achieve, KFAchieveInterface );
        __FIND_MODULE__( _kf_battle, KFBattleClientInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
    }
}