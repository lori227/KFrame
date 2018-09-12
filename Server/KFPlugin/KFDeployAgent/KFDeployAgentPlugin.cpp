#include "KFDeployAgentPlugin.h"
#include "KFDeployAgentModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDeployAgentPlugin::Install()
    {
        __REGISTER_MODULE__( KFDeployAgent );
    }

    void KFDeployAgentPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDeployAgent );
    }

    void KFDeployAgentPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_ftp, KFFtpInterface );
        __FIND_MODULE__( _kf_mysql, KFMySQLInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_schedule, KFScheduleInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }
}