#include "KFOptionPlugin.hpp"
#include "KFOptionModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFOptionPlugin::Install()
    {
        __REGISTER_MODULE__( KFOption );
    }

    void KFOptionPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFOption );
    }

    void KFOptionPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }
}