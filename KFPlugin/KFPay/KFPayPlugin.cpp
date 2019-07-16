#include "KFPayPlugin.hpp"
#include "KFPayModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFPayPlugin::Install()
    {
        __REGISTER_MODULE__( KFPay );
    }

    void KFPayPlugin::UnInstall()
    {
        __UN_MODULE__( KFPay );
    }

    void KFPayPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }
}