#include "KFResetPlugin.hpp"
#include "KFResetModule.hpp"

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
}