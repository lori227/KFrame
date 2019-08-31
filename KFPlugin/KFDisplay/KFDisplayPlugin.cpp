#include "KFDisplayPlugin.hpp"
#include "KFDisplayModule.hpp"
#include "KFNoticeModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDisplayPlugin::Install()
    {
        __REGISTER_MODULE__( KFDisplay );
        __REGISTER_MODULE__( KFNotice );
    }

    void KFDisplayPlugin::UnInstall()
    {
        __UN_MODULE__( KFDisplay );
        __UN_MODULE__( KFNotice );
    }

    void KFDisplayPlugin::LoadModule()
    {
        __FIND_MODULE_NO_LOG__( _kf_game, KFGameInterface );
        __FIND_MODULE_NO_LOG__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE_NO_LOG__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE_NO_LOG__( _kf_tcp_server, KFTcpServerInterface );
    }
}