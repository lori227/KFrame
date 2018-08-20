#include "KFDirClientPlugin.h"
#include "KFDirClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDirClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFDirClient );
    }

    void KFDirClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDirClient );
    }

    void KFDirClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_zone, KFZoneInterface );
        __FIND_MODULE__( _kf_gate, KFGateInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
    }
}