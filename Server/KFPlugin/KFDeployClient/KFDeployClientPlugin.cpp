#include "KFDeployClientPlugin.h"
#include "KFDeployClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDeployClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFDeployClient );
    }

    void KFDeployClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDeployClient );
    }

    void KFDeployClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_command, KFDeployCommandInterface );
        __FIND_MODULE_NO_LOG__( _kf_tcp_server, KFTcpServerInterface );
    }
}