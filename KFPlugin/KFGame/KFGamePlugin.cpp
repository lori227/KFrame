#include "KFGamePlugin.hpp"
#include "KFGameModule.hpp"

namespace KFrame
{
    void KFGamePlugin::Install()
    {
        __REGISTER_MODULE__( KFGame );
    }

    void KFGamePlugin::UnInstall()
    {
        __UN_MODULE__( KFGame );
    }

    void KFGamePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_data_client, KFDataClientInterface );
        __FIND_MODULE__( _kf_deploy_client, KFDeployClientInterface );
    }

}