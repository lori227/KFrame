#include "KFClusterMasterPlugin.hpp"
#include "KFClusterMasterModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterMasterPlugin::Install()
    {
        __REGISTER_MODULE__( KFClusterMaster );
    }

    void KFClusterMasterPlugin::UnInstall()
    {
        __UN_MODULE__( KFClusterMaster );
    }

    void KFClusterMasterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
    }

    void KFClusterMasterPlugin::AddConfig()
    {

    }
}