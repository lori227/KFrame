#include "KFTcpServerPlugin.hpp"
#include "KFTcpServerModule.hpp"
//////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    void KFTcpServerPlugin::Install()
    {
        __REGISTER_MODULE__( KFTcpServer );
    }

    void KFTcpServerPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFTcpServer );
    }

    void KFTcpServerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
    }
}