#include "KFBattleMasterPlugin.h"
#include "KFBattleMasterModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFBattleMasterPlugin::Install()
    {
        __REGISTER_MODULE__( KFBattleMaster );
    }

    void KFBattleMasterPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFBattleMaster );
    }

    void KFBattleMasterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }
}