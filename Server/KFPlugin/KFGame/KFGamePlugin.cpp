#include "KFGamePlugin.h"
#include "KFGameModule.h"

namespace KFrame
{
    void KFGamePlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFGamePlugin, KFGameInterface >( new KFGameModule() );
    }

    void KFGamePlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFGamePlugin, KFGameInterface >();
    }

    void KFGamePlugin::LoadModule()
    {
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
    }

}