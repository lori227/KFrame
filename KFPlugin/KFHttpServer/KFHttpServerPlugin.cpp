#include "KFHttpServerPlugin.hpp"
#include "KFHttpServerModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFHttpServerPlugin::Install()
    {
        __REGISTER_MODULE__( KFHttpServer );
    }

    void KFHttpServerPlugin::UnInstall()
    {
        __UN_MODULE__( KFHttpServer );
    }

    void KFHttpServerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
    }

    void KFHttpServerPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFHttpServerConfig );
    }
}