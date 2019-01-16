#include "KFLuaPlugin.h"
#include "KFLuaModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLuaPlugin::Install()
    {
        __REGISTER_MODULE__( KFLua );
    }

    void KFLuaPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLua );
    }

    void KFLuaPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_command, KFDeployCommandInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }
}