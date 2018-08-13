#include "KFGroupShardPlugin.h"
#include "KFGroupShardModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFGroupShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFGroupShard );
    }

    void KFGroupShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFGroupShard );
    }

    void KFGroupShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}