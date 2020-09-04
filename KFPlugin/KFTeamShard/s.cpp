#include "KFTeamShardModule.hpp"

namespace KFrame
{
    void KFTeamShardModule::BeforeRun()
    {
        _kf_route->RegisterConnectionFunction( this, &KFTeamShardModule::OnRouteConnectCluster );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_component = _kf_kernel->FindComponent( __STRING__( team ) );
        _kf_component->RegisterEntityInitializeFunction( this, &KFTeamShardModule::InitTeam );
        _kf_component->RegisterEntityUninitializeFunction( this, &KFTeamShardModule::UnInitTeam );

        // 注册更新函数
        _kf_component->RegisterSyncAddFunction( this, &KFTeamShardModule::SendTeamAddDataToMember );
        _kf_component->RegisterSyncRemoveFunction( this, &KFTeamShardModule::SendTeamRemoveDataToMember );
        _kf_component->RegisterSyncUpdateFunction( this, &KFTeamShardModule::SendTeamUpdateDataToMember );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_CREATE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamCreateToTeamReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_JION_FAILED_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamJoinFailedToTeamReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_ONLINE_QUERY_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamOnlineQueryToTeamReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_INT_VALUE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamIntValueToTeamReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_STR_VALUE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamStrValueToTeamReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_MEMBER_INT_VALUE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamMemberIntValueToTeamReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_MEMBER_STR_VALUE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamMemberStrValueToTeamReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_LEAVE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamLeaveToTeamReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_KICK_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamKickToTeamReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TEAM_AGREE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamAgreeToTeamReq );
    }

    void KFTeamShardModule::BeforeShut()
    {
        _kf_route->UnRegisterConnectionFunction( this );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_component->UnRegisterEntityInitializeFunction();
        _kf_component->UnRegisterEntityUninitializeFunction();

        _kf_component->UnRegisterSyncAddFunction();
        _kf_component->UnRegisterSyncRemoveFunction();
        _kf_component->UnRegisterSyncUpdateFunction();

        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_TEAM_CREATE_TO_TEAM_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_JION_FAILED_TO_TEAM_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_ONLINE_QUERY_TO_TEAM_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_INT_VALUE_TO_TEAM_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_STR_VALUE_TO_TEAM_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_MEMBER_INT_VALUE_TO_TEAM_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_MEMBER_STR_VALUE_TO_TEAM_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_LEAVE_TO_TEAM_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_KICK_TO_TEAM_REQ );
        __UN_MESSAGE__( KFMsg::S2S_TEAM_AGREE_TO_TEAM_REQ );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTeamShardModule::OnRouteConnectCluster( uint64 serverid )
    {
        RouteObjectList teamlist;
        for ( auto kfteam = _kf_component->FirstEntity(); kfteam != nullptr; kfteam = _kf_component->NextEntity() )
        {
            teamlist.insert( kfteam->GetKeyID() );
        }
        _kf_route->SyncObject( teamlist );
    }

    void KFTeamShardModule::SendMessageToTeam( KFEntity* kfteam, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto kfmemberrecord = kfteam->Find( __STRING__( member ) );
        for ( auto kfmember = kfmemberrecord->First(); kfmember != nullptr; kfmember = kfmemberrecord->Next() )
        {
            SendMessageToMember( kfmember, msgid, message );
        }
    }

    void KFTeamShardModule::SendMessageToMember( KFData* kfmember, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto serverid = kfmember->Get<uint64>( __STRING__( basic ), __STRING__( serverid ) );
        _kf_route->SendToPlayer( serverid, kfmember->GetKeyID(), msgid, message );
    }

    void KFTeamShardModule::InitTeam( KFEntity* team )
    {
        auto teamuuid = team->GetKeyID();
        _kf_route->AddObject( teamuuid );
    }

    void KFTeamShardModule::UnInitTeam( KFEntity* team )
    {
        auto teamuuid = team->GetKeyID();
        _kf_route->RemoveObject( teamuuid );
    }

    void KFTeamShardModule::SendTeamUpdateDataToMember( KFEntity* kfteam, KFMsg::PBObject& pbobject )
    {
        KFMsg::S2SSyncUpdateDataFromServer sync;
        sync.set_dataname( __STRING__( team ) );
        sync.mutable_pbdata()->Swap( &pbobject );
        SendMessageToTeam( kfteam, KFMsg::S2S_SYNC_UPDATE_DATA_FROM_SERVER, &sync );
    }

    void KFTeamShardModule::SendTeamAddDataToMember( KFEntity* kfteam, KFMsg::PBObject& pbobject )
    {
        KFMsg::S2SSyncAddDataFromServer sync;
        sync.set_dataname( __STRING__( team ) );
        sync.mutable_pbdata()->Swap( &pbobject );
        SendMessageToTeam( kfteam, KFMsg::S2S_SYNC_ADD_DATA_FROM_SERVER, &sync );
    }

    void KFTeamShardModule::SendTeamRemoveDataToMember( KFEntity* kfteam, KFMsg::PBObject& pbobject )
    {
        KFMsg::S2SSyncRemoveDataFromServer sync;
        sync.set_dataname( __STRING__( team ) );
        sync.mutable_pbdata()->Swap( &pbobject );
        SendMessageToTeam( kfteam, KFMsg::S2S_SYNC_REMOVE_DATA_FROM_SERVER, &sync );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamCreateToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamCreateToTeamReq );

        auto kfsetting = KFTeamConfig::Instance()->FindSetting( kfmsg.id() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamSettingError );
        }

        // 直接创建队伍, 如果玩家有队伍, 在加入队伍时game上处理失败情况
        auto teamuuid = KFGlobal::Instance()->STMakeUuid( __STRING__( team ) );
        auto kfteam = _kf_component->CreateEntity( teamuuid );

        kfteam->Set( __STRING__( configid ), kfmsg.id() );
        kfteam->Set( __STRING__( name ), kfmsg.name() );
        kfteam->Set( __STRING__( info ), kfmsg.info() );
        kfteam->Set( __STRING__( maxcount ), kfsetting->_max_count );

        auto kfmemberrecord = kfteam->Find( __STRING__( member ) );
        auto kfmember = kfteam->CreateData( kfmemberrecord );
        _kf_kernel->ParseFromProto( kfmember, &kfmsg.pbcaptain() );

        // 设置队长
        SetTeamCaptain( kfteam, kfmember, false );

        // 添加队员
        kfmemberrecord->Add( kfmember->GetKeyID(), kfmember );

        // 通知队员加入队伍
        SendJoinTeamToMember( kfteam, kfmember );
    }

    void KFTeamShardModule::ChangeTeamCaptain( KFEntity* kfteam, uint64 captainid )
    {
        auto kfmemberrecord = kfteam->Find( __STRING__( member ) );
        for ( auto kfmember = kfmemberrecord->First(); kfmember != nullptr; kfmember = kfmemberrecord->Next() )
        {
            if ( kfmember->GetKeyID() == captainid )
            {
                continue;
            }

            return SetTeamCaptain( kfteam, kfmember, true );
        }
    }

    void KFTeamShardModule::SetTeamCaptain( KFEntity* kfteam, KFData* kfmember, bool update )
    {
        if ( !update )
        {
            kfteam->Set( __STRING__( captainid ), kfmember->GetKeyID() );
            kfteam->Set( __STRING__( captainname ), kfmember->Get<std::string>( __STRING__( name ) ) );
        }
        else
        {
            kfteam->UpdateData( __STRING__( captainid ), KFEnum::Set, kfmember->GetKeyID() );
            kfteam->UpdateData( __STRING__( captainname ), kfmember->Get<std::string>( __STRING__( name ) ) );
        }
    }

    void KFTeamShardModule::SendJoinTeamToMember( KFEntity* kfteam, KFData* kfmember )
    {
        auto pbteam = _kf_kernel->SerializeToClient( kfteam );

        KFMsg::S2STeamJoinToGameAck ack;
        ack.mutable_pbteam()->CopyFrom( *pbteam );
        SendMessageToMember( kfmember, KFMsg::S2S_TEAM_JON_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamJoinFailedToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamJoinFailedToTeamReq );

        auto kfteam = _kf_component->FindEntity( kfmsg.teamid() );
        if ( kfteam == nullptr )
        {
            return;
        }

        RemoveTeamMember( kfteam, kfmsg.playerid() );
    }

    void KFTeamShardModule::RemoveTeamMember( KFEntity* kfteam, uint64 memberid )
    {
        // 删除队员
        auto kfmemberrecord = kfteam->Find( __STRING__( member ) );
        kfteam->RemoveData( kfmemberrecord, memberid );
        if ( kfmemberrecord->Size() == 0u )
        {
            // 已经没有队员了, 删除队伍
            _kf_component->RemoveEntity( kfteam );
        }
        else
        {
            // 更新队伍当前数量
            kfteam->UpdateData( __STRING__( nowcount ), KFEnum::Set, kfmemberrecord->Size() );

			auto captainid = kfteam->Get<uint64>( __STRING__( captainid ) );
			if ( captainid == memberid )
			{
				// 更新队长
				ChangeTeamCaptain( ( KFEntity* )kfteam, captainid );
			}
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamOnlineQueryToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamOnlineQueryToTeamReq );
        auto kfteam = _kf_component->FindEntity( kfmsg.teamid() );
        if ( kfteam == nullptr )
        {
            return;
        }

        auto kfmember = kfteam->Find( __STRING__( member ), kfmsg.playerid() );
        if ( kfmember == nullptr )
        {
            return;
        }

        // 如果在队伍, 把队伍信息返回给客户端
        auto pbteam = _kf_kernel->SerializeToClient( kfteam );

        KFMsg::S2STeamOnlineQueryToGameAck ack;
        ack.mutable_pbteam()->CopyFrom( *pbteam );
        _kf_route->RepeatToRoute( route, KFMsg::S2S_TEAM_ONLINE_QUERY_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamIntValueToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamIntValueToTeamReq );

        auto kfteam = _kf_component->FindEntity( kfmsg.teamid() );
        if ( kfteam == nullptr )
        {
            return;
        }

        auto captainid = kfteam->Get( __STRING__( captainid ) );
        if ( captainid != kfmsg.playerid() )
        {
            return;
        }

        for ( auto iter = kfmsg.pbdata().begin(); iter != kfmsg.pbdata().end(); ++iter )
        {
            kfteam->UpdateData( iter->first, KFEnum::Set, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamStrValueToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamStrValueToTeamReq );

        auto kfteam = _kf_component->FindEntity( kfmsg.teamid() );
        if ( kfteam == nullptr )
        {
            return;
        }

        auto captainid = kfteam->Get( __STRING__( captainid ) );
        if ( captainid != kfmsg.playerid() )
        {
            return;
        }

        for ( auto iter = kfmsg.pbdata().begin(); iter != kfmsg.pbdata().end(); ++iter )
        {
            kfteam->UpdateData( iter->first, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamMemberIntValueToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamMemberIntValueToTeamReq );

        auto kfteam = _kf_component->FindEntity( kfmsg.teamid() );
        if ( kfteam == nullptr )
        {
            return;
        }

        auto kfmember = kfteam->Find( __STRING__( member ), kfmsg.playerid() );
        if ( kfmember == nullptr )
        {
            return;
        }

        for ( auto iter = kfmsg.pbdata().begin(); iter != kfmsg.pbdata().end(); ++iter )
        {
            kfteam->UpdateData( kfmember, iter->first, KFEnum::Set, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamMemberStrValueToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamMemberStrValueToTeamReq );

        auto kfteam = _kf_component->FindEntity( kfmsg.teamid() );
        if ( kfteam == nullptr )
        {
            return;
        }

        auto kfmember = kfteam->Find( __STRING__( member ), kfmsg.playerid() );
        if ( kfmember == nullptr )
        {
            return;
        }

        for ( auto iter = kfmsg.pbdata().begin(); iter != kfmsg.pbdata().end(); ++iter )
        {
            kfteam->UpdateData( kfmember, iter->first, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamLeaveToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamLeaveToTeamReq );

        auto kfteam = _kf_component->FindEntity( kfmsg.teamid() );
        if ( kfteam == nullptr )
        {
            return;
        }

        // 通知离开队伍
        KFMsg::S2STeamLeaveToGameAck ack;
        ack.set_type( KFMsg::Leave );
        ack.set_teamid( kfmsg.teamid() );
        _kf_route->RepeatToRoute( route, KFMsg::S2S_TEAM_LEAVE_TO_GAME_ACK, &ack );

        // 删除玩家
        RemoveTeamMember( kfteam, kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamKickToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamKickToTeamReq );

        auto kfteam = _kf_component->FindEntity( kfmsg.teamid() );
        if ( kfteam == nullptr )
        {
            return;
        }

        auto captainid = kfteam->Get( __STRING__( captainid ) );
        if ( captainid != kfmsg.captainid() )
        {
            return;
        }

        auto kfmember = kfteam->Find( __STRING__( member ), kfmsg.memberid() );
        if ( kfmember == nullptr )
        {
            return;
        }

        {
            // 通知离开队伍
            KFMsg::S2STeamLeaveToGameAck ack;
            ack.set_type( KFMsg::Kick );
            ack.set_teamid( kfmsg.teamid() );
            SendMessageToMember( kfmember, KFMsg::S2S_TEAM_LEAVE_TO_GAME_ACK, &ack );
        }

        // 删除玩家
        RemoveTeamMember( kfteam, kfmsg.memberid() );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamAgreeToTeamReq )
    {
        __PROTO_PARSE__( KFMsg::S2STeamAgreeToTeamReq );

        auto kfteam = _kf_component->FindEntity( kfmsg.teamid() );
        if ( kfteam == nullptr )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamNotExist );
        }

        auto maxcount = kfteam->Get( __STRING__( maxcount ) );
        auto nowcount = kfteam->Get( __STRING__( nowcount ) );
        if ( nowcount >= maxcount )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamIsFull );
        }

        auto kfmemberrecord = kfteam->Find( __STRING__( member ) );
        auto kfmember = kfteam->CreateData( kfmemberrecord );
        _kf_kernel->ParseFromProto( kfmember, &kfmsg.pbplayer() );

        // 加入队伍中
        kfteam->AddData( kfmemberrecord, kfmember->GetKeyID(), kfmember );

		// 更新数量
        kfteam->UpdateData( __STRING__( nowcount ), KFEnum::Set, kfmemberrecord->Size() );

        // 通知队员加入队伍
        SendJoinTeamToMember( kfteam, kfmember );
    }
}