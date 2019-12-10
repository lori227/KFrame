#include "KFSignInPlugin.hpp"
#include "KFSignInModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFSignInPlugin::Install()
    {
        __REGISTER_MODULE__( KFSignIn );
    }

    void KFSignInPlugin::UnInstall()
    {
        __UN_MODULE__( KFSignIn );
    }

    void KFSignInPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_game, KFGameInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
    }

    void KFSignInPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFSignInConfig );
    }
}