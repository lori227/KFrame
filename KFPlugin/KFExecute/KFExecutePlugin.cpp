#include "KFExecutePlugin.hpp"
#include "KFExecuteModule.hpp"
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
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
    }

    void KFExecutePlugin::AddConfig()
    {

    }
}