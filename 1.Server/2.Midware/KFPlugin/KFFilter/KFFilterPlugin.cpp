#include "KFFilterPlugin.h"
#include "KFFilterModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFFilterPlugin::Install()
    {
        __REGISTER_MODULE__( KFFilter );
    }

    void KFFilterPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFFilter );
    }

    void KFFilterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }
}