#include "KFCommandPlugin.h"
#include "KFCommandModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFCommandPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFCommandPlugin, KFCommandInterface >( new KFCommandModule() );
    }

    void KFCommandPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFCommandPlugin, KFCommandInterface >();
    }

    void KFCommandPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
        _kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
    }
}