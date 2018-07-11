#include "KFTimerPlugin.h"
#include "KFTimerModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTimerPlugin::Install()
    {
        __REGISTER_MODULE__( KFTimer );
    }

    void KFTimerPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFTimer );
    }

    void KFTimerPlugin::LoadModule()
    {

    }
}