#include "KFHttpClientPlugin.h"
#include "KFHttpClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFHttpClientPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFHttpClientPlugin, KFHttpClientInterface >( new KFHttpClientModule() );
    }

    void KFHttpClientPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFHttpClientPlugin, KFHttpClientInterface >();
    }

    void KFHttpClientPlugin::LoadModule()
    {

    }
}