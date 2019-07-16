#include "KFDeployClientPlugin.hpp"
#include "KFDeployClientModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDeployClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFDeployClient );
    }

    void KFDeployClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFDeployClient );
    }

    void KFDeployClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
    }
}