#include "KFDirClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFDirClientModule::KFDirClientModule()
    {
    }

    KFDirClientModule::~KFDirClientModule()
    {
    }

    void KFDirClientModule::BeforeRun()
    {
        _kf_cluster->RegisterConnectionFunction( __KF_STRING__( dir ), this, &KFDirClientModule::OnConnectionDirCluster );
    }

    void KFDirClientModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDirClientModule::OnConnectionDirCluster( uint32 serverid )
    {
        __REGISTER_LOOP_TIMER__( 0, 2000, &KFDirClientModule::OnTimerUpdateOnlineToDir );
    }

    __KF_TIMER_FUNCTION__( KFDirClientModule::OnTimerUpdateOnlineToDir )
    {
        auto kfzone = _kf_zone->GetZone();
        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SUpdateOnlineToDirReq req;
        req.set_zoneid( kfzone->_id );
        req.set_zonename( kfzone->_name );
        req.set_zonechannel( kfglobal->_app_channel );
        req.set_appid( kfglobal->_app_id );
        req.set_ip( kfglobal->_interanet_ip );
        req.set_port( kfglobal->_listen_port );
        req.set_onlinecount( _kf_gate->GetRoleCount() );
        _kf_cluster->SendMessageToShard( __KF_STRING__( dir ), KFMsg::S2S_UPDATE_ONLINE_TO_DIR_REQ, &req );
    }
}