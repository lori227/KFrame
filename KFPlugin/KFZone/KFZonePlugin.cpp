#include "KFZonePlugin.hpp"
#include "KFZoneModule.hpp"

namespace KFrame
{
    void KFZonePlugin::Install()
    {
        __REGISTER_MODULE__( KFZone );
    }

    void KFZonePlugin::UnInstall()
    {
        __UN_MODULE__( KFZone );
    }

    void KFZonePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
    }

}