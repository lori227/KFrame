#include "KFGamePlugin.h"
#include "KFGameModule.h"

namespace KFrame
{
    void KFGamePlugin::Install()
    {
        __REGISTER_MODULE__( KFGame );
    }

    void KFGamePlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFGame );
    }

    void KFGamePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_command, KFDeployCommandInterface );
    }

}