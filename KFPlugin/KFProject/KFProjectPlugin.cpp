#include "KFProjectPlugin.hpp"
#include "KFProjectModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFProjectPlugin::Install()
    {
        __REGISTER_MODULE__( KFProject );
    }

    void KFProjectPlugin::UnInstall()
    {
        __UN_MODULE__( KFProject );
    }

    void KFProjectPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
    }

    void KFProjectPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFProjectConfig );
        __KF_ADD_CONFIG__( KFConstantConfig );
        __KF_ADD_CONFIG__( KFRewardConfig );
    }

}