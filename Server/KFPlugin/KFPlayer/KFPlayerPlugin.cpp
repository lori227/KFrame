#include "KFPlayerPlugin.h"
#include "KFPlayerModule.h"

namespace KFrame
{
    void KFPlayerPlugin::Install()
    {
        __REGISTER_MODULE__( KFPlayer );
    }

    void KFPlayerPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFPlayer );
    }

    void KFPlayerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_zone, KFZoneInterface );
        __FIND_MODULE__( _kf_game, KFGameInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_debug, KFDebugInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_data, KFDataClientInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_public, KFPublicClientInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_command, KFDeployCommandInterface );
    }

}