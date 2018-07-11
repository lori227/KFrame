#include "KFClusterServerPlugin.h"
#include "KFClusterServerModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterServerPlugin::Install()
    {
        __REGISTER_MODULE__( KFClusterServer );
    }

    void KFClusterServerPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFClusterServer );
    }

    void KFClusterServerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }
}