#include "KFLogClientPlugin.hpp"
#include "KFLogClientModule.hpp"

namespace KFrame
{
    void KFLogClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFLogClient );
    }

    void KFLogClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFLogClient );
    }

    void KFLogClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }

}