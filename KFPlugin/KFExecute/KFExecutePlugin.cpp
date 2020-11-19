#include "KFExecutePlugin.hpp"
#include "KFExecuteModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFExecutePlugin::Install()
    {
        __REGISTER_MODULE__( KFExecute );
    }

    void KFExecutePlugin::UnInstall()
    {
        __UN_MODULE__( KFExecute );
    }

    void KFExecutePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_deploy_client, KFDeployClientInterface );
    }

    void KFExecutePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFExecuteConfig );
    }
}