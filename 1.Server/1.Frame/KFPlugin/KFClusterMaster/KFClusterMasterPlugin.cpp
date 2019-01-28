#include "KFClusterMasterPlugin.hpp"
#include "KFClusterMasterModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterMasterPlugin::Install()
    {
        __REGISTER_MODULE__( KFClusterMaster );
    }

    void KFClusterMasterPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFClusterMaster );
    }

    void KFClusterMasterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }
}