#include "KFTcpDiscoverPlugin.hpp"
#include "KFTcpDiscoverModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTcpDiscoverPlugin::Install()
    {
        __REGISTER_MODULE__( KFTcpDiscover );
    }

    void KFTcpDiscoverPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFTcpDiscover );
    }

    void KFTcpDiscoverPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
    }
}