#include "KFOptionPlugin.hpp"
#include "KFOptionModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFOptionPlugin::Install()
    {
        __REGISTER_MODULE__( KFOption );
    }

    void KFOptionPlugin::UnInstall()
    {
        __UN_MODULE__( KFOption );
    }

    void KFOptionPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }

    void KFOptionPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFOptionConfig );
    }

}