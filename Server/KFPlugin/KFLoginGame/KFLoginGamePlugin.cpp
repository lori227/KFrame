#include "KFLoginGamePlugin.h"
#include "KFLoginGameModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLoginGamePlugin::Install()
    {
        __REGISTER_MODULE__( KFLoginGame );
    }

    void KFLoginGamePlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLoginGame );
    }

    void KFLoginGamePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_zone, KFZoneInterface );
        __FIND_MODULE__( _kf_game, KFGameInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_data, KFDataClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }
}