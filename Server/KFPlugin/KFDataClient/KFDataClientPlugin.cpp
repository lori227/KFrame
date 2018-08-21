#include "KFDataClientPlugin.h"
#include "KFDataClientModule.h"

namespace KFrame
{
    void KFDataClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFDataClient );
    }

    void KFDataClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDataClient );
    }

    void KFDataClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
    }

}