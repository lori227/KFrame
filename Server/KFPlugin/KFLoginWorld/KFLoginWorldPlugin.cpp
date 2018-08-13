#include "KFLoginWorldPlugin.h"
#include "KFLoginWorldModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLoginWorldPlugin::Install()
    {
        __REGISTER_MODULE__( KFLoginWorld );
    }

    void KFLoginWorldPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLoginWorld );
    }

    void KFLoginWorldPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_world, KFWorldInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}