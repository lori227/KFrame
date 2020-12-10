#include "KFTeamClientModule.hpp"

namespace KFrame
{
#define __TEAM_ROUTE_NAME__ __STRING__( team )

    void KFTeamClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA__( &KFTeamClientModule::OnTeamIntValueUpdate );
        __REGISTER_UPDATE_STRING__( &KFTeamClientModule::OnTeamStrValueUpdate );
        ///////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_PLAYER_ENTER__( &KFTeamClientModule::OnEnterTeamModule );
        __REGISTER_PLAYER_LEAVE__( &KFTeamClientModule::OnLeaveTeamModule );
        ///////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::MSG_TEAM_CREATE_REQ, KFMsg::MsgTeamCreateReq, HandleTeamCreateReq );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::MSG_TEAM_LEAVE_REQ, KFMsg::MsgTeamLeaveReq, HandleTeamLeaveReq );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::MSG_TEAM_KICK_REQ, KFMsg::MsgTeamKickReq, HandleTeamKickReq );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::MSG_TEAM_INVITE_REQ, KFMsg::MsgTeamInviteReq, HandleTeamInviteReq );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::MSG_TEAM_APPLY_REQ, KFMsg::MsgTeamApplyReq, HandleTeamApplyReq );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::MSG_TEAM_AGREE_REQ, KFMsg::MsgTeamAgreeReq, HandleTeamAgreeReq );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::S2S_TEAM_JON_TO_GAME_ACK, KFMsg::S2STeamJoinToGameAck, HandleTeamJoinToGameAck );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::S2S_TEAM_ONLINE_QUERY_TO_GAME_ACK, KFMsg::S2STeamOnlineQueryToGameAck, HandleTeamOnlineQueryToGameAck );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::S2S_TEAM_LEAVE_TO_GAME_ACK, KFMsg::S2STeamLeaveToGameAck, HandleTeamLeaveToGameAck );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::S2S_TEAM_TELL_INVITE_TO_GAME_ACK, KFMsg::S2STeamTellInviteToGameAck, HandleTeamTellInviteToGameAck );
        __REGISTER_MESSAGE__( KFTeamClientModule, KFMessageEnum::Player, KFMsg::S2S_TEAM_TELL_APPLY_TO_GAME_ACK, KFMsg::S2STeamTellInviteToGameAck, HandleTeamTellApplyToGameAck );
    }

    void KFTeamClientModule::BeforeShut()
    {
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();

        __UN_UPDATE_DATA__();
        __UN_UPDATE_STRING__();
        ///////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_TEAM_CREATE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TEAM_LEAVE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TEAM_KICK_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TEAM_INVITE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TEAM_APPLY_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TEAM_AGREE_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_JON_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_ONLINE_QUERY_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_LEAVE_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_TELL_INVITE_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_TELL_APPLY_TO_GAME_ACK );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFTeamClientModule::OnEnterTeamModule )
    {
        // 判断队伍是否存在, 存在去队伍集群中请求
        auto teamid = player->Get( __STRING__( team ), __STRING__( id ) );
        if ( teamid == _invalid_int )
        {
            return;
        }

        KFMsg::S2STeamOnlineQueryToTeamReq req;
        req.set_teamid( teamid );
        req.set_playerid( player->GetKeyID() );
        auto ok = _kf_route->RepeatToObject( player->GetKeyID(), __TEAM_ROUTE_NAME__, teamid, KFMsg::S2S_TEAM_ONLINE_QUERY_TO_TEAM_REQ, &req );
        if ( ok )
        {
            // 把队伍id清除
            player->Set( __STRING__( team ), __STRING__( id ), _invalid_int );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFTeamClientModule::OnLeaveTeamModule )
    {
        auto teamid = player->Get( __STRING__( team ), __STRING__( id ) );
        if ( teamid == _invalid_int )
        {
            return;
        }

        EnterLeaveUpdateToTeam( player );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTeamClientModule::OnTeamIntValueUpdate )
    {
        if ( !kfdata->HaveMask( KFDataDefine::DataMaskTeam ) ||
                !kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskTeam ) )
        {
            return;
        }

        auto teamid = player->Get( __STRING__( team ), __STRING__( id ) );
        if ( teamid == _invalid_int )
        {
            return;
        }

        UpdateMemberIntValueToTeam( player, kfdata->_data_setting->_name, newvalue );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFTeamClientModule::OnTeamStrValueUpdate )
    {
        if ( !kfdata->HaveMask( KFDataDefine::DataMaskTeam ) ||
                !kfdata->GetParent()->HaveMask( KFDataDefine::DataMaskTeam ) )
        {
            return;
        }

        auto teamid = player->Get( __STRING__( team ), __STRING__( id ) );
        if ( teamid == _invalid_int )
        {
            return;
        }

        UpdateMemberStrValueToTeam( player, kfdata->_data_setting->_name, newvalue );
    }

    void KFTeamClientModule::UpdateIntValueToTeam( KFEntity* player, const StringUInt64& values )
    {
        auto teamid = player->Get( __STRING__( team ), __STRING__( id ) );

        KFMsg::S2STeamIntValueToTeamReq req;
        req.set_teamid( teamid );
        req.set_playerid( player->GetKeyID() );

        auto pbdata = req.mutable_pbdata();
        for ( auto& iter : values )
        {
            ( *pbdata )[ iter.first ] = iter.second;
        }
        _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, teamid, KFMsg::S2S_TEAM_INT_VALUE_TO_TEAM_REQ, &req );
    }

    void KFTeamClientModule::UpdateStrValueToTeam( KFEntity* player, const StringMap& values )
    {
        auto teamid = player->Get( __STRING__( team ), __STRING__( id ) );

        KFMsg::S2STeamStrValueToTeamReq req;
        req.set_teamid( teamid );
        req.set_playerid( player->GetKeyID() );
        auto pbdata = req.mutable_pbdata();
        for ( auto& iter : values )
        {
            ( *pbdata )[ iter.first ] = iter.second;
        }
        _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, teamid, KFMsg::S2S_TEAM_STR_VALUE_TO_TEAM_REQ, &req );
    }

    void KFTeamClientModule::UpdateMemberIntValueToTeam( KFEntity* player, const std::string& dataname, uint64 datavalue )
    {
        StringUInt64 values;
        values[ dataname ] = datavalue;
        UpdateMemberIntValueToTeam( player, values );
    }

    void KFTeamClientModule::UpdateMemberIntValueToTeam( KFEntity* player, const StringUInt64& values )
    {
        auto teamid = player->Get( __STRING__( team ), __STRING__( id ) );

        KFMsg::S2STeamMemberIntValueToTeamReq req;
        req.set_teamid( teamid );
        req.set_playerid( player->GetKeyID() );

        auto pbdata = req.mutable_pbdata();
        for ( auto& iter : values )
        {
            ( *pbdata )[ iter.first ] = iter.second;
        }
        _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, teamid, KFMsg::S2S_TEAM_MEMBER_INT_VALUE_TO_TEAM_REQ, &req );
    }

    void KFTeamClientModule::UpdateMemberStrValueToTeam( KFEntity* player, const std::string& dataname, const std::string& datavalue )
    {
        auto teamid = player->Get( __STRING__( team ), __STRING__( id ) );

        KFMsg::S2STeamMemberStrValueToTeamReq req;
        req.set_teamid( teamid );
        req.set_playerid( player->GetKeyID() );
        ( *req.mutable_pbdata() )[ dataname ] = datavalue;
        _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, teamid, KFMsg::S2S_TEAM_MEMBER_STR_VALUE_TO_TEAM_REQ, &req );
    }

    void KFTeamClientModule::EnterLeaveUpdateToTeam( KFEntity* player )
    {
        auto kfbasic = player->Find( __STRING__( basic ) );

        StringUInt64 values;
        values[ __STRING__( serverid ) ] = kfbasic->Get( __STRING__( serverid ) );
        values[ __STRING__( status ) ] = kfbasic->Get( __STRING__( status ) );
        values[ __STRING__( statustime ) ] = kfbasic->Get( __STRING__( statustime ) );
        UpdateMemberIntValueToTeam( player, values );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamCreateReq, KFMsg::MsgTeamCreateReq )
    {
        // 判断自己是否有队伍
        auto teamid = kfentity->Get( __STRING__( team ), __STRING__( id ) );
        if ( teamid != _invalid_int )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamAlready );
        }

        // 非法字符判断
        if ( !kfmsg->name().empty() && _kf_filter->CheckFilter( kfmsg->name() ) )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamNameFilter );
        }

        if ( !kfmsg->info().empty() && _kf_filter->CheckFilter( kfmsg->info() ) )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamInfoFilter );
        }

        // 队伍配置
        auto kfsetting = KFTeamConfig::Instance()->FindSetting( kfmsg->id() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamSettingError );
        }

        auto pbbasic = _kf_kernel->SerializeToClient( kfentity->Find( __STRING__( basic ) ) );

        // 发送到队伍集群处理
        KFMsg::S2STeamCreateToTeamReq req;
        req.set_id( kfmsg->id() );
        req.set_name( kfmsg->name() );
        req.set_info( kfmsg->info() );
        req.mutable_pbcaptain()->CopyFrom( *pbbasic );
        auto ok = _kf_route->SendToRand( __TEAM_ROUTE_NAME__, KFMsg::S2S_TEAM_CREATE_TO_TEAM_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( kfentity, KFMsg::RouteServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamJoinToGameAck, KFMsg::S2STeamJoinToGameAck )
    {
        // 先判断是否有队伍
        auto kfteam = kfentity->Find( __STRING__( team ) );
        auto teamid = kfteam->Get( __STRING__( id ) );
        if ( teamid != _invalid_int )
        {
            if ( teamid != kfmsg->id() )
            {
                // 已经在队伍, 并且不是同一个队伍, 需要从队伍中删除玩家
                KFMsg::S2STeamJoinFailedToTeamReq req;
                req.set_teamid( kfmsg->id() );
                req.set_playerid( kfentity->GetKeyID() );
                _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, kfmsg->id(), KFMsg::S2S_TEAM_JION_FAILED_TO_TEAM_REQ, &req );
            }
            return;
        }

        // 保存队伍数据
        _kf_kernel->ParseFromProto( kfteam, &kfmsg->pbteam() );

        // 同步给客户端
        kfentity->SyncUpdateDataToClient( kfteam, kfmsg->id() );

        // 提示
        _kf_display->DelayToClient( kfentity, KFMsg::TeamJoinOk );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamOnlineQueryToGameAck, KFMsg::S2STeamOnlineQueryToGameAck )
    {
        auto kfteam = kfentity->Find( __STRING__( team ) );

        // 保存队伍数据
        _kf_kernel->ParseFromProto( kfteam, &kfmsg->pbteam() );

        // 同步给客户端
        kfentity->SyncUpdateDataToClient( kfteam, kfteam->Get( __STRING__( id ) ) );

        // 更新状态
        EnterLeaveUpdateToTeam( kfentity );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamLeaveReq, KFMsg::MsgTeamLeaveReq )
    {
        auto teamid = kfentity->Get( __STRING__( team ), __STRING__( id ) );
        if ( teamid == _invalid_int )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamNoInTeam );
        }

        KFMsg::S2STeamLeaveToTeamReq req;
        req.set_teamid( teamid );
        req.set_playerid( kfentity->GetKeyID() );
        auto ok = _kf_route->SendToObject( kfentity->GetKeyID(), __TEAM_ROUTE_NAME__, teamid, KFMsg::S2S_TEAM_LEAVE_TO_TEAM_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( kfentity, KFMsg::RouteServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamKickReq, KFMsg::MsgTeamKickReq )
    {
        auto kfteam = kfentity->Find( __STRING__( team ) );
        auto teamid = kfteam->Get( __STRING__( id ) );
        if ( teamid == _invalid_int )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamNoInTeam );
        }

        auto captainid = kfteam->Get( __STRING__( captainid ) );
        if ( captainid != kfentity->GetKeyID() )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamNoInCaptain );
        }

        auto kfmember = kfteam->Find( __STRING__( member ), kfmsg->memberid() );
        if ( kfmember == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamNotHaveMember );
        }

        KFMsg::S2STeamKickToTeamReq req;
        req.set_teamid( teamid );
        req.set_memberid( kfmsg->memberid() );
        req.set_captainid( kfentity->GetKeyID() );
        auto ok = _kf_route->SendToObject( kfentity->GetKeyID(), __TEAM_ROUTE_NAME__, teamid, KFMsg::S2S_TEAM_KICK_TO_TEAM_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( kfentity, KFMsg::RouteServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamLeaveToGameAck, KFMsg::S2STeamLeaveToGameAck )
    {
        auto kfteam = kfentity->Find( __STRING__( team ) );
        auto teamid = kfteam->Get( __STRING__( id ) );
        if ( teamid != kfmsg->teamid() )
        {
            return;
        }

        // 清空队伍信息
        kfentity->ClearRecord( kfteam->Find( __STRING__( member ) ) );
        kfentity->UpdateObject( kfteam, __STRING__( id ), KFEnum::Set, _invalid_int );

        switch ( kfmsg->type() )
        {
        case KFMsg::Leave:
            _kf_display->DelayToClient( kfentity, KFMsg::TeamLeave );
            break;
        case KFMsg::Kick:
            _kf_display->DelayToClient( kfentity, KFMsg::TeamBeKick );
            break;
        case KFMsg::Dissolve:
            _kf_display->DelayToClient( kfentity, KFMsg::TeamDissolve );
            break;
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamInviteReq, KFMsg::MsgTeamInviteReq )
    {
        auto kfteam = kfentity->Find( __STRING__( team ) );
        auto teamid = kfteam->Get( __STRING__( id ) );
        if ( teamid == _invalid_int )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamNoInTeam );
        }

        auto maxcount = kfteam->Get( __STRING__( maxcount ) );
        auto nowcount = kfteam->Get( __STRING__( nowcount ) );
        if ( nowcount >= maxcount )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamIsFull );
        }

        SendTeamInviteToTarget( kfentity, kfteam, kfmsg->serverid(), kfmsg->playerid() );
    }

    void KFTeamClientModule::SendTeamInviteToTarget( KFEntity* player, KFData* kfteam, uint64 serverid, uint64 playerid )
    {
        auto pbteam = _kf_kernel->SerializeToView( kfteam );
        auto pbbasic = _kf_kernel->SerializeToView( player->Find( __STRING__( basic ) ) );

        // 发送到目标玩家
        KFMsg::S2STeamTellInviteToGameAck tell;
        tell.mutable_pbteam()->CopyFrom( *pbteam );
        tell.mutable_pbplayer()->CopyFrom( *pbbasic );
        auto ok = _kf_route->SendToEntity( player->GetKeyID(), serverid, playerid, KFMsg::S2S_TEAM_TELL_INVITE_TO_GAME_ACK, &tell );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RouteServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamTellInviteToGameAck, KFMsg::S2STeamTellInviteToGameAck )
    {
        auto playerid = __ROUTE_RECV_ID__;
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamNotOnline );
        }

        // 判断状态
        auto status = player->GetStatus();
        if ( status == KFMsg::PlayingStatus )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamPlaying );
        }

        // 通知邀请
        KFMsg::MsgTeamTellInvite tell;
        tell.mutable_pbteam()->CopyFrom( kfmsg->pbteam() );
        tell.mutable_pbplayer()->CopyFrom( kfmsg->pbplayer() );
        _kf_player->SendToClient( player, KFMsg::MSG_TEAM_TELL_INVITE, &tell );

        // 通知发送邀请成功
        _kf_display->SendToPlayer( route, KFMsg::TeamInviteOk );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamApplyReq, KFMsg::MsgTeamApplyReq )
    {
        auto status = kfentity->GetStatus();
        if ( status == KFMsg::PlayingStatus )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamPlaying );
        }

        auto pbbasic = _kf_kernel->SerializeToView( kfentity->Find( __STRING__( basic ) ) );

        KFMsg::S2STeamTellApplyToGameAck tell;
        tell.mutable_pbplayer()->CopyFrom( *pbbasic );
        auto ok = _kf_route->SendToEntity( kfentity->GetKeyID(), kfmsg->serverid(), kfmsg->playerid(), KFMsg::S2S_TEAM_TELL_APPLY_TO_GAME_ACK, &tell );
        if ( !ok )
        {
            _kf_display->SendToClient( kfentity, KFMsg::RouteServerBusy );
        }
    }

    // 客户端收到申请, 直接再发一次邀请即可
    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamTellApplyToGameAck, KFMsg::S2STeamTellInviteToGameAck )
    {
        auto playerid = __ROUTE_RECV_ID__;
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamNotOnline );
        }

        auto kfteam = player->Find( __STRING__( team ) );
        auto teamid = kfteam->Get( __STRING__( id ) );
        if ( teamid == _invalid_int )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamNoInTeam );
        }

        auto maxcount = kfteam->Get( __STRING__( maxcount ) );
        auto nowcount = kfteam->Get( __STRING__( nowcount ) );
        if ( nowcount >= maxcount )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamIsFull );
        }

        // 判断状态
        auto status = player->GetStatus();
        if ( status == KFMsg::PlayingStatus )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamPlaying );
        }

        KFMsg::MsgTeamTellApply tell;
        tell.mutable_pbplayer()->CopyFrom( kfmsg->pbplayer() );
        _kf_player->SendToClient( player, KFMsg::MSG_TEAM_TELL_APPLY, &tell );

        // 通知申请玩家
        _kf_display->SendToPlayer( route, KFMsg::TeamApplyOk );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamAgreeReq, KFMsg::MsgTeamAgreeReq )
    {
        if ( kfmsg->teamid() == _invalid_int )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamIdError );
        }

        auto kfteam = kfentity->Find( __STRING__( team ) );
        auto teamid = kfteam->Get( __STRING__( id ) );
        if ( teamid != _invalid_int )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::TeamAlready );
        }

        auto pbbasic = _kf_kernel->SerializeToView( kfentity->Find( __STRING__( basic ) ) );

        KFMsg::S2STeamAgreeToTeamReq req;
        req.set_teamid( kfmsg->teamid() );
        req.mutable_pbplayer()->CopyFrom( *pbbasic );
        auto ok = _kf_route->SendToObject( kfentity->GetKeyID(), __TEAM_ROUTE_NAME__, kfmsg->teamid(), KFMsg::S2S_TEAM_AGREE_TO_TEAM_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( kfentity, KFMsg::RouteServerBusy );
        }
    }
}
