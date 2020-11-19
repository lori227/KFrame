#include "KFPayClientPlugin.hpp"
#include "KFPayClientModule.hpp"
#include "KFLoader/KFLoaderInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFPayClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFPayClient );
    }

    void KFPayClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFPayClient );
    }

    void KFPayClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }

    void KFPayClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFPayConfig );
    }
}