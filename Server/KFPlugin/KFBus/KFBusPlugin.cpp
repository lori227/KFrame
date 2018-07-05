#include "KFBusPlugin.h"
#include "KFBusModule.h"

namespace KFrame
{
    void KFBusPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFBusPlugin, KFBusInterface >( new KFBusModule() );
    }

    void KFBusPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFBusPlugin, KFBusInterface >();
    }

    void KFBusPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_ip_address = _kf_plugin_manage->FindModule< KFIpAddressInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
    }

}