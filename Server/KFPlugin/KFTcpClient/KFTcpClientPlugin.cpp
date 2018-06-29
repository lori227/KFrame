#include "KFTcpClientPlugin.h"
#include "KFTcpClientModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTcpClientPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFTcpClientPlugin, KFTcpClientInterface >( new KFTcpClientModule() );
    }

    void KFTcpClientPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFTcpClientPlugin, KFTcpClientInterface >();
    }

    void KFTcpClientPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_connection = _kf_plugin_manage->FindModule< KFConnectionInterface >();
    }
}