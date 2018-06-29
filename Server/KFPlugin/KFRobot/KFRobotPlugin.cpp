#include "KFRobotPlugin.h"
#include "KFRobotModule.h"

namespace KFrame
{
    void KFRobotPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFRobotPlugin, KFRobotInterface >( new KFRobotModule() );
    }

    void KFRobotPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFRobotPlugin, KFRobotInterface >();
    }

    void KFRobotPlugin::LoadModule()
    {
        _kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
    }


}