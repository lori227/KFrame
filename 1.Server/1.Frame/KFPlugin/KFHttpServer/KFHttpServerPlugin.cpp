#include "KFHttpServerPlugin.h"
#include "KFHttpServerModule.h"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFHttpServerPlugin::Install()
    {
        __REGISTER_MODULE__( KFHttpServer );
    }

    void KFHttpServerPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFHttpServer );
    }

    void KFHttpServerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
    }
}