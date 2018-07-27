#include "KFGroupClientModule.h"
#include "KFGroupClientConfig.h"

namespace KFrame
{
    KFGroupClientModule::KFGroupClientModule()
    {
        _kf_group_member = nullptr;
    }

    KFGroupClientModule::~KFGroupClientModule()
    {

    }

    void KFGroupClientModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_group_config, true );
    }

    void KFGroupClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterUpdateDataModule( this, &KFGroupClientModule::OnUpdateDataCallBack );
        _kf_component->RegisterUpdateStringModule( this, &KFGroupClientModule::OnUpdateStringCallBack );

        _kf_player->RegisterEnterFunction( this, &KFGroupClientModule::OnEnterQueryMatchGroup );
        _kf_player->RegisterLeaveFunction( this, &KFGroupClientModule::OnLeaveUpdateMatchGroup );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( group ), __KF_STRING__( id ), this, &KFGroupClientModule::OnGroupIdUpdateCallBack );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( group ), __KF_STRING__( maxcount ), this, &KFGroupClientModule::OnGroupMaxCountUpdateCallBack );

        _kf_component->RegisterAddDataFunction( __KF_STRING__( groupmember ), this, &KFGroupClientModule::OnAddGroupMemberCallBack );
        _kf_component->RegisterRemoveDataFunction( __KF_STRING__( groupmember ), this, &KFGroupClientModule::OnRemoveGroupMemberCallBack );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_INVITE_MATCH_GROUP_REQ, &KFGroupClientModule::HandleInviteMatchGroupReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_RECEIVE_INVITE_MATCH_GROUP_REQ, &KFGroupClientModule::HandleReceiveInviteMatchGroupReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REPLY_INVITE_MATCH_GROUP_REQ, &KFGroupClientModule::HandleReplyInviteMatchGroupReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_MATCH_GROUP_MEMBER_ACK, &KFGroupClientModule::HandleAddMatchGroupMemberAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_REMOVE_MATCH_GROUP_MEMBER_ACK, &KFGroupClientModule::HandleRemoveMatchGroupMemberAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_LEAVE_MATCH_GROUP_REQ, &KFGroupClientModule::HandleLeaveMatchGroupReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_KICK_MATCH_GROUP_REQ, &KFGroupClientModule::HandleKickMatchGroupReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_APPLY_MATCH_GROUP_REQ, &KFGroupClientModule::HandleApplyMatchGroupReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REPLY_APPLY_MATCH_GROUP_REQ, &KFGroupClientModule::HandleReplyApplyMatchGroupReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LEAVE_MATCH_GROUP_ACK, &KFGroupClientModule::HandleLeaveMatchGroupAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_CONSENT_APPLY_MATCH_GROUP_ACK, &KFGroupClientModule::HandleConsentApplyMatchGroupAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GROUP_MEMBER_ACK, &KFGroupClientModule::HandleUpdateGroupMemberAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_MATCH_GROUP_PREPARE_REQ, &KFGroupClientModule::HandleMatchGroupPrepareReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GROUP_DATA_ACK, &KFGroupClientModule::HandleUpdateGroupDataAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_APPLY_MATCH_GROUP_ACK, &KFGroupClientModule::HandleApplyMatchGroupAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_GROUP_MATCH_REQ, &KFGroupClientModule::HandleUpdateGroupMatchReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CONSENT_INVITE_MATCH_GROUP_REQ, &KFGroupClientModule::HandleConsentInviteMatchGroupReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_MATCH_GROUP_DATA_ACK, &KFGroupClientModule::HandleTellMatchGroupDataAck );

    }

    void KFGroupClientModule::OnceRun()
    {
        _kf_group_member = _kf_kernel->CreateObject( "Group", __KF_STRING__( groupmember ) );

    }

    void KFGroupClientModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        _kf_kernel->ReleaseObject( _kf_group_member );

        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateStringModule( this );

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        ///////////////////////////////////////////////////////////////////////////////
        _kf_component->UnRegisterUpdateDataFunction( __KF_STRING__( group ), __KF_STRING__( id ) );
        _kf_component->UnRegisterUpdateDataFunction( __KF_STRING__( group ), __KF_STRING__( maxcount ) );

        _kf_component->UnRegisterAddDataFunction( __KF_STRING__( groupmember ) );
        _kf_component->UnRegisterRemoveDataFunction( __KF_STRING__( groupmember ) );
        ///////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_INVITE_MATCH_GROUP_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_RECEIVE_INVITE_MATCH_GROUP_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REPLY_INVITE_MATCH_GROUP_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_MATCH_GROUP_MEMBER_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REMOVE_MATCH_GROUP_MEMBER_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LEAVE_MATCH_GROUP_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_KICK_MATCH_GROUP_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_APPLY_MATCH_GROUP_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REPLY_APPLY_MATCH_GROUP_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LEAVE_MATCH_GROUP_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CONSENT_APPLY_MATCH_GROUP_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GROUP_MEMBER_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_MATCH_GROUP_PREPARE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GROUP_DATA_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_APPLY_MATCH_GROUP_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_UPDATE_GROUP_MATCH_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CONSENT_INVITE_MATCH_GROUP_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_MATCH_GROUP_DATA_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFGroupClientModule::SendMessageToGroup( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToShard( __KF_STRING__( group ), msgid, message );
    }

    bool KFGroupClientModule::SendMessageToGroup( uint64 groupid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToObject( __KF_STRING__( group ), groupid, msgid, message );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFGroupClientModule::OnGroupIdUpdateCallBack )
    {
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        player->UpdateData( kfbasic, __KF_STRING__( groupid ), KFOperateEnum::Set, newvalue );

        if ( newvalue != _invalid_int )
        {
            player->UpdateData( kfbasic, __KF_STRING__( status ), KFOperateEnum::Set, KFMsg::StatusEnum::GroupStatus );
        }
        else
        {
            player->UpdateData( kfbasic, __KF_STRING__( status ), KFOperateEnum::Set, KFMsg::StatusEnum::OnlineStatus );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFGroupClientModule::OnGroupMaxCountUpdateCallBack )
    {
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        player->UpdateData( kfbasic, __KF_STRING__( maxgroupcount ), KFOperateEnum::Set, newvalue );
    }

    __KF_ADD_DATA_FUNCTION__( KFGroupClientModule::OnAddGroupMemberCallBack )
    {
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        auto nowcount = kfparent->Size();
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( nowgroupcount ), KFOperateEnum::Set, nowcount );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFGroupClientModule::OnRemoveGroupMemberCallBack )
    {
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        // 有队伍才更新
        auto groupid = kfbasic->GetValue< uint64 >( __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            return;
        }

        auto nowcount = kfparent->Size();
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( nowgroupcount ), KFOperateEnum::Set, nowcount );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFGroupClientModule::PrepareMatchGroup( KFEntity* player, uint32 matchid, uint32 maxcount )
    {
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        auto groupid = kfbasic->GetValue< uint64 >( __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            auto kfgroup = kfobject->FindData( __KF_STRING__( group ) );
            groupid = kfgroup->GetValue< uint64 >( __KF_STRING__( id ) );
            if ( groupid == _invalid_int )
            {
                // 没有创建小组
                groupid = KFUtility::Make64Guid( player->GetKeyID() );
                kfgroup->SetValue< uint64 >( __KF_STRING__( id ), groupid );
                kfgroup->SetValue< uint32 >( __KF_STRING__( matchid ), matchid );
                kfgroup->SetValue< uint64 >( __KF_STRING__( maxcount ), maxcount );
            }
        }
        else
        {
            // 判断最大数量
            auto maxgroupcount = kfbasic->GetValue< uint32 >( __KF_STRING__( maxgroupcount ) );
            if ( maxcount > maxgroupcount )
            {
                UpdateMatchToGroup( player, groupid, matchid, maxcount );
            }
        }

        return groupid;
    }

    bool KFGroupClientModule::CreateMatchGroup( KFEntity* player, uint64 groupid )
    {
        auto kfobject = player->GetData();
        auto kfgroup = kfobject->FindData( __KF_STRING__( group ) );

        auto matchid = kfgroup->GetValue< uint32 >( __KF_STRING__( matchid ) );
        auto maxcount = kfgroup->GetValue< uint32 >( __KF_STRING__( maxcount ) );

        // 先清空数据
        kfgroup->SetValue< uint64 >( __KF_STRING__( id ), _invalid_int );
        kfgroup->SetValue< uint32 >( __KF_STRING__( matchid ), _invalid_int );
        kfgroup->SetValue< uint32 >( __KF_STRING__( maxcount ), _invalid_int );

        // 发送到队伍集群
        KFMsg::S2SCreateMatchGroupReq req;
        req.set_groupid( groupid );
        req.set_matchid( matchid );
        req.set_maxcount( maxcount );
        req.set_playerid( player->GetKeyID() );
        FormatMatchGroupMember( player, req.mutable_pbmember() );
        return SendMessageToGroup( KFMsg::S2S_CREATE_MATCH_GROUP_REQ, &req );
    }

    void KFGroupClientModule::FormatMatchGroupMember( KFEntity* player, KFMsg::PBObject* pbobject )
    {
        // 基础信息
        auto kfobject = player->GetData();
        auto kfplayerbasic = kfobject->FindData( __KF_STRING__( basic ) );
        auto kfmemberbasic = _kf_group_member->FindData( __KF_STRING__( basic ) );
        kfmemberbasic->CopyFrom( kfplayerbasic );

        // 队员id
        _kf_group_member->SetKeyID( player->GetKeyID() );

        // 模型id
        auto modelid = kfobject->GetValue< uint32 >( __KF_STRING__( modelid ) );
        _kf_group_member->SetValue< uint32 >( __KF_STRING__( modelid ), modelid );

        // 时装id
        auto clothesid = kfobject->GetValue< uint32 >( __KF_STRING__( clothesid ) );
        _kf_group_member->SetValue< uint32 >( __KF_STRING__( clothesid ), clothesid );

        // 序列化
        _kf_kernel->SerializeToClient( _kf_group_member, pbobject );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleUpdateGroupMatchReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateGroupMatchReq );

        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        auto groupid = kfbasic->GetValue< uint64 >( __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupNotInGroup );
        }

        auto maxcount = _kf_match->GetMatchMaxCount( kfmsg.matchid() );
        auto nowgroupcount = kfbasic->GetValue< uint32 >( __KF_STRING__( nowgroupcount ) );
        if ( maxcount < nowgroupcount )
        {
            return _kf_display->SendToClient( player, KFMsg::MatchGroupPlayerLimit );
        }

        UpdateMatchToGroup( player, groupid, kfmsg.matchid(), maxcount );
    }

    void KFGroupClientModule::UpdateMatchToGroup( KFEntity* player, uint64 groupid, uint32 matchid, uint32 maxcount )
    {
        KFMsg::S2SUpdateGroupMatchReq req;
        req.set_groupid( groupid );
        req.set_matchid( matchid );
        req.set_maxcount( maxcount );
        req.set_playerid( player->GetKeyID() );
        SendMessageToGroup( groupid, KFMsg::S2S_UPDATE_GROUP_MATCH_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleInviteMatchGroupReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgInviteMatchGroupReq );

        // 已经在匹配, 不能邀请
        auto kfobject = player->GetData();
        auto matchid = kfobject->GetValue< uint32 >( __KF_STRING__( matchid ) );
        if ( matchid != _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupInviteInMatch );
        }

        auto maxcount = _kf_match->GetMatchMaxCount( kfmsg.matchid() );
        if ( maxcount == 1 )
        {
            return _kf_display->SendToClient( player, KFMsg::MatchGroupPlayerLimit );
        }

        // 判断队伍人数
        auto membercount = GetGroupMemberCount( kfobject );
        if ( membercount >= maxcount )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupMemberIsFull );
        }

        // 先不要创建队伍
        auto groupid = PrepareMatchGroup( player, kfmsg.matchid(), maxcount );

        // 转发给被邀请者
        KFMsg::S2SReceiveInviteMatchGroupReq req;
        req.set_groupid( groupid );
        req.set_source( kfmsg.source() );
        req.set_matchid( kfmsg.matchid() );
        req.set_playerid( kfmsg.playerid() );
        req.set_playername( kfmsg.playername() );
        req.set_inviterplayerid( playerid );
        req.set_inviterserverid( KFGlobal::Instance()->_app_id );
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        _kf_kernel->SerializeToClient( kfbasic, req.mutable_inviter() );
        auto ok = _kf_route->SendMessageToRoute( kfmsg.serverid(), kfmsg.playerid(), KFMsg::S2S_RECEIVE_INVITE_MATCH_GROUP_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::GroupServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleReceiveInviteMatchGroupReq )
    {
        __PROTO_PARSE__( KFMsg::S2SReceiveInviteMatchGroupReq );

        auto player = _kf_player->FindPlayer( kfmsg.playerid(), __FUNCTION_LINE__ );
        if ( player == nullptr )
        {
            return _kf_display->SendToPlayer( kfmsg.inviterserverid(), kfmsg.inviterplayerid(),
                                              KFMsg::GroupPlayerOffline, kfmsg.playername() );
        }

        auto kfobject = player->GetData();
        auto groupid = kfobject->GetValue< uint32 >( __KF_STRING__( basic ), __KF_STRING__( groupid ) );
        if ( groupid != _invalid_int )
        {
            return _kf_display->SendToPlayer( kfmsg.inviterserverid(), kfmsg.inviterplayerid(),
                                              KFMsg::GroupAlreadyInGroup, kfmsg.playername() );
        }

        // 如果有设置5分钟内部接受邀请
        auto refusegrouptime = kfobject->GetValue< uint64 >( __KF_STRING__( refusegroupinvite ) );
        if ( refusegrouptime > KFGlobal::Instance()->_real_time )
        {
            return _kf_display->SendToPlayer( kfmsg.inviterserverid(), kfmsg.inviterplayerid(),
                                              KFMsg::GroupRefuseInvite, kfmsg.playername() );
        }

        _kf_display->SendToPlayer( kfmsg.inviterserverid(), kfmsg.inviterplayerid(),
                                   KFMsg::InviteMatchGroupOK, kfmsg.playername() );

        // 添加到邀请列表
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( groupinvite ) );
        auto kfinvite = _kf_kernel->CreateObject( kfinviterecord->GetDataSetting() );
        kfinvite->SetValue< uint64 >( __KF_STRING__( id ), kfmsg.groupid() );
        kfinvite->SetValue< uint32 >( __KF_STRING__( matchid ), kfmsg.matchid() );
        kfinvite->SetValue< std::string >( __KF_STRING__( source ), kfmsg.source() );
        kfinvite->SetValue< uint64 >( __KF_STRING__( time ), KFGlobal::Instance()->_real_time );

        auto kfbasic = kfinvite->FindData( __KF_STRING__( basic ) );
        _kf_kernel->ParseFromProto( kfbasic, &kfmsg.inviter() );

        player->RemoveData( kfinviterecord, kfmsg.groupid() );
        player->AddData( kfinviterecord, kfmsg.groupid(), kfinvite );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleReplyInviteMatchGroupReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReplyInviteMatchGroupReq );

        auto kfobject = player->GetData();
        auto kfinviterecord = kfobject->FindData( __KF_STRING__( groupinvite ) );
        auto kfinvite = kfinviterecord->FindData( kfmsg.groupid() );
        if ( kfinvite == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupInviteNotExist );
        }

        if ( kfmsg.operate() == KFMsg::InviteEnum::Refuse || kfmsg.operate() == KFMsg::InviteEnum::RefuseMinute )
        {
            if ( kfmsg.operate() == KFMsg::InviteEnum::RefuseMinute )
            {
                auto refusetime = KFGlobal::Instance()->_real_time + _kf_group_config->_refuse_time;
                kfobject->SetValue< uint64 >( __KF_STRING__( refusegroupinvite ), refusetime );
            }

            auto name = kfobject->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );

            // 转发消息, 拒绝了邀请
            auto kfbasic = kfinvite->FindData( __KF_STRING__( basic ) );
            _kf_display->SendToPlayer( kfbasic, KFMsg::GroupRefuseYourInvite, name );
        }
        else if ( kfmsg.operate() == KFMsg::InviteEnum::Consent )
        {
            auto groupid = kfobject->GetValue< uint64 >( __KF_STRING__( basic ), __KF_STRING__( groupid ) );
            if ( groupid != _invalid_int )
            {
                return _kf_display->SendToClient( player, KFMsg::GroupInGroup );
            }

            // 防止操作太快, 加入两个队伍
            auto operatetime = kfobject->GetValue< uint64 >( __KF_STRING__( operatetime ) );
            if ( KFGlobal::Instance()->_game_time < operatetime + 2000 )
            {
                return _kf_display->SendToClient( player, KFMsg::OperateFrequently );
            }

            // 判断邀请时间
            auto invitetime = kfinvite->GetValue< uint64 >( __KF_STRING__( time ) );
            if ( KFGlobal::Instance()->_real_time > invitetime + _kf_group_config->_invite_valid_time )
            {
                // 超时
                _kf_display->SendToClient( player, KFMsg::GroupInviteTimeOut );
            }
            else
            {
                // 转发消息给邀请者, 同意了邀请
                KFMsg::S2SConsentInviteMatchGroupReq req;
                req.set_groupid( kfmsg.groupid() );
                req.set_playerid( player->GetKeyID() );
                req.set_serverid( KFGlobal::Instance()->_app_id );
                FormatMatchGroupMember( player, req.mutable_pbmember() );

                auto kfbasic = kfinvite->FindData( __KF_STRING__( basic ) );
                auto ok = _kf_player->SendMessageToClient( kfbasic, KFMsg::S2S_CONSENT_INVITE_MATCH_GROUP_REQ, &req );
                if ( ok )
                {
                    kfobject->SetValue< uint64 >( __KF_STRING__( operatetime ), KFGlobal::Instance()->_game_time );
                }
                else
                {
                    _kf_display->SendToClient( player, KFMsg::GroupServerBusy );
                }
            }
        }

        // 删除邀请信息
        player->RemoveData( kfinviterecord, kfmsg.groupid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleConsentInviteMatchGroupReq )
    {
        __PROTO_PARSE__( KFMsg::S2SConsentInviteMatchGroupReq );

        auto playerid = __KF_DATA_ID__( kfguid );
        auto player = _kf_player->FindPlayer( playerid, __FUNCTION_LINE__ );
        if ( player == nullptr )
        {
            return _kf_display->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupNotExist );
        }

        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        auto kfgroup = kfobject->FindData( __KF_STRING__( group ) );

        auto groupid = kfbasic->GetValue< uint64 >( __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            // 没有队伍, 创建一个新的队伍
            groupid = kfgroup->GetValue< uint64 >( __KF_STRING__( id ) );
            if ( groupid == _invalid_int )
            {
                groupid = kfmsg.groupid();
            }

            // 创建队伍
            auto ok = CreateMatchGroup( player, groupid );
            if ( !ok )
            {
                return _kf_display->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupServerBusy );
            }
        }
        else
        {
            // 队长已经在其他队伍中, 不能加入队伍
            if ( groupid != kfmsg.groupid() )
            {
                return _kf_display->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupNotExist );
            }

            // 判断队员数量
            auto maxgroupcount = kfgroup->GetValue< uint32 >( __KF_STRING__( maxcount ) );
            auto kfmemberrecord = kfgroup->FindData( __KF_STRING__( groupmember ) );
            if ( kfmemberrecord->Size() >= maxgroupcount )
            {
                return _kf_display->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupMemberIsFull );
            }
        }

        // 发送到集群
        KFMsg::S2SAddMatchGroupMemberReq req;
        req.set_groupid( groupid );
        req.set_playerid( kfmsg.playerid() );
        req.set_serverid( kfmsg.serverid() );
        req.mutable_pbmember()->CopyFrom( kfmsg.pbmember() );
        auto ok = SendMessageToGroup( groupid, KFMsg::S2S_ADD_MATCH_GROUP_MEMBER_REQ, &req );
        if ( !ok )
        {
            return _kf_display->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleTellMatchGroupDataAck )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2STellMatchGroupDataAck );

        auto kfobject = player->GetData();
        auto kfgroup = kfobject->FindData( __KF_STRING__( group ) );

        player->UpdateData( kfgroup, __KF_STRING__( id ), KFOperateEnum::Set, kfmsg.groupid() );
        player->UpdateData( kfgroup, __KF_STRING__( matchid ), KFOperateEnum::Set, kfmsg.matchid() );
        player->UpdateData( kfgroup, __KF_STRING__( maxcount ), KFOperateEnum::Set, kfmsg.maxcount() );
        player->UpdateData( kfgroup, __KF_STRING__( captainid ), KFOperateEnum::Set, kfmsg.captainid() );

        // 加入队员
        auto kfmemberrecord = kfgroup->FindData( __KF_STRING__( groupmember ) );
        for ( auto i = 0; i < kfmsg.pbmember_size(); ++i )
        {
            auto pbmember = &kfmsg.pbmember( i );

            auto kfmember = _kf_kernel->CreateObject( kfmemberrecord->GetDataSetting(), pbmember );
            player->AddData( kfmemberrecord, kfmember );
        }

        // 删除邀请列表
        player->RemoveData( __KF_STRING__( groupinvite ) );

        // 新加入通知客户端
        if ( kfmsg.newadd() )
        {
            _kf_display->SendToClient( player, KFMsg::GroupJoinOK );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleAddMatchGroupMemberAck )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2SAddMatchGroupMemberAck );

        auto kfobject = player->GetData();
        auto kfmemberrecord = kfobject->FindData( __KF_STRING__( group ), __KF_STRING__( groupmember ) );

        auto kfmember = _kf_kernel->CreateObject( kfmemberrecord->GetDataSetting(), &kfmsg.pbmember() );
        player->AddData( kfmemberrecord, kfmember );

        // 通知玩家
        auto name = kfmember->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
        _kf_display->SendToClient( player, KFMsg::GroupMemberJoin, name );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleRemoveMatchGroupMemberAck )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2SRemoveMatchGroupMemberAck );

        auto kfobject = player->GetData();
        auto kfmemberrecord = kfobject->FindData( __KF_STRING__( group ), __KF_STRING__( groupmember ) );

        auto kfmember = kfmemberrecord->FindData( kfmsg.memberid() );
        if ( kfmember != nullptr )
        {
            auto name = kfmember->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) );
            _kf_display->SendToClient( player, KFMsg::GroupMemberLeave, name );
        }

        player->RemoveData( kfmemberrecord, kfmsg.memberid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleLeaveMatchGroupReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgLeaveMatchGroupReq );

        auto kfobject = player->GetData();
        auto groupid = kfobject->GetValue< uint64 >( __KF_STRING__( basic ), __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupNotInGroup );
        }

        // 离开队伍
        ProcessLeaveMatchGroup( player );

        // 通知组队服务器
        KFMsg::S2SLeaveMatchGroupReq req;
        req.set_groupid( groupid );
        req.set_playerid( playerid );
        auto ok = SendMessageToGroup( groupid, KFMsg::S2S_LEAVE_MATCH_GROUP_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::GroupServerBusy );
        }
    }

    void KFGroupClientModule::ProcessLeaveMatchGroup( KFEntity* player )
    {
        // 清除队伍id
        player->UpdateData( __KF_STRING__( group ), __KF_STRING__( id ), KFOperateEnum::Set, _invalid_int );

        // 删除队员
        player->RemoveData( __KF_STRING__( groupinvite ) );
        player->RemoveData( __KF_STRING__( groupapply ) );
        player->RemoveData( __KF_STRING__( group ), __KF_STRING__( groupmember ) );

        // 通知离开队伍
        _kf_display->SendToClient( player, KFMsg::GroupLeaveOK );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleKickMatchGroupReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgKickMatchGroupReq );

        auto kfobject = player->GetData();
        auto groupid = kfobject->GetValue< uint64 >( __KF_STRING__( basic ), __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupNotInGroup );
        }

        if ( kfmsg.memberid() == playerid )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupCanNotKickSelf );
        }

        // 通知组队服务器
        KFMsg::S2SKickMatchGroupReq req;
        req.set_groupid( groupid );
        req.set_captainid( playerid );
        req.set_memberid( kfmsg.memberid() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        auto ok = SendMessageToGroup( groupid, KFMsg::S2S_KICK_MATCH_GROUP_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::GroupServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleLeaveMatchGroupAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SLeaveMatchGroupAck );

        // 离开队伍
        ProcessLeaveMatchGroup( player );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleApplyMatchGroupReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgApplyMatchGroupReq );

        auto kfobject = player->GetData();
        auto groupid = kfobject->GetValue< uint64 >( __KF_STRING__( basic ), __KF_STRING__( groupid ) );
        if ( groupid != _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupInGroup );
        }

        KFMsg::S2SApplyMatchGroupReq req;
        req.set_groupid( kfmsg.groupid() );
        req.set_playerid( playerid );
        req.set_serverid( KFGlobal::Instance()->_app_id );

        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        _kf_kernel->SerializeToClient( kfbasic, req.mutable_pbmember() );

        auto ok = SendMessageToGroup( kfmsg.groupid(), KFMsg::S2S_APPLY_MATCH_GROUP_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::GroupServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleApplyMatchGroupAck )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2SApplyMatchGroupAck );

        // 添加到申请列表
        auto kfobject = player->GetData();
        auto matchid = kfobject->GetValue< uint32 >( __KF_STRING__( matchid ) );
        if ( matchid != _invalid_int )
        {
            return _kf_display->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupApplyInMatch );
        }

        auto kfapplyrecord = kfobject->FindData( __KF_STRING__( groupapply ) );
        auto kfapply = kfapplyrecord->FindData( kfmsg.playerid() );
        if ( kfapply != nullptr )
        {
            player->RemoveData( kfapplyrecord, kfmsg.playerid() );
        }

        kfapply = _kf_kernel->CreateObject( kfapplyrecord->GetDataSetting() );
        kfapply->SetValue< uint32 >( __KF_STRING__( id ), kfmsg.playerid() );
        kfapply->SetValue< uint64 >( __KF_STRING__( time ), KFGlobal::Instance()->_real_time );

        auto kfbasic = kfapply->FindData( __KF_STRING__( basic ) );
        _kf_kernel->ParseFromProto( kfbasic, &kfmsg.pbmember() );

        player->AddData( kfapplyrecord, kfapply );
    }

    uint32 KFGroupClientModule::GetGroupMemberCount( KFData* kfobject )
    {
        auto kfgroupmember = kfobject->FindData( __KF_STRING__( group ), __KF_STRING__( groupmember ) );
        if ( kfgroupmember == nullptr )
        {
            return _invalid_int;
        }

        return kfgroupmember->Size();
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleReplyApplyMatchGroupReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReplyApplyMatchGroupReq );

        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        auto groupid = kfbasic->GetValue< uint64 >( __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupNotInGroup );
        }

        // 是否存在邀请
        auto kfapply = kfobject->FindData( __KF_STRING__( groupapply ), kfmsg.applyid(), __KF_STRING__( basic ) );
        if ( kfapply == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupApplyNotExist );
        }

        if ( kfmsg.operate() == KFMsg::InviteEnum::Refuse )
        {
            // 拒绝
            auto name = kfbasic->GetValue< std::string >( __KF_STRING__( name ) );
            _kf_display->SendToPlayer( kfapply, KFMsg::GroupRefuseApply, name );
        }
        else if ( kfmsg.operate() == KFMsg::InviteEnum::Consent )
        {
            auto applytime = kfapply->GetValue< uint64 >( __KF_STRING__( time ) );
            if ( KFGlobal::Instance()->_real_time > ( applytime + _kf_group_config->_invite_valid_time ) )
            {
                _kf_display->SendToClient( player, KFMsg::GroupApplyTimeOut );
            }
            else
            {
                // 判断数量
                auto membercount = GetGroupMemberCount( kfobject );
                auto maxcount = kfbasic->GetValue< uint32 >( __KF_STRING__( maxgroupcount ) );

                if ( membercount >= maxcount )
                {
                    _kf_display->SendToClient( player, KFMsg::GroupMemberIsFull );
                }
                else
                {
                    // 转发给玩家, 同意了申请
                    KFMsg::S2SConsentApplyMatchGroupAck ack;
                    ack.set_groupid( groupid );
                    ack.set_captainid( playerid );
                    ack.set_serverid( KFGlobal::Instance()->_app_id );
                    ack.set_playerid( kfmsg.applyid() );
                    ack.set_playername( kfapply->GetValue< std::string >( __KF_STRING__( basic ), __KF_STRING__( name ) ) );
                    auto ok = _kf_player->SendMessageToClient( kfapply, KFMsg::S2S_CONSENT_APPLY_MATCH_GROUP_ACK, &ack );
                    if ( ok )
                    {
                        return _kf_display->SendToClient( player, KFMsg::GroupServerBusy );
                    }
                }
            }
        }

        // 删除邀请信息
        player->RemoveData( __KF_STRING__( groupapply ), kfmsg.applyid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleConsentApplyMatchGroupAck )
    {
        __PROTO_PARSE__( KFMsg::S2SConsentApplyMatchGroupAck );

        auto player = _kf_player->FindPlayer( kfmsg.playerid(), __FUNCTION_LINE__ );
        if ( player == nullptr )
        {
            return _kf_display->SendToPlayer( kfmsg.serverid(), kfmsg.captainid(), KFMsg::GroupPlayerOffline, kfmsg.playername() );
        }

        // 判断是否有队伍
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        auto groupid = kfbasic->GetValue< uint64 >( __KF_STRING__( groupid ) );
        if ( groupid != _invalid_int )
        {
            return _kf_display->SendToPlayer( kfmsg.serverid(), kfmsg.captainid(), KFMsg::GroupAlreadyInGroup, kfmsg.playername() );
        }

        // 加入队员
        KFMsg::S2SAddMatchGroupMemberReq req;
        req.set_groupid( kfmsg.groupid() );
        req.set_playerid( kfmsg.playerid() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        FormatMatchGroupMember( player, req.mutable_pbmember() );
        auto ok = SendMessageToGroup( groupid, KFMsg::S2S_ADD_MATCH_GROUP_MEMBER_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToPlayer( kfmsg.playerid(), kfmsg.captainid(), KFMsg::GroupServerBusy );
        }
    }

    void KFGroupClientModule::OnEnterQueryMatchGroup( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        auto groupid = kfbasic->GetValue< uint64 >( __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            return;
        }

        // 先设置成没有队伍
        kfbasic->SetValue< uint64 >( __KF_STRING__( groupid ), _invalid_int );

        // 查询玩家队伍信息
        KFMsg::S2SOnLineQueryMatchGroupReq req;
        req.set_groupid( groupid );
        req.set_playerid( player->GetKeyID() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToGroup( groupid, KFMsg::S2S_ONLINE_QUERY_MATCH_GROUP_REQ, &req );
    }

    void KFGroupClientModule::OnLeaveUpdateMatchGroup( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto groupid = kfobject->GetValue< uint64 >( __KF_STRING__( basic ), __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SOffLineUpdateMatchGroupReq req;
        req.set_groupid( groupid );
        req.set_playerid( player->GetKeyID() );
        SendMessageToGroup( groupid, KFMsg::S2S_OFFLINE_UPDATE_MATCH_GROUP_REQ, &req );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFGroupClientModule::OnUpdateDataCallBack )
    {
        OnUpdateDataToGroup( player, kfdata );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFGroupClientModule::OnUpdateStringCallBack )
    {
        OnUpdateDataToGroup( player, kfdata );
    }

    void KFGroupClientModule::OnUpdateDataToGroup( KFEntity* player, KFData* kfdata )
    {
        auto kfparent = kfdata->GetParent();
        if ( !kfdata->HaveFlagMask( KFDataDefine::Mask_Group_Data ) ||
                !kfparent->HaveFlagMask( KFDataDefine::Mask_Group_Data ) )
        {
            return;
        }

        // 是否有队伍
        auto kfobject = player->GetData();
        auto groupid = kfobject->GetValue< uint64 >( __KF_STRING__( basic ), __KF_STRING__( groupid ) );
        if ( groupid == _invalid_int )
        {
            return;
        }

        // 更新到组队服务器
        KFMsg::PBStrings pbstrings;
        auto pbstring = pbstrings.add_pbstring();
        pbstring->set_name( kfdata->GetName() );
        pbstring->set_value( kfdata->ToString() );
        UpdateMemberToMatchGroup( groupid, player->GetKeyID(), kfparent->GetName(), pbstrings );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleUpdateGroupMemberAck )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2SUpdateGroupMemberAck );

        auto kfobject = player->GetData();
        auto kfmember = kfobject->FindData( __KF_STRING__( group ), __KF_STRING__( groupmember ), kfmsg.memberid() );
        if ( kfmember == nullptr )
        {
            return;
        }

        auto kfupdate = kfmember->FindData( kfmsg.dataname() );
        if ( kfupdate == nullptr )
        {
            kfupdate = kfmember;
        }

        auto pbstrings = &kfmsg.pbstrings();
        for ( auto i = 0; i < pbstrings->pbstring_size(); ++i )
        {
            auto pbstring = &pbstrings->pbstring( i );
            player->UpdateData( kfupdate, pbstring->name(), pbstring->value() );
        }
    }

    bool KFGroupClientModule::UpdateMemberToMatchGroup( uint64 groupid, uint32 playerid, const std::string& dataname, const KFMsg::PBStrings& pbstrings )
    {
        KFMsg::S2SUpdateGroupMemberReq req;
        req.set_groupid( groupid );
        req.set_memberid( playerid );
        req.set_dataname( dataname );
        req.mutable_pbstrings()->CopyFrom( pbstrings );
        return SendMessageToGroup( groupid, KFMsg::S2S_UPDATE_GROUP_MEMBER_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleMatchGroupPrepareReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMatchGroupPrepareReq );

        auto kfobject = player->GetData();
        auto kfgroup = kfobject->FindData( __KF_STRING__( group ) );

        auto groupid = kfgroup->GetValue< uint64 >( __KF_STRING__( id ) );
        if ( groupid == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::GroupNotInGroup );
        }

        // 更新准备状态
        KFMsg::PBStrings pbstrings;
        auto pbstring = pbstrings.add_pbstring();
        pbstring->set_name( __KF_STRING__( prepare ) );
        pbstring->set_value( KFUtility::ToString< uint32 >( kfmsg.prepare() ) );
        auto ok = UpdateMemberToMatchGroup( groupid, playerid, _invalid_str, pbstrings );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::GroupServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGroupClientModule::HandleUpdateGroupDataAck )
    {
        __ROUTE_PROTO_PARSE__( KFMsg::S2SUpdateGroupDataAck );

        auto kfobject = player->GetData();
        auto kfgroup = kfobject->FindData( __KF_STRING__( group ) );

        for ( auto i = 0; i < kfmsg.pbstring_size(); ++i )
        {
            auto pbstring = &kfmsg.pbstring( i );
            player->UpdateData( kfgroup, pbstring->name(), pbstring->value() );
        }
    }

    bool KFGroupClientModule::IsGroupMember( KFEntity* player, uint32 playerid )
    {
        auto kfobject = player->GetData();
        auto kfmember = kfobject->FindData( __KF_STRING__( group ), __KF_STRING__( groupmember ), playerid );
        return kfmember != nullptr;
    }

    bool KFGroupClientModule::IsGroupCaptain( KFEntity* player, uint32 playerid )
    {
        auto kfobject = player->GetData();
        auto kfgroup = kfobject->FindData( __KF_STRING__( group ) );
        auto captainid = kfgroup->GetValue< uint32 >( __KF_STRING__( captainid ) );
        return captainid == playerid;
    }

    void KFGroupClientModule::RemoveGroupMember( uint64 groupid, uint32 playerid )
    {
        // 通知组队服务器
        KFMsg::S2SLeaveMatchGroupReq req;
        req.set_groupid( groupid );
        req.set_playerid( playerid );
        SendMessageToGroup( groupid, KFMsg::S2S_LEAVE_MATCH_GROUP_REQ, &req );
    }

    uint32 KFGroupClientModule::GroupMemberCount( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfmemberrecord = kfobject->FindData( __KF_STRING__( group ), __KF_STRING__( groupmember ) );
        auto count = kfmemberrecord->Size();
        return __MAX__( count, 1 );
    }

    bool KFGroupClientModule::IsInGroup( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto groupid = kfobject->GetValue< uint64 >( __KF_STRING__( group ), __KF_STRING__( id ) );
        return groupid != _invalid_int;
    }

}