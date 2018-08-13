#include "KFRankClientPlugin.h"
#include "KFRankClientModule.h"

namespace KFrame
{
    void KFRankClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFRankClient );
    }

    void KFRankClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRankClient );
    }

    void KFRankClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }

}