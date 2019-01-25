#include "KFTcpServerPlugin.h"
#include "KFTcpServerModule.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"

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
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }
}