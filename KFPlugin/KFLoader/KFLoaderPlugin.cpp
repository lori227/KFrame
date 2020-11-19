#include "KFLoaderPlugin.hpp"
#include "KFLoaderModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLoaderPlugin::Install()
    {
        __REGISTER_MODULE__( KFLoader );
    }

    void KFLoaderPlugin::UnInstall()
    {
        __UN_MODULE__( KFLoader );
    }

    void KFLoaderPlugin::LoadModule()
    {
    }
}