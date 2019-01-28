#include "KFMessagePlugin.hpp"
#include "KFMessageModule.hpp"
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