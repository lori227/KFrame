#include "KFLoginWorldPlugin.h"
#include "KFLoginWorldModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLoginWorldPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFLoginWorldPlugin, KFLoginWorldInterface >( new KFLoginWorldModule() );
    }

    void KFLoginWorldPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFLoginWorldPlugin, KFLoginWorldInterface >();
    }

    void KFLoginWorldPlugin::LoadModule()
    {
        _kf_world = _kf_plugin_manage->FindModule< KFWorldInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
    }
}