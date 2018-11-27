#include "KFPlatformPlugin.h"
#include "KFPlatformModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFPlatformPlugin::Install()
    {
        __REGISTER_MODULE__( KFPlatform );
    }

    void KFPlatformPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFPlatform );
    }

    void KFPlatformPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_option, KFOptionInterface );
    }

}