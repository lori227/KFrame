#include "KFGroupProxyPlugin.h"
#include "KFGroupProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFGroupProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFGroupProxy );
    }

    void KFGroupProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFGroupProxy );
    }

    void KFGroupProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_cluster_proxy, KFClusterProxyInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}