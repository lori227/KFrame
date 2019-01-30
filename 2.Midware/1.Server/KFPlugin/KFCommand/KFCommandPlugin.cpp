#include "KFCommandPlugin.hpp"
#include "KFCommandModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFCommandPlugin::Install()
    {
        __REGISTER_MODULE__( KFCommand );
    }

    void KFCommandPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFCommand );
    }

    void KFCommandPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
    }
}