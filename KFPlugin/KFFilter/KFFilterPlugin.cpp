#include "KFFilterPlugin.hpp"
#include "KFFilterModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFFilterPlugin::Install()
    {
        __REGISTER_MODULE__( KFFilter );
    }

    void KFFilterPlugin::UnInstall()
    {
        __UN_MODULE__( KFFilter );
    }

    void KFFilterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
    }

    void KFFilterPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFFilterConfig );
    }
}