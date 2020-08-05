#include "KFTeamClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFTeamClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA__( &KFTeamClientModule::OnDataUpdateCallBack );
        ///////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_PLAYER_ENTER__( &KFTeamClientModule::OnEnterTeamModule );
        __REGISTER_PLAYER_LEAVE__( &KFTeamClientModule::OnLeaveTeamModule );
        ///////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_TEAM_CREATE_REQ, &KFTeamClientModule::HandleTeamCreateReq );
    }

    void KFTeamClientModule::BeforeShut()
    {
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();
        ///////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_TEAM_CREATE_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFTeamClientModule::OnEnterTeamModule )
    {

    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFTeamClientModule::OnLeaveTeamModule )
    {

    }

    __KF_UPDATE_DATA_FUNCTION__( KFTeamClientModule::OnDataUpdateCallBack )
    {

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamCreateReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTeamCreateReq );


    }

}
