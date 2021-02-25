#include "KFTeamShardModule.hpp"

namespace KFrame
{
    void KFTeamShardModule::BeforeRun()
    {
        _kf_route->RegisterConnectionFunction( this, &KFTeamShardModule::OnRouteConnectCluster );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        _component = _kf_kernel->FindComponent( __STRING__( team ) );
        // 注册更新函数
        _component->RegisterSyncAddFunction( this, &KFTeamShardModule::SendTeamAddDataToMember );
        _component->RegisterSyncRemoveFunction( this, &KFTeamShardModule::SendTeamRemoveDataToMember );
        _component->RegisterSyncUpdateFunction( this, &KFTeamShardModule::SendTeamUpdateDataToMember );

        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_CREATE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamCreateToTeamReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_JION_FAILED_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamJoinFailedToTeamReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_ONLINE_QUERY_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamOnlineQueryToTeamReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_INT_VALUE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamIntValueToTeamReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_STR_VALUE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamStrValueToTeamReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_MEMBER_INT_VALUE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamMemberIntValueToTeamReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_MEMBER_STR_VALUE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamMemberStrValueToTeamReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_LEAVE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamLeaveToTeamReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_KICK_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamKickToTeamReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_TEAM_AGREE_TO_TEAM_REQ, &KFTeamShardModule::HandleTeamAgreeToTeamReq );
    }

    void KFTeamShardModule::BeforeShut()
    {
        _kf_route->UnRegisterConnectionFunction( this );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        _component->UnRegisterSyncAddFunction();
        _component->UnRegisterSyncRemoveFunction();
        _component->UnRegisterSyncUpdateFunction();
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
    void KFTeamShardModule::OnRouteConnectCluster( uint64 server_id )
    {
        UInt64Set team_list;
        for ( auto team_data = _component->FirstEntity(); team_data != nullptr; team_data = _component->NextEntity() )
        {
            team_list.insert( team_data->GetKeyID() );
        }
        _kf_route->SyncObject( team_list );
    }

    void KFTeamShardModule::SendMessageToTeam( EntityPtr team_data, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto member_record = team_data->Find( __STRING__( member ) );
        for ( auto member_data = member_record->First(); member_data != nullptr; member_data = member_record->Next() )
        {
            SendMessageToMember( member_data, msg_id, message );
        }
    }

    void KFTeamShardModule::SendMessageToMember( DataPtr member_data, uint32 msg_id, ::google::protobuf::Message* message )
    {
        auto server_id = member_data->Get<uint64>( __STRING__( basic ), __STRING__( serverid ) );
        _kf_route->SendToEntity( server_id, member_data->GetKeyID(), msg_id, message );
    }

    void KFTeamShardModule::SendTeamUpdateDataToMember( EntityPtr team_data, KFMsg::PBObject& proto_object )
    {
        KFMsg::S2SSyncUpdateDataFromServer sync;
        sync.set_data_name( __STRING__( team ) );
        sync.mutable_pbdata()->Swap( &proto_object );
        SendMessageToTeam( team_data, KFMsg::S2S_SYNC_UPDATE_DATA_FROM_SERVER, &sync );
    }

    void KFTeamShardModule::SendTeamAddDataToMember( EntityPtr team_data, KFMsg::PBObject& proto_object )
    {
        KFMsg::S2SSyncAddDataFromServer sync;
        sync.set_data_name( __STRING__( team ) );
        sync.mutable_pbdata()->Swap( &proto_object );
        SendMessageToTeam( team_data, KFMsg::S2S_SYNC_ADD_DATA_FROM_SERVER, &sync );
    }

    void KFTeamShardModule::SendTeamRemoveDataToMember( EntityPtr team_data, KFMsg::PBObject& proto_object )
    {
        KFMsg::S2SSyncRemoveDataFromServer sync;
        sync.set_data_name( __STRING__( team ) );
        sync.mutable_pbdata()->Swap( &proto_object );
        SendMessageToTeam( team_data, KFMsg::S2S_SYNC_REMOVE_DATA_FROM_SERVER, &sync );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamCreateToTeamReq, KFMsg::S2STeamCreateToTeamReq )
    {
        auto setting = KFTeamConfig::Instance()->FindSetting( kfmsg->id() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamSettingError );
        }

        // 直接创建队伍, 如果玩家有队伍, 在加入队伍时game上处理失败情况
        auto team_uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( team ) );
        auto team_data = _component->CreateEntity( team_uuid );

        team_data->Operate( __STRING__( configid ), KFEnum::Set, kfmsg->id() );
        team_data->Operate( __STRING__( name ), KFEnum::Set, kfmsg->name() );
        team_data->Operate( __STRING__( info ), KFEnum::Set, kfmsg->info() );
        team_data->Operate( __STRING__( maxcount ), KFEnum::Set, setting->_max_count );

        auto member_record = team_data->Find( __STRING__( member ) );
        auto member_data = team_data->CreateData( member_record );
        _kf_kernel->ParseFromMessage( member_data, &kfmsg->pbcaptain() );

        // 设置队长
        SetTeamCaptain( team_data, member_data, false );

        // 添加队员
        member_record->Add( member_data->GetKeyID(), member_data );

        // 通知队员加入队伍
        SendJoinTeamToMember( team_data, member_data );

        // 通知转发集群
        _kf_route->AddObject( team_uuid );
    }

    void KFTeamShardModule::ChangeTeamCaptain( EntityPtr team_data, uint64 captain_id )
    {
        auto member_record = team_data->Find( __STRING__( member ) );
        for ( auto member_data = member_record->First(); member_data != nullptr; member_data = member_record->Next() )
        {
            if ( member_data->GetKeyID() == captain_id )
            {
                continue;
            }

            return SetTeamCaptain( team_data, member_data, true );
        }
    }

    void KFTeamShardModule::SetTeamCaptain( EntityPtr team_data, DataPtr member_data, bool update )
    {
        if ( !update )
        {
            team_data->Operate( __STRING__( captainid ), KFEnum::Set, member_data->GetKeyID() );
            team_data->Operate( __STRING__( captainname ), KFEnum::Set, member_data->Get<std::string>( __STRING__( name ) ) );
        }
        else
        {
            team_data->UpdateData( __STRING__( captainid ), KFEnum::Set, member_data->GetKeyID() );
            team_data->UpdateData( __STRING__( captainname ), member_data->Get<std::string>( __STRING__( name ) ) );
        }
    }

    void KFTeamShardModule::SendJoinTeamToMember( EntityPtr team_data, DataPtr member_data )
    {
        auto pb_team = _kf_kernel->SerializeToClient( team_data );

        KFMsg::S2STeamJoinToGameAck ack;
        ack.mutable_pbteam()->CopyFrom( *pb_team );
        SendMessageToMember( member_data, KFMsg::S2S_TEAM_JON_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamJoinFailedToTeamReq, KFMsg::S2STeamJoinFailedToTeamReq )
    {
        auto team_data = _component->FindEntity( kfmsg->teamid() );
        if ( team_data == nullptr )
        {
            return;
        }

        RemoveTeamMember( team_data, kfmsg->playerid() );
    }

    void KFTeamShardModule::RemoveTeamMember( EntityPtr team_data, uint64 member_id )
    {
        // 删除队员
        auto member_record = team_data->Find( __STRING__( member ) );
        team_data->RemoveRecord( member_record, member_id );
        if ( member_record->Size() == 0u )
        {
            // 已经没有队员了, 删除队伍
            _component->RemoveEntity( team_data );
            _kf_route->RemoveObject( team_data->GetKeyID() );
        }
        else
        {
            // 更新队伍当前数量
            team_data->UpdateData( __STRING__( nowcount ), KFEnum::Set, member_record->Size() );

            // 更新队长
            auto captain_id = team_data->Get<uint64>( __STRING__( captainid ) );
            if ( captain_id == member_id )
            {
                ChangeTeamCaptain( team_data, captain_id );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamOnlineQueryToTeamReq, KFMsg::S2STeamOnlineQueryToTeamReq )
    {
        auto team_data = _component->FindEntity( kfmsg->teamid() );
        if ( team_data == nullptr )
        {
            return;
        }

        auto member_data = team_data->Find( __STRING__( member ), kfmsg->playerid() );
        if ( member_data == nullptr )
        {
            return;
        }

        // 如果在队伍, 把队伍信息返回给客户端
        auto pb_team = _kf_kernel->SerializeToClient( team_data );

        KFMsg::S2STeamOnlineQueryToGameAck ack;
        ack.mutable_pbteam()->CopyFrom( *pb_team );
        _kf_route->RepeatToRoute( route, KFMsg::S2S_TEAM_ONLINE_QUERY_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamIntValueToTeamReq, KFMsg::S2STeamIntValueToTeamReq )
    {
        auto team_data = _component->FindEntity( kfmsg->teamid() );
        if ( team_data == nullptr )
        {
            return;
        }

        auto captain_id = team_data->Get( __STRING__( captainid ) );
        if ( captain_id != kfmsg->playerid() )
        {
            return;
        }

        for ( auto iter = kfmsg->pbdata().begin(); iter != kfmsg->pbdata().end(); ++iter )
        {
            team_data->UpdateData( iter->first, KFEnum::Set, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamStrValueToTeamReq, KFMsg::S2STeamStrValueToTeamReq )
    {
        auto team_data = _component->FindEntity( kfmsg->teamid() );
        if ( team_data == nullptr )
        {
            return;
        }

        auto captain_id = team_data->Get( __STRING__( captainid ) );
        if ( captain_id != kfmsg->playerid() )
        {
            return;
        }

        for ( auto iter = kfmsg->pbdata().begin(); iter != kfmsg->pbdata().end(); ++iter )
        {
            team_data->UpdateData( iter->first, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamMemberIntValueToTeamReq, KFMsg::S2STeamMemberIntValueToTeamReq )
    {
        auto team_data = _component->FindEntity( kfmsg->teamid() );
        if ( team_data == nullptr )
        {
            return;
        }

        auto member_data = team_data->Find( __STRING__( member ), kfmsg->playerid() );
        if ( member_data == nullptr )
        {
            return;
        }

        for ( auto iter = kfmsg->pbdata().begin(); iter != kfmsg->pbdata().end(); ++iter )
        {
            team_data->UpdateObject( member_data, iter->first, KFEnum::Set, iter->second );

            // 如果是队长下线, 需要更换队长
            if ( iter->first == __STRING__( serverid ) && iter->second == _invalid_int )
            {
                auto captain_id = team_data->Get( __STRING__( captainid ) );
                if ( captain_id == member_data->GetKeyID() )
                {
                    ChangeTeamCaptain( team_data, captain_id );
                }
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamMemberStrValueToTeamReq, KFMsg::S2STeamMemberStrValueToTeamReq )
    {
        auto team_data = _component->FindEntity( kfmsg->teamid() );
        if ( team_data == nullptr )
        {
            return;
        }

        auto member_data = team_data->Find( __STRING__( member ), kfmsg->playerid() );
        if ( member_data == nullptr )
        {
            return;
        }

        for ( auto iter = kfmsg->pbdata().begin(); iter != kfmsg->pbdata().end(); ++iter )
        {
            team_data->UpdateObject( member_data, iter->first, iter->second );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamLeaveToTeamReq, KFMsg::S2STeamLeaveToTeamReq )
    {
        auto team_data = _component->FindEntity( kfmsg->teamid() );
        if ( team_data == nullptr )
        {
            return;
        }

        // 通知离开队伍
        KFMsg::S2STeamLeaveToGameAck ack;
        ack.set_type( KFMsg::Leave );
        ack.set_teamid( kfmsg->teamid() );
        _kf_route->RepeatToRoute( route, KFMsg::S2S_TEAM_LEAVE_TO_GAME_ACK, &ack );

        // 删除玩家
        RemoveTeamMember( team_data, kfmsg->playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamKickToTeamReq, KFMsg::S2STeamKickToTeamReq )
    {
        auto team_data = _component->FindEntity( kfmsg->teamid() );
        if ( team_data == nullptr )
        {
            return;
        }

        auto captain_id = team_data->Get( __STRING__( captainid ) );
        if ( captain_id != kfmsg->captainid() )
        {
            return;
        }

        auto member_data = team_data->Find( __STRING__( member ), kfmsg->memberid() );
        if ( member_data == nullptr )
        {
            return;
        }

        {
            // 通知离开队伍
            KFMsg::S2STeamLeaveToGameAck ack;
            ack.set_type( KFMsg::Kick );
            ack.set_teamid( kfmsg->teamid() );
            SendMessageToMember( member_data, KFMsg::S2S_TEAM_LEAVE_TO_GAME_ACK, &ack );
        }

        // 删除玩家
        RemoveTeamMember( team_data, kfmsg->memberid() );
    }

    __KF_MESSAGE_FUNCTION__( KFTeamShardModule::HandleTeamAgreeToTeamReq, KFMsg::S2STeamAgreeToTeamReq )
    {
        auto team_data = _component->FindEntity( kfmsg->teamid() );
        if ( team_data == nullptr )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamNotExist );
        }

        auto max_count = team_data->Get( __STRING__( maxcount ) );
        auto now_count = team_data->Get( __STRING__( nowcount ) );
        if ( now_count >= max_count )
        {
            return _kf_display->SendToPlayer( route, KFMsg::TeamIsFull );
        }

        auto member_record = team_data->Find( __STRING__( member ) );
        auto member_data = team_data->CreateData( member_record );
        _kf_kernel->ParseFromMessage( member_data, &kfmsg->pbplayer() );

        // 加入队伍中
        team_data->AddRecord( member_record, member_data->GetKeyID(), member_data );

        // 通知队员加入队伍
        SendJoinTeamToMember( team_data, member_data );
    }
}