#include "KFHttpClientPlugin.h"
#include "KFHttpClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFHttpClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFHttpClient );
    }

    void KFHttpClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFHttpClient );
    }

    void KFHttpClientPlugin::LoadModule()
    {

    }
}