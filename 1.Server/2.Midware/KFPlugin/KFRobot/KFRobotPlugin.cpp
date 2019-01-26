#include "KFRobotPlugin.h"
#include "KFRobotModule.h"

namespace KFrame
{
    void KFRobotPlugin::Install()
    {
        __REGISTER_MODULE__( KFRobot );

    }

    void KFRobotPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRobot );
    }

    void KFRobotPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }
}