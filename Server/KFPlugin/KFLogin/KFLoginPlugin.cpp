#include "KFLoginPlugin.h"
#include "KFLoginModule.h"

namespace KFrame
{
    void KFLoginPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFLoginPlugin, KFLoginInterface >( new KFLoginModule() );
    }

    void KFLoginPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFLoginPlugin, KFLoginInterface >();
    }

    void KFLoginPlugin::LoadModule()
    {
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
    }

}

