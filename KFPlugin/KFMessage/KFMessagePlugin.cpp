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
        __UN_MODULE__( KFMessage );
    }

    void KFMessagePlugin::LoadModule()
    {

    }
}