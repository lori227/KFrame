#include "KFDirClientModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFDirClientModule::BeforeRun()
    {
        __REGISTER_LOOP_TIMER__( 0, 5000, &KFDirClientModule::OnTimerUpdateOnlineToDir );
    }

    void KFDirClientModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFDirClientModule::OnTimerUpdateOnlineToDir )
    {
        auto kfzone = _kf_zone->GetZone();
        auto kfglobal = KFGlobal::Instance();

        KFMsg::S2SUpdateOnlineToDirReq req;
        req.set_zoneid( kfzone->_id );
        req.set_zonename( kfzone->_name );
        req.set_appid( kfglobal->_app_id._union._id );
        req.set_zonechannel( kfglobal->_app_id._union._app_data._channel_id );
        req.set_ip( kfglobal->_interanet_ip );
        req.set_port( kfglobal->_listen_port );
        req.set_onlinecount( _kf_gate->GetRoleCount() );
        _kf_route->SendToAll( __KF_STRING__( dir ), KFMsg::S2S_UPDATE_ONLINE_TO_DIR_REQ, &req );
    }
}