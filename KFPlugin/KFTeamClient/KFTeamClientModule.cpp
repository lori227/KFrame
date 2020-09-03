#include "KFTeamClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __TEAM_ROUTE_NAME__ __STRING__( team )

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
        // 判断队伍是否存在, 存在去队伍集群中请求
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

        // 判断自己是否有队伍
        auto teamid = player->Get( __STRING__( team ), __STRING__( id ) );
        if ( teamid != _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::TeamAlready );
        }

        // 非法字符判断
        if ( !kfmsg.name().empty() && _kf_filter->CheckFilter( kfmsg.name() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::TeamNameFilter );
        }

        if ( !kfmsg.info().empty() && _kf_filter->CheckFilter( kfmsg.info() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::TeamInfoFilter );
        }

        // 队伍配置
        auto kfsetting = KFTeamConfig::Instance()->FindSetting( kfmsg.id() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::TeamSettingError );
        }

        auto pbbasic = _kf_kernel->SerializeToClient( player->Find( __STRING__( basic ) ) );

        // 发送到队伍集群处理
        KFMsg::S2STeamCreateToTeamReq req;
        req.set_id( kfmsg.id() );
        req.set_name( kfmsg.name() );
        req.set_info( kfmsg.info() );
        req.mutable_pbcaptain()->CopyFrom( *pbbasic );
        auto ok = _kf_route->SendToRand( __TEAM_ROUTE_NAME__, KFMsg::S2S_TEAM_CREATE_TO_TEAM_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RouteServerBusy );
        }
    }
}
