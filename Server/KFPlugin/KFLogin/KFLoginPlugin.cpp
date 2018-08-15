#include "KFLoginPlugin.h"
#include "KFLoginModule.h"

namespace KFrame
{
    void KFLoginPlugin::Install()
    {
        __REGISTER_MODULE__( KFLogin );
    }

    void KFLoginPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLogin );
    }

    void KFLoginPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }

}

