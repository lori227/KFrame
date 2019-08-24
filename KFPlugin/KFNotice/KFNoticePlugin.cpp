#include "KFNoticePlugin.hpp"
#include "KFNoticeModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFNoticePlugin::Install()
    {
        __REGISTER_MODULE__( KFNotice );
    }

    void KFNoticePlugin::UnInstall()
    {
        __UN_MODULE__( KFNotice );
    }

    void KFNoticePlugin::LoadModule()
    {
        __FIND_MODULE_NO_LOG__( _kf_game, KFGameInterface );
        __FIND_MODULE_NO_LOG__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE_NO_LOG__( _kf_route, KFRouteClientInterface );
    }
}