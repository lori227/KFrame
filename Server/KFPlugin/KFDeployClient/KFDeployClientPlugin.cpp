#include "KFDeployClientPlugin.h"
#include "KFDeployClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDeployClientPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFDeployClientPlugin, KFDeployClientInterface >( new KFDeployClientModule() );
    }

    void KFDeployClientPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFDeployClientPlugin, KFDeployClientInterface >();
    }

    void KFDeployClientPlugin::LoadModule()
    {
        _kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_ip_address = _kf_plugin_manage->FindModule< KFIpAddressInterface >();
    }
}