#include "KFLogProxyPlugin.h"
#include "KFLogProxyModule.h"

namespace KFrame
{
    void KFLogProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFLogProxy );
    }

    void KFLogProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLogProxy );
    }

    void KFLogProxyPlugin::LoadModule()
    {
    }
}