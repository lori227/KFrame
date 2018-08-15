#include "KFLoginLoginPlugin.h"
#include "KFLoginLoginModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLoginLoginPlugin::Install()
    {
        __REGISTER_MODULE__( KFLoginLogin );
    }

    void KFLoginLoginPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLoginLogin );
    }

    void KFLoginLoginPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_zone, KFZoneInterface );
        __FIND_MODULE__( _kf_login, KFLoginInterface );
        __FIND_MODULE__( _kf_data, KFDataClientInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_deploy_command, KFDeployCommandInterface );
    }
}