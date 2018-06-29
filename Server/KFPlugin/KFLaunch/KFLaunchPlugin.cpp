#include "KFLaunchPlugin.h"
#include "KFLaunchModule.h"
#include "KFFtp/KFFtpInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFCommand/KFCommandInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLaunchPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFLaunchPlugin, KFLaunchInterface >( new KFLaunchModule() );
    }

    void KFLaunchPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFLaunchPlugin, KFLaunchInterface >();
    }

    void KFLaunchPlugin::LoadModule()
    {
        _kf_ftp = _kf_plugin_manage->FindModule< KFFtpInterface >();
        _kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_command = _kf_plugin_manage->FindModule< KFCommandInterface >();
        _kf_schedule = _kf_plugin_manage->FindModule< KFScheduleInterface >();
        _kf_http_server = _kf_plugin_manage->FindModule< KFHttpServerInterface >();
    }
}