#include "KFConfigPlugin.h"
#include "KFConfigModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFConfigPlugin::Install()
    {
        __REGISTER_MODULE__( KFConfig );
    }

    void KFConfigPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFConfig );
    }

    void KFConfigPlugin::LoadModule()
    {
    }
}