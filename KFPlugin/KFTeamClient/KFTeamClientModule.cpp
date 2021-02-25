#include "KFTeamClientModule.hpp"

namespace KFrame
{
#define __TEAM_ROUTE_NAME__ __STRING__( team )

    void KFTeamClientModule::BeforeRun()
    {
        _component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA__( &KFTeamClientModule::OnTeamIntValueUpdate );
        __REGISTER_UPDATE_STRING__( &KFTeamClientModule::OnTeamStrValueUpdate );
        ///////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_PLAYER_ENTER__( &KFTeamClientModule::OnEnterTeamModule );
        __REGISTER_PLAYER_LEAVE__( &KFTeamClientModule::OnLeaveTeamModule );
        ///////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_TEAM_CREATE_REQ, &KFTeamClientModule::HandleTeamCreateReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_TEAM_LEAVE_REQ, &KFTeamClientModule::HandleTeamLeaveReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_TEAM_KICK_REQ, &KFTeamClientModule::HandleTeamKickReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_TEAM_INVITE_REQ, &KFTeamClientModule::HandleTeamInviteReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_TEAM_APPLY_REQ, &KFTeamClientModule::HandleTeamApplyReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_TEAM_AGREE_REQ, &KFTeamClientModule::HandleTeamAgreeReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_TEAM_JON_TO_GAME_ACK, &KFTeamClientModule::HandleTeamJoinToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_TEAM_ONLINE_QUERY_TO_GAME_ACK, &KFTeamClientModule::HandleTeamOnlineQueryToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_TEAM_LEAVE_TO_GAME_ACK, &KFTeamClientModule::HandleTeamLeaveToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_TEAM_TELL_INVITE_TO_GAME_ACK, &KFTeamClientModule::HandleTeamTellInviteToGameAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_TEAM_TELL_APPLY_TO_GAME_ACK, &KFTeamClientModule::HandleTeamTellApplyToGameAck );
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
        auto team_id = player->Get( __STRING__( team ), __STRING__( id ) );
        if ( team_id == _invalid_int )
        {
            return;
        }

        KFMsg::S2STeamOnlineQueryToTeamReq req;
        req.set_teamid( team_id );
        req.set_playerid( player->GetKeyID() );
        auto ok = _kf_route->RepeatToObject( player->GetKeyID(), __TEAM_ROUTE_NAME__, team_id, KFMsg::S2S_TEAM_ONLINE_QUERY_TO_TEAM_REQ, &req );
        if ( ok )
        {
            // 把队伍id清除
            player->Operate( __STRING__( team ), __STRING__( id ), KFEnum::Set, _invalid_int );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFTeamClientModule::OnLeaveTeamModule )
    {
        auto team_id = player->Get( __STRING__( team ), __STRING__( id ) );
        if ( team_id == _invalid_int )
        {
            return;
        }

        EnterLeaveUpdateToTeam( player );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTeamClientModule::OnTeamIntValueUpdate )
    {
        if ( !data->HaveMask( KFDataDefine::DataMaskTeam ) ||
                !data->GetParent()->HaveMask( KFDataDefine::DataMaskTeam ) )
        {
            return;
        }

        auto team_id = player->Get( __STRING__( team ), __STRING__( id ) );
        if ( team_id == _invalid_int )
        {
            return;
        }

        UpdateMemberIntValueToTeam( player, data->_data_setting->_name, new_value );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFTeamClientModule::OnTeamStrValueUpdate )
    {
        if ( !data->HaveMask( KFDataDefine::DataMaskTeam ) ||
                !data->GetParent()->HaveMask( KFDataDefine::DataMaskTeam ) )
        {
            return;
        }

        auto team_id = player->Get( __STRING__( team ), __STRING__( id ) );
        if ( team_id == _invalid_int )
        {
            return;
        }

        UpdateMemberStrValueToTeam( player, data->_data_setting->_name, new_value );
    }

    void KFTeamClientModule::UpdateIntValueToTeam( EntityPtr player, const StringUInt64& values )
    {
        auto team_id = player->Get( __STRING__( team ), __STRING__( id ) );

        KFMsg::S2STeamIntValueToTeamReq req;
        req.set_teamid( team_id );
        req.set_playerid( player->GetKeyID() );

        auto& pb_data = *req.mutable_pbdata();
        __MAP_TO_PROTO__( values, pb_data );
        _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, team_id, KFMsg::S2S_TEAM_INT_VALUE_TO_TEAM_REQ, &req );
    }

