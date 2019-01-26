#include "KFBattleClientPlugin.h"
#include "KFBattleClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFBattleClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFBattleClient );
    }

    void KFBattleClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFBattleClient );
    }

    void KFBattleClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
        __FIND_MODULE__( _kf_relation, KFRelationClientInterface );
    }
}