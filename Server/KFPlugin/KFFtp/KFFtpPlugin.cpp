#include "KFFtpPlugin.h"
#include "KFFtpModule.h"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFFtpPlugin::Install()
    {
        __REGISTER_MODULE__( KFFtp );
    }

    void KFFtpPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFFtp );
    }

    void KFFtpPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}