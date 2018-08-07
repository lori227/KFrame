#include "KFZonePlugin.h"
#include "KFZoneModule.h"

namespace KFrame
{
    void KFZonePlugin::Install()
    {
        __REGISTER_MODULE__( KFZone );
    }

    void KFZonePlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFZone );
    }

    void KFZonePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }

}