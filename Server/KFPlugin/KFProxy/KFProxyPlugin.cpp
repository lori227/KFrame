#include "KFProxyPlugin.h"
#include "KFProxyModule.h"

namespace KFrame
{
    void KFProxyPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFProxyPlugin, KFProxyInterface >( new KFProxyModule() );
    }

    void KFProxyPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFProxyPlugin, KFProxyInterface >();
    }

    void KFProxyPlugin::LoadModule()
    {
        _kf_gate = _kf_plugin_manage->FindModule< KFGateInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
    }

}