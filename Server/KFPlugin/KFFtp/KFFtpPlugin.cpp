#include "KFFtpPlugin.h"
#include "KFFtpModule.h"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFFtpPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFFtpPlugin, KFFtpInterface >( new KFFtpModule() );
    }

    void KFFtpPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFFtpPlugin, KFFtpInterface >();
    }

    void KFFtpPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
    }
}