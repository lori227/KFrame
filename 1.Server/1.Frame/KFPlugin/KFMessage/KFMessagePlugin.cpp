#include "KFMessagePlugin.h"
#include "KFMessageModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMessagePlugin::Install()
    {
        __REGISTER_MODULE__( KFMessage );
    }

    void KFMessagePlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFMessage );
    }

    void KFMessagePlugin::LoadModule()
    {

    }
}