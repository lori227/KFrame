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
        __UN_MODULE__( KFConfig );
    }

    void KFConfigPlugin::LoadModule()
    {
    }
}