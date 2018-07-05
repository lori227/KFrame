#include "KFDeployAgentPlugin.h"
#include "KFDeployAgentModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDeployAgentPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFDeployAgentPlugin, KFDeployAgentInterface >( new KFDeployAgentModule() );
    }

    void KFDeployAgentPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFDeployAgentPlugin, KFDeployAgentInterface >();
    }

    void KFDeployAgentPlugin::LoadModule()
    {
        _kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_ip_address = _kf_plugin_manage->FindModule<KFIpAddressInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
    }
}