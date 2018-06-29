#include "KFMessagePlugin.h"
#include "KFMessageModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMessagePlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFMessagePlugin, KFMessageInterface >( new KFMessageModule() );
    }

    void KFMessagePlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFMessagePlugin, KFMessageInterface >();
    }

    void KFMessagePlugin::LoadModule()
    {

    }
}