#include "KFChatPlugin.hpp"
#include "KFChatModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFChatPlugin::Install()
    {
        __REGISTER_MODULE__( KFChat );
    }

    void KFChatPlugin::UnInstall()
    {
        __UN_MODULE__( KFChat );
    }

    void KFChatPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_game, KFGameInterface );
        __FIND_MODULE__( _kf_filter, KFFilterInterface );
    }

    void KFChatPlugin::AddConfig()
    {
    }
}