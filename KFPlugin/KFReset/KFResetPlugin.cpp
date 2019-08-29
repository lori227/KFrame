#include "KFResetPlugin.hpp"
#include "KFResetModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFResetPlugin::Install()
    {
        __REGISTER_MODULE__( KFReset );
    }

    void KFResetPlugin::UnInstall()
    {
        __UN_MODULE__( KFReset );
    }

    void KFResetPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
    }

    void KFResetPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFResetConfig );
    }

}