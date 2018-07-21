#include "KFLoginGatePlugin.h"
#include "KFLoginGateModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLoginGatePlugin::Install()
    {
        __REGISTER_MODULE__( KFLoginGate );
    }

    void KFLoginGatePlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLoginGate );
    }

    void KFLoginGatePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_zone, KFZoneInterface );
        __FIND_MODULE__( _kf_gate, KFGateInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}