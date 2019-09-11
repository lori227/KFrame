#include "KFDropPlugin.hpp"
#include "KFDropModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDropPlugin::Install()
    {
        __REGISTER_MODULE__( KFDrop );
    }

    void KFDropPlugin::UnInstall()
    {
        __UN_MODULE__( KFDrop );
    }

    void KFDropPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
    }

    void KFDropPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFElementConfig );
        __KF_ADD_CONFIG__( KFDropDataConfig );
        __KF_ADD_CONFIG__( KFDropGroupConfig );
    }
}