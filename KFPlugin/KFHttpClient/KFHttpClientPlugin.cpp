#include "KFHttpClientPlugin.hpp"
#include "KFHttpClientModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFHttpClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFHttpClient );


    }

    void KFHttpClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFHttpClient );
    }

    void KFHttpClientPlugin::LoadModule()
    {

    }
}