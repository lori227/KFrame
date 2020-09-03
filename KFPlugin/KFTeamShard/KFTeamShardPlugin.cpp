#include "KFTeamShardPlugin.hpp"
#include "KFTeamShardModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    void KFTeamShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFTeamShard );
    }

    void KFTeamShardPlugin::UnInstall()
    {
        __UN_MODULE__( KFTeamShard );
    }

    void KFTeamShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }

    void KFTeamShardPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFTeamConfig );
    }
}