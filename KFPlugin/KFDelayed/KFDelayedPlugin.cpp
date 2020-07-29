#include "KFDelayedPlugin.hpp"
#include "KFDelayedModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDelayedPlugin::Install()
    {
        __REGISTER_MODULE__( KFDelayed );
    }

    void KFDelayedPlugin::UnInstall()
    {
        __UN_MODULE__( KFDelayed );
    }

    void KFDelayedPlugin::LoadModule()
    {

    }
}