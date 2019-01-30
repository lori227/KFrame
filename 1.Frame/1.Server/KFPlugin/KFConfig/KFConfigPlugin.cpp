#include "KFConfigPlugin.hpp"
#include "KFConfigModule.hpp"
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