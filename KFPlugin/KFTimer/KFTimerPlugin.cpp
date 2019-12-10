#include "KFTimerPlugin.hpp"
#include "KFTimerModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTimerPlugin::Install()
    {
        __REGISTER_MODULE__( KFTimer );
    }

    void KFTimerPlugin::UnInstall()
    {
        __UN_MODULE__( KFTimer );
    }

    void KFTimerPlugin::LoadModule()
    {

    }
}