    void KFTeamClientModule::UpdateStrValueToTeam( EntityPtr player, const StringMap& values )
    {
        auto team_id = player->Get( __STRING__( team ), __STRING__( id ) );

        KFMsg::S2STeamStrValueToTeamReq req;
        req.set_teamid( team_id );
        req.set_playerid( player->GetKeyID() );
        auto& pb_data = *req.mutable_pbdata();
        __MAP_TO_PROTO__( values, pb_data );
        _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, team_id, KFMsg::S2S_TEAM_STR_VALUE_TO_TEAM_REQ, &req );
    }

    void KFTeamClientModule::UpdateMemberIntValueToTeam( EntityPtr player, const std::string& data_name, uint64 data_value )
    {
        StringUInt64 values;
        values[ data_name ] = data_value;
        UpdateMemberIntValueToTeam( player, values );
    }

    void KFTeamClientModule::UpdateMemberIntValueToTeam( EntityPtr player, const StringUInt64& values )
    {
        auto team_id = player->Get( __STRING__( team ), __STRING__( id ) );

        KFMsg::S2STeamMemberIntValueToTeamReq req;
        req.set_teamid( team_id );
        req.set_playerid( player->GetKeyID() );

        auto& pb_data = *req.mutable_pbdata();
        __MAP_TO_PROTO__( values, pb_data );
        _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, team_id, KFMsg::S2S_TEAM_MEMBER_INT_VALUE_TO_TEAM_REQ, &req );
    }

    void KFTeamClientModule::UpdateMemberStrValueToTeam( EntityPtr player, const std::string& data_name, const std::string& data_value )
    {
        auto team_id = player->Get( __STRING__( team ), __STRING__( id ) );

        KFMsg::S2STeamMemberStrValueToTeamReq req;
        req.set_teamid( team_id );
        req.set_playerid( player->GetKeyID() );
        ( *req.mutable_pbdata() )[ data_name ] = data_value;
        _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, team_id, KFMsg::S2S_TEAM_MEMBER_STR_VALUE_TO_TEAM_REQ, &req );
    }

    void KFTeamClientModule::EnterLeaveUpdateToTeam( EntityPtr player )
    {
        auto basic_data = player->Find( __STRING__( basic ) );

        StringUInt64 values;
        values[ __STRING__( serverid ) ] = basic_data->Get( __STRING__( serverid ) );
        values[ __STRING__( status ) ] = basic_data->Get( __STRING__( status ) );
        values[ __STRING__( statustime ) ] = basic_data->Get( __STRING__( statustime ) );
        UpdateMemberIntValueToTeam( player, values );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamCreateReq, KFMsg::MsgTeamCreateReq )
    {
        // 判断自己是否有队伍
        auto team_id = entity->Get( __STRING__( team ), __STRING__( id ) );
        if ( team_id != _invalid_int )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamAlready );
        }

        // 非法字符判断
        if ( !kfmsg->name().empty() && _kf_filter->CheckFilter( kfmsg->name() ) )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamNameFilter );
        }

        if ( !kfmsg->info().empty() && _kf_filter->CheckFilter( kfmsg->info() ) )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamInfoFilter );
        }

        // 队伍配置
        auto setting = KFTeamConfig::Instance()->FindSetting( kfmsg->id() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamSettingError );
        }

        auto pb_basic = _kf_kernel->SerializeToClient( entity->Find( __STRING__( basic ) ) );

        // 发送到队伍集群处理
        KFMsg::S2STeamCreateToTeamReq req;
        req.set_id( kfmsg->id() );
        req.set_name( kfmsg->name() );
        req.set_info( kfmsg->info() );
        req.mutable_pbcaptain()->CopyFrom( *pb_basic );
        auto ok = _kf_route->SendToRand( __TEAM_ROUTE_NAME__, KFMsg::S2S_TEAM_CREATE_TO_TEAM_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( entity, KFMsg::RouteServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamJoinToGameAck, KFMsg::S2STeamJoinToGameAck )
    {
        // 先判断是否有队伍
        auto team_data = entity->Find( __STRING__( team ) );
        auto team_id = team_data->Get( __STRING__( id ) );
        if ( team_id != _invalid_int )
        {
            if ( team_id != kfmsg->id() )
            {
                // 已经在队伍, 并且不是同一个队伍, 需要从队伍中删除玩家
                KFMsg::S2STeamJoinFailedToTeamReq req;
                req.set_teamid( kfmsg->id() );
                req.set_playerid( entity->GetKeyID() );
                _kf_route->RepeatToObject( __TEAM_ROUTE_NAME__, kfmsg->id(), KFMsg::S2S_TEAM_JION_FAILED_TO_TEAM_REQ, &req );
            }
            return;
        }

        // 保存队伍数据
        _kf_kernel->ParseFromMessage( team_data, &kfmsg->pbteam() );

        // 同步给客户端
        entity->SyncUpdateDataToClient( team_data, kfmsg->id() );

        // 提示
        _kf_display->DelayToClient( entity, KFMsg::TeamJoinOk );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamOnlineQueryToGameAck, KFMsg::S2STeamOnlineQueryToGameAck )
    {
        auto team_data = entity->Find( __STRING__( team ) );

        // 保存队伍数据
        _kf_kernel->ParseFromMessage( team_data, &kfmsg->pbteam() );

        // 同步给客户端
        entity->SyncUpdateDataToClient( team_data, team_data->Get( __STRING__( id ) ) );

        // 更新状态
        EnterLeaveUpdateToTeam( entity );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamLeaveReq, KFMsg::MsgTeamLeaveReq )
    {
        auto team_id = entity->Get( __STRING__( team ), __STRING__( id ) );
        if ( team_id == _invalid_int )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamNoInTeam );
        }

        KFMsg::S2STeamLeaveToTeamReq req;
        req.set_teamid( team_id );
        req.set_playerid( entity->GetKeyID() );
        auto ok = _kf_route->SendToObject( entity->GetKeyID(), __TEAM_ROUTE_NAME__, team_id, KFMsg::S2S_TEAM_LEAVE_TO_TEAM_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( entity, KFMsg::RouteServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamKickReq, KFMsg::MsgTeamKickReq )
    {
        auto team_data = entity->Find( __STRING__( team ) );
        auto team_id = team_data->Get( __STRING__( id ) );
        if ( team_id == _invalid_int )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamNoInTeam );
        }

        auto captain_id = team_data->Get( __STRING__( captainid ) );
        if ( captain_id != entity->GetKeyID() )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamNoInCaptain );
        }

        auto member_data = team_data->Find( __STRING__( member ), kfmsg->memberid() );
        if ( member_data == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamNotHaveMember );
        }

        KFMsg::S2STeamKickToTeamReq req;
        req.set_teamid( team_id );
        req.set_memberid( kfmsg->memberid() );
        req.set_captainid( entity->GetKeyID() );
        auto ok = _kf_route->SendToObject( entity->GetKeyID(), __TEAM_ROUTE_NAME__, team_id, KFMsg::S2S_TEAM_KICK_TO_TEAM_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( entity, KFMsg::RouteServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamLeaveToGameAck, KFMsg::S2STeamLeaveToGameAck )
    {
        auto team_data = entity->Find( __STRING__( team ) );
        auto team_id = team_data->Get( __STRING__( id ) );
        if ( team_id != kfmsg->teamid() )
        {
            return;
        }

        // 清空队伍信息
        entity->ClearRecord( team_data->Find( __STRING__( member ) ) );
        entity->UpdateObject( team_data, __STRING__( id ), KFEnum::Set, _invalid_int );

        switch ( kfmsg->type() )
        {
        case KFMsg::Leave:
            _kf_display->DelayToClient( entity, KFMsg::TeamLeave );
            break;
        case KFMsg::Kick:
            _kf_display->DelayToClient( entity, KFMsg::TeamBeKick );
            break;
        case KFMsg::Dissolve:
            _kf_display->DelayToClient( entity, KFMsg::TeamDissolve );
            break;
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamInviteReq, KFMsg::MsgTeamInviteReq )
    {
        auto team_data = entity->Find( __STRING__( team ) );
        auto team_id = team_data->Get( __STRING__( id ) );
        if ( team_id == _invalid_int )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamNoInTeam );
        }

        auto max_count = team_data->Get( __STRING__( maxcount ) );
        auto now_count = team_data->Get( __STRING__( nowcount ) );
        if ( now_count >= max_count )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamIsFull );
        }

        SendTeamInviteToTarget( entity, team_data, kfmsg->serverid(), kfmsg->playerid() );
    }

    void KFTeamClientModule::SendTeamInviteToTarget( EntityPtr player, DataPtr team_data, uint64 server_id, uint64 player_id )
    {
        auto pb_team = _kf_kernel->SerializeToView( team_data );
        auto pb_basic = _kf_kernel->SerializeToView( player->Find( __STRING__( basic ) ) );

        // 发送到目标玩家
        KFMsg::S2STeamTellInviteToGameAck tell;
        tell.mutable_pbteam()->CopyFrom( *pb_team );
        tell.mutable_pbplayer()->CopyFrom( *pb_basic );
        auto ok = _kf_route->SendToEntity( player->GetKeyID(), server_id, player_id, KFMsg::S2S_TEAM_TELL_INVITE_TO_GAME_ACK, &tell );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RouteServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamTellInviteToGameAck, KFMsg::S2STeamTellInviteToGameAck )
    {
        // 判断状态
        auto status = entity->GetStatus();
        if ( status == KFMsg::PlayingStatus )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamPlaying );
        }

        // 通知邀请
        KFMsg::MsgTeamTellInvite tell;
        tell.mutable_pbteam()->CopyFrom( kfmsg->pbteam() );
        tell.mutable_pbplayer()->CopyFrom( kfmsg->pbplayer() );
        _kf_player->SendToClient( entity, KFMsg::MSG_TEAM_TELL_INVITE, &tell );

        // 通知发送邀请成功
        _kf_display->SendToPlayer( route, KFMsg::TeamInviteOk );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamApplyReq, KFMsg::MsgTeamApplyReq )
    {
        auto status = entity->GetStatus();
        if ( status == KFMsg::PlayingStatus )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamPlaying );
        }

        auto pb_basic = _kf_kernel->SerializeToView( entity->Find( __STRING__( basic ) ) );

        KFMsg::S2STeamTellApplyToGameAck tell;
        tell.mutable_pbplayer()->CopyFrom( *pb_basic );
        auto ok = _kf_route->SendToEntity( entity->GetKeyID(), kfmsg->serverid(), kfmsg->playerid(), KFMsg::S2S_TEAM_TELL_APPLY_TO_GAME_ACK, &tell );
        if ( !ok )
        {
            _kf_display->SendToClient( entity, KFMsg::RouteServerBusy );
        }
    }

    // 客户端收到申请, 直接再发一次邀请即可
    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamTellApplyToGameAck, KFMsg::S2STeamTellInviteToGameAck )
    {
        auto team_data = entity->Find( __STRING__( team ) );
        auto team_id = team_data->Get( __STRING__( id ) );
        if ( team_id == _invalid_int )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamNoInTeam );
        }

        auto max_count = team_data->Get( __STRING__( maxcount ) );
        auto now_count = team_data->Get( __STRING__( nowcount ) );
        if ( now_count >= max_count )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamIsFull );
        }

        // 判断状态
        auto status = entity->GetStatus();
        if ( status == KFMsg::PlayingStatus )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamPlaying );
        }

        KFMsg::MsgTeamTellApply tell;
        tell.mutable_pbplayer()->CopyFrom( kfmsg->pbplayer() );
        _kf_player->SendToClient( entity, KFMsg::MSG_TEAM_TELL_APPLY, &tell );

        // 通知申请玩家
        _kf_display->SendToPlayer( route, KFMsg::TeamApplyOk );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamClientModule::HandleTeamAgreeReq, KFMsg::MsgTeamAgreeReq )
    {
        if ( kfmsg->teamid() == _invalid_int )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamIdError );
        }

        auto team_data = entity->Find( __STRING__( team ) );
        auto team_id = team_data->Get( __STRING__( id ) );
        if ( team_id != _invalid_int )
        {
            return _kf_display->SendToClient( entity, KFMsg::TeamAlready );
        }

        auto pb_basic = _kf_kernel->SerializeToView( entity->Find( __STRING__( basic ) ) );

        KFMsg::S2STeamAgreeToTeamReq req;
        req.set_teamid( kfmsg->teamid() );
        req.mutable_pbplayer()->CopyFrom( *pb_basic );
        auto ok = _kf_route->SendToObject( entity->GetKeyID(), __TEAM_ROUTE_NAME__, kfmsg->teamid(), KFMsg::S2S_TEAM_AGREE_TO_TEAM_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( entity, KFMsg::RouteServerBusy );
        }
    }
}
