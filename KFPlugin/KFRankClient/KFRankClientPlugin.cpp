#include "KFRankClientPlugin.hpp"
#include "KFRankClientModule.hpp"
#include "KFLoader/KFLoaderInterface.h"

namespace KFrame
{
    void KFRankClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFRankClient );
    }

    void KFRankClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFRankClient );
    }

    void KFRankClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }

    void KFRankClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRankConfig );
    }
}