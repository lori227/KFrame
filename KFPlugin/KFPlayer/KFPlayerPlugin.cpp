#include "KFPlayerPlugin.hpp"
#include "KFPlayerModule.hpp"

namespace KFrame
{
    void KFPlayerPlugin::Install()
    {
        __REGISTER_MODULE__( KFPlayer );
    }

    void KFPlayerPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFPlayer );
    }

    void KFPlayerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_command, KFCommandInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }

}