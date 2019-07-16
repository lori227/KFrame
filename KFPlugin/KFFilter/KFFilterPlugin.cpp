#include "KFFilterPlugin.hpp"
#include "KFFilterModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFFilterPlugin::Install()
    {
        __REGISTER_MODULE__( KFFilter );
    }

    void KFFilterPlugin::UnInstall()
    {
        __UN_MODULE__( KFFilter );
    }

    void KFFilterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }
}