#include "KFIpAddressPlugin.h"
#include "KFIpAddressModule.h"

namespace KFrame
{
    void KFIpAddressPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFIpAddressPlugin, KFIpAddressInterface >( new KFIpAddressModule() );
    }

    void KFIpAddressPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFIpAddressPlugin, KFIpAddressInterface >();
    }

    void KFIpAddressPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
    }

}