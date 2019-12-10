#include "KFLuaPlugin.hpp"
#include "KFLuaModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLuaPlugin::Install()
    {
        __REGISTER_MODULE__( KFLua );
    }

    void KFLuaPlugin::UnInstall()
    {
        __UN_MODULE__( KFLua );
    }

    void KFLuaPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_deploy_client, KFDeployClientInterface );
    }
}