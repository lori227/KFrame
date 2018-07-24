#include "KFLogClientPlugin.h"
#include "KFLogClientModule.h"

namespace KFrame
{
    void KFLogClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFLogClient );
    }

    void KFLogClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLogClient );
    }

    void KFLogClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
    }

}