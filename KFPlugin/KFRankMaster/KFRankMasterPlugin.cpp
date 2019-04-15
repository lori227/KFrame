#include "KFRankMasterPlugin.hpp"
#include "KFRankMasterModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRankMasterPlugin::Install()
    {
        __REGISTER_MODULE__( KFRankMaster );
    }

    void KFRankMasterPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRankMaster );
    }

    void KFRankMasterPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_schedule, KFScheduleInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }
}