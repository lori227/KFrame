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
            auto serverid = kfmember->Get<uint64>( __STRING__( basic ), __STRING__( serverid ) );
            _kf_route->SendToPlayer( serverid, kfmember->GetKeyID(), msgid, message );
        }
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
        KFMsg::S2STeamSyncUpdateData sync;
        sync.mutable_pbdata()->Swap( &pbobject );
        SendMessageToTeam( kfteam, KFMsg::S2S_TEAM_SYNC_UPDATE_DATA, &sync );
    }

    void KFTeamShardModule::SendTeamAddDataToMember( KFEntity* kfteam, KFMsg::PBObject& pbobject )
    {
        KFMsg::S2STeamSyncAddData sync;
        sync.mutable_pbdata()->Swap( &pbobject );
        SendMessageToTeam( kfteam, KFMsg::S2S_TEAM_SYNC_ADD_DATA, &sync );
    }

    void KFTeamShardModule::SendTeamRemoveDataToMember( KFEntity* kfteam, KFMsg::PBObject& pbobject )
    {
        KFMsg::S2STeamSyncRemoveData sync;
        sync.mutable_pbdata()->Swap( &pbobject );
        SendMessageToTeam( kfteam, KFMsg::S2S_TEAM_SYNC_REMOVE_DATA, &sync );
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
        auto playerid = kfmember->GetKeyID();
        auto serverid = kfmember->Get<uint64>( __STRING__( basic ), __STRING__( serverid ) );

        KFMsg::S2STeamJoinToGameAck ack;
        ack.mutable_pbteam()->CopyFrom( *pbteam );
        _kf_route->SendToPlayer( serverid, playerid, KFMsg::S2S_TEAM_JON_TO_GAME_ACK, &ack );
    }
}