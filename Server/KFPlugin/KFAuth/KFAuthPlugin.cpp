#include "KFAuthPlugin.h"
#include "KFAuthModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFAuthPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFAuthPlugin, KFAuthInterface >( new KFAuthModule() );
    }

    void KFAuthPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFAuthPlugin, KFAuthInterface >();
    }

    void KFAuthPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_channel = _kf_plugin_manage->FindModule< KFChannelInterface >();
        _kf_ip_address = _kf_plugin_manage->FindModule< KFIpAddressInterface >();
        _kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
        _kf_http_server = _kf_plugin_manage->FindModule< KFHttpServerInterface >();
    }
}