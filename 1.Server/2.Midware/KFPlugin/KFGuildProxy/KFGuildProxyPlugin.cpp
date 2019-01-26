#include "KFGuildProxyPlugin.h"
#include "KFGuildProxyModule.h"

namespace KFrame
{
    void KFGuildProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFGuildProxy );
    }

    void KFGuildProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFGuildProxy );
    }

    void KFGuildProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_cluster_proxy, KFClusterProxyInterface );
    }

}