#include "KFCompoundPlugin.hpp"
#include "KFCompoundModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFCompoundPlugin::Install()
    {
        __REGISTER_MODULE__( KFCompound );
    }

    void KFCompoundPlugin::UnInstall()
    {
        __UN_MODULE__( KFCompound );
    }

    void KFCompoundPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
    }
}