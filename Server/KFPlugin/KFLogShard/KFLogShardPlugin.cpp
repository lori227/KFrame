#include "KFLogShardPlugin.h"
#include "KFLogShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{

    void KFLogShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFLogShard );
    }

    void KFLogShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLogShard );
    }

    void KFLogShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
    }

}