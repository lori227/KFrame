#include "KFDataClientPlugin.h"
#include "KFDataClientModule.h"

namespace KFrame
{
    void KFDataClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFDataClient );
        _kf_plugin_manage->RegistModule< KFDataClientPlugin, KFDataClientInterface >( new KFDataClientModule() );
    }

    void KFDataClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDataClient );
        _kf_plugin_manage->UnRegistModule< KFDataClientPlugin, KFDataClientInterface >();
    }

    void KFDataClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
    }

}