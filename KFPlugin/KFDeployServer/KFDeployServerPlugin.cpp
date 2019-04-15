#include "KFDeployServerPlugin.hpp"
#include "KFDeployServerModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDeployServerPlugin::Install()
    {
        __REGISTER_MODULE__( KFDeployServer );
    }

    void KFDeployServerPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDeployServer );
    }

    void KFDeployServerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_mysql, KFMySQLInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_schedule, KFScheduleInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }
}