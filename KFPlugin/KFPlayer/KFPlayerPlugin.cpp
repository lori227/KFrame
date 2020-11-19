#include "KFPlayerPlugin.hpp"
#include "KFPlayerModule.hpp"
#include "KFLoader/KFLoaderInterface.h"

namespace KFrame
{
    void KFPlayerPlugin::Install()
    {
        __REGISTER_MODULE__( KFPlayer );
    }

    void KFPlayerPlugin::UnInstall()
    {
        __UN_MODULE__( KFPlayer );
    }

    void KFPlayerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_command, KFCommandInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }

    void KFPlayerPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFPlayerConfig );
    }
}