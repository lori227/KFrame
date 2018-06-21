#include "KFGroupShardModule.h"
#include "KFGroupShardConfig.h"

namespace KFrame
{

	KFGroupShardModule::KFGroupShardModule()
	{
		_kf_component = nullptr;
	}

	KFGroupShardModule::~KFGroupShardModule()
	{
	}

	void KFGroupShardModule::InitModule()
	{
		_kf_config->AddConfig( _kf_group_config, _kf_plugin->_plugin_name, _kf_plugin->_config, false );
		///////////////////////////////////////////////////////////////////////////////
	}

	void KFGroupShardModule::BeforeRun()
	{
		_kf_component = _kf_kernel->FindComponent( KFField::_group );
		_kf_tcp_server->RegisterDiscoverFunction( this, &KFGroupShardModule::OnServerDiscoverClient );
		////////////////////////////////////////////////////////////////////////
		__REGISTER_MESSAGE__( KFMsg::S2S_CREATE_MATCH_GROUP_TO_SHARD_REQ, &KFGroupShardModule::HandleCreateMatchGroupToShardReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_ADD_MATCH_GROUP_MEMBER_REQ, &KFGroupShardModule::HandleAddMatchGroupMemberReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_LEAVE_MATCH_GROUP_REQ, &KFGroupShardModule::HandleLeaveMatchGroupReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_KICK_MATCH_GROUP_REQ, &KFGroupShardModule::HandleKickMatchGroupReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_ONLINE_QUERY_MATCH_GROUP_REQ, &KFGroupShardModule::HandleOnLineQueryGroupReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GROUP_MEMBER_REQ, &KFGroupShardModule::HandleUpdateGroupMemberReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_OFFLINE_UPDATE_MATCH_GROUP_REQ, &KFGroupShardModule::HandleOffLineUpdateGroupReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_APPLY_MATCH_GROUP_REQ, &KFGroupShardModule::HandleApplyMatchGroupReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GROUP_MATCH_REQ, &KFGroupShardModule::HandleUpdateGroupMatchReq );
		
	}

	void KFGroupShardModule::BeforeShut()
	{
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
		_kf_tcp_server->UnRegisterDiscoverFunction( this );
		////////////////////////////////////////////////////////////////////////
		__UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_MATCH_GROUP_TO_SHARD_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_MATCH_GROUP_MEMBER_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_LEAVE_MATCH_GROUP_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_MATCH_GROUP_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_ONLINE_QUERY_MATCH_GROUP_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GROUP_MEMBER_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_OFFLINE_UPDATE_MATCH_GROUP_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_APPLY_MATCH_GROUP_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GROUP_MATCH_REQ );
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	__KF_SERVER_DISCOVER_FUNCTION__( KFGroupShardModule::OnServerDiscoverClient )
	{
		if ( handlename == KFField::_group && handletype == KFField::_proxy )
		{
			OnServerDiscoverGroupProxy( handleid );
		}
	}

	void KFGroupShardModule::OnServerDiscoverGroupProxy( uint32 proxyid )
	{
		std::list< uint64 > grouplist;

		auto* kfgroup = _kf_component->FirstEntity();
		while ( kfgroup != nullptr )
		{
			grouplist.push_back( kfgroup->GetKeyID() );
			kfgroup = _kf_component->NextEntity();
		}
		
		_kf_cluster_shard->AddObjectToProxy( proxyid, grouplist );
	}
	
	bool KFGroupShardModule::SendMessageToMember( KFData* kfmember, uint32 msgid, ::google::protobuf::Message* message )
	{
		auto kfbasic = kfmember->FindData( KFField::_basic );
		auto serverid = kfbasic->GetValue< uint32 >( KFField::_server_id );
		if ( serverid == _invalid_int )
		{
			return false;
		}
	
		auto playerid = kfbasic->GetValue< uint32 >( KFField::_id );
		return _kf_cluster_shard->SendMessageToPlayer( serverid, playerid, msgid, message );
	}

	void KFGroupShardModule::SendMessageToGroup( KFData* kfmemberrecord, uint32 memberid, uint32 msgid, ::google::protobuf::Message* message )
	{
		auto kfmember = kfmemberrecord->FirstData();
		while ( kfmember != nullptr )
		{
			if ( kfmember->GetKeyID() != memberid )
			{
				SendMessageToMember( kfmember, msgid, message );
			}
			kfmember = kfmemberrecord->NextData();
		}
	}

	__KF_MESSAGE_FUNCTION__( KFGroupShardModule::HandleCreateMatchGroupToShardReq )
	{
		__PROTO_PARSE__( KFMsg::S2SCreateMatchGroupToShardReq );

		auto strgroupid = __KF_STRING__( kfmsg.groupid() );
		KFLogger::LogLogic( KFLogger::Debug, "[%s] create group[%s] req!",
			__FUNCTION__, strgroupid.c_str() );

		auto kfgroup = _kf_component->CreateEntity( kfmsg.groupid() );
		
		auto kfobject = kfgroup->GetData();
		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );

		auto kfmember = _kf_kernel->CreateObject( kfmemberrecord->GetDataSetting() );
		_kf_kernel->ParseFromProto( kfmember, &kfmsg.pbmember() );
		
		kfmember->SetValue< uint32 >( KFField::_is_captain, 1 );
		kfobject->SetValue< uint32 >( KFField::_captain_id, kfmember->GetKeyID() );
		kfobject->SetValue< uint32 >( KFField::_max_count, kfmsg.maxcount() );
		kfobject->SetValue< uint32 >( KFField::_match_id, kfmsg.matchid() );

		kfmemberrecord->AddData( kfmember->GetKeyID(), kfmember );

		// 更新到proxy
		_kf_cluster_shard->AddObjectToProxy( kfmsg.groupid() );
		KFLogger::LogLogic( KFLogger::Debug, "[%s] create group[%s] ok!",
			__FUNCTION__, strgroupid.c_str() );
	}

	__KF_MESSAGE_FUNCTION__( KFGroupShardModule::HandleAddMatchGroupMemberReq )
	{
		__PROTO_PARSE__( KFMsg::S2SAddMatchGroupMemberReq );
		auto strgroupid = __KF_STRING__( kfmsg.groupid() );
		KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] add member req!",
			__FUNCTION__, strgroupid.c_str() );

		auto kfgroup = _kf_component->FindEntity( kfmsg.groupid(), __FUNCTION_LINE__ );
		if ( kfgroup == nullptr )
		{
			_kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupNotExist );
			return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] can't find!",
				__FUNCTION__, strgroupid.c_str() );
		}

		// 判断成员数量
		auto kfobject = kfgroup->GetData();
		auto maxcount = kfobject->GetValue< uint32 >( KFField::_max_count );

		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );
		if ( kfmemberrecord->Size() >= maxcount )
		{
			_kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupMemberIsFull );
			return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] is full!",
				__FUNCTION__, strgroupid.c_str() );
		}

		// 加入新成员
		auto kfnewmember = _kf_kernel->CreateObject( kfmemberrecord->GetDataSetting(), &kfmsg.pbmember() );
		kfmemberrecord->AddData( kfnewmember->GetKeyID(), kfnewmember );

		{
			// 通知新队员加入
			KFMsg::S2SAddMatchGroupMemberAck ack;
			ack.add_pbmember()->CopyFrom( kfmsg.pbmember() );
			SendMessageToGroup( kfmemberrecord, kfnewmember->GetKeyID(), KFMsg::S2S_ADD_MATCH_GROUP_MEMBER_ACK, &ack );
		}

		{
			// 通知新队员队伍信息
			SendGroupDataToMember( kfmemberrecord, kfnewmember, kfgroup, true );
		}

		KFLogger::LogLogic( KFLogger::Debug, "[%s] reply invite group[%s] ok!",
			__FUNCTION__, strgroupid.c_str() );
	}

	__KF_MESSAGE_FUNCTION__( KFGroupShardModule::HandleApplyMatchGroupReq )
	{
		__PROTO_PARSE__( KFMsg::S2SApplyMatchGroupReq );

		auto strgroupid = __KF_STRING__( kfmsg.groupid() );
		auto kfgroup = _kf_component->FindEntity( kfmsg.groupid(), __FUNCTION_LINE__ );
		if ( kfgroup == nullptr )
		{
			return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupNotExist );
		}

		auto kfobject = kfgroup->GetData();
		auto captainid = kfobject->GetValue< uint32 >( KFField::_captain_id );
		auto kfmember = kfobject->FindData( KFField::_group_member, captainid );
		if ( kfmember == nullptr )
		{
			 _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupNoCaption );
			 return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] can't captain!",
				__FUNCTION__, strgroupid.c_str() );
		}

		KFMsg::S2SApplyMatchGroupAck ack;
		ack.set_playerid( kfmsg.playerid() );
		ack.set_serverid( kfmsg.serverid() );
		ack.mutable_pbmember()->CopyFrom( kfmsg.pbmember() );
		auto ok = SendMessageToMember( kfmember, KFMsg::S2S_APPLY_MATCH_GROUP_ACK, &ack );
		if ( !ok )
		{
			_kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GroupCaptainOffline );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFGroupShardModule::HandleLeaveMatchGroupReq )
	{
		__PROTO_PARSE__( KFMsg::S2SLeaveMatchGroupReq );

		auto strgroupid = __KF_STRING__( kfmsg.groupid() );
		KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] leave group[%s] req!",
			__FUNCTION__, kfmsg.playerid(), strgroupid.c_str() );

		auto kfgroup = _kf_component->FindEntity( kfmsg.groupid(), __FUNCTION_LINE__ );
		if ( kfgroup == nullptr )
		{
			return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] can't find!",
				__FUNCTION__, strgroupid.c_str() );
		}

		auto kfobject = kfgroup->GetData();
		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );
		kfmemberrecord->RemoveData( kfmsg.playerid() );

		{
			// 发送消息
			KFMsg::S2SRemoveMatchGroupMemberAck req;
			req.set_memberid( kfmsg.playerid() );
			SendMessageToGroup( kfmemberrecord, kfmsg.playerid(), KFMsg::S2S_REMOVE_MATCH_GROUP_MEMBER_ACK, &req );
		}

		// 判断队员数量
		if ( kfmemberrecord->Size() == 0 )
		{
			ProcessRemoveGroup( kfmsg.groupid() ); 
		}
		else
		{
			// 判断是否是队长
			auto captainid = kfobject->GetValue< uint32 >( KFField::_captain_id );
			if ( captainid == kfmsg.playerid() )
			{
				ProcessChooseGroupCaptain( kfgroup );
			}
		}
	}

	KFData* KFGroupShardModule::ChooseNewGroupCaptain( KFData* kfmemberrecord )
	{
		auto kfmember = kfmemberrecord->FirstData();
		while ( kfmember != nullptr )
		{
			auto serverid = kfmember->GetValue< uint32 >( KFField::_basic, KFField::_server_id );
			if ( serverid != _invalid_int )
			{
				return kfmember;
			}

			kfmember = kfmemberrecord->NextData();
		}

		return nullptr;
	}

	bool KFGroupShardModule::ProcessChooseGroupCaptain( KFEntity* kfgroup )
	{
		auto kfobject = kfgroup->GetData();
		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );

		auto kfmember = ChooseNewGroupCaptain( kfmemberrecord );
		if ( kfmember == nullptr )
		{
			// 没有新队长
			ProcessRemoveGroup( kfgroup->GetKeyID() );
			return false;
		}

		{
			// 设置新队长
			kfobject->SetValue< uint32 >( KFField::_captain_id, kfmember->GetKeyID() );
			
			// 通知玩家你已经成为队长了
			KFMsg::S2SUpdateGroupCaptainAck ack;
			ack.set_iscaptain( 1 );
			SendMessageToMember( kfmember, KFMsg::S2S_UPDATE_GROUP_CAPTAIN_ACK, &ack );
		}

		{
			// 更新队长消息
			KFMsg::PBStrings pbstrings;
			auto pbstring = pbstrings.add_pbstring();
			pbstring->set_name( KFField::_is_captain );
			pbstring->set_value( "1" );
			SendUpdateMemberToGroup( kfgroup->GetKeyID(), kfmemberrecord, kfmember, "", pbstrings );
		}

		return true;
	}

	void KFGroupShardModule::ProcessRemoveGroup( uint64 groupid )
	{
		// 没有队员, 删除队伍
		_kf_component->RemoveEntity( groupid );

		// 通知proxy
		_kf_cluster_shard->RemoveObjectToProxy( groupid );

		auto strgroupid = __KF_STRING__( groupid );
		KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] disband!",
			__FUNCTION__, strgroupid.c_str() );
	}

	__KF_MESSAGE_FUNCTION__( KFGroupShardModule::HandleKickMatchGroupReq )
	{
		__PROTO_PARSE__( KFMsg::S2SKickMatchGroupReq );

		auto strgroupid = __KF_STRING__( kfmsg.groupid() );
		KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] player[%u] kick member[%u] req!",
			__FUNCTION__, strgroupid.c_str(), kfmsg.captainid(), kfmsg.memberid() );

		auto kfgroup = _kf_component->FindEntity( kfmsg.groupid(), __FUNCTION_LINE__ );
		if ( kfgroup == nullptr )
		{
			_kf_display->SendToGame( kfmsg.serverid(), kfmsg.captainid(), KFMsg::GroupNotExist );
			return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] can't find!",
				__FUNCTION__, strgroupid.c_str() );
		}

		auto kfobject = kfgroup->GetData();
		auto captainid = kfobject->GetValue< uint32 >( KFField::_captain_id );
		if ( captainid != kfmsg.captainid() )
		{
			_kf_display->SendToGame( kfmsg.serverid(), kfmsg.captainid(), KFMsg::GroupNotCaption );
			return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] player[%u] not captainid[%u]!",
				__FUNCTION__, strgroupid.c_str(), kfmsg.captainid(), captainid );
		}

		// 踢掉玩家
		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );
		auto kfkickmember = kfmemberrecord->FindData( kfmsg.memberid() );
		if ( kfkickmember != nullptr )
		{
			// 通知被T的玩家, 你已经离开了队伍
			KFMsg::S2SLeaveMatchGroupAck ack;
			ack.set_playerid( kfkickmember->GetKeyID() );
			SendMessageToMember( kfkickmember, KFMsg::S2S_LEAVE_MATCH_GROUP_ACK, &ack );

			// 删除队员
			kfmemberrecord->RemoveData( kfmsg.memberid() );
		}

		{
			// 通知队员, 有队员离开
			KFMsg::S2SRemoveMatchGroupMemberAck req;
			req.set_memberid( kfmsg.memberid() );
			SendMessageToGroup( kfmemberrecord, kfmsg.memberid(), KFMsg::S2S_REMOVE_MATCH_GROUP_MEMBER_ACK, &req );
		}

		KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] player[%u] kick member[%u] ok!",
			__FUNCTION__, strgroupid.c_str(), kfmsg.captainid(), kfmsg.memberid() );
	}

	__KF_MESSAGE_FUNCTION__( KFGroupShardModule::HandleOnLineQueryGroupReq )
	{
		__PROTO_PARSE__( KFMsg::S2SOnLineQueryMatchGroupReq );

		auto strgroupid = __KF_STRING__( kfmsg.groupid() );
		KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] online query group[%s] req!",
			__FUNCTION__, kfmsg.playerid(), strgroupid.c_str() );

		auto kfgroup = _kf_component->FindEntity( kfmsg.groupid(), __FUNCTION_LINE__ );
		if ( kfgroup == nullptr )
		{
			return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] can't find!",
				__FUNCTION__, strgroupid.c_str() );
		}

		auto kfobject = kfgroup->GetData();
		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );
		auto kfquerymember = kfmemberrecord->FindData( kfmsg.playerid() );
		if ( kfquerymember == nullptr )
		{
			return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] can't find member[%u]!",
				__FUNCTION__, strgroupid.c_str(), kfmsg.playerid() );
		}

		// 上线更新数据
		KFMsg::PBStrings pbstrings;
		{
			auto pbstring = pbstrings.add_pbstring();
			pbstring->set_name( KFField::_server_id );
			pbstring->set_value( KFUtility::ToString< uint32 >( kfmsg.serverid() ) );
		}
		{
			auto pbstring = pbstrings.add_pbstring();
			pbstring->set_name( KFField::_status );
			pbstring->set_value( KFUtility::ToString< uint32 >( KFMsg::StatusEnum::GroupStatus ) );
		}
		SendUpdateMemberToGroup( kfmsg.groupid(), kfmemberrecord, kfquerymember, KFField::_basic, pbstrings );
	
		// 把队员更新玩家
		SendGroupDataToMember( kfmemberrecord, kfquerymember, kfgroup, false );
	}

	void KFGroupShardModule::SendGroupDataToMember( KFData* kfmemberrecord, KFData* kfnewmember, KFEntity* kfgroup, bool newadd )
	{
		auto kfobject = kfgroup->GetData();
		auto captainid = kfobject->GetValue< uint32 >( KFField::_captain_id );
		auto maxcount = kfobject->GetValue< uint32 >( KFField::_max_count );
		auto matchid = kfobject->GetValue< uint32 >( KFField::_match_id );

		KFMsg::S2SAddMatchGroupMemberAck ack;
		ack.set_groupid( kfgroup->GetKeyID() );
		ack.set_maxcount( maxcount );
		ack.set_matchid( matchid );
		ack.set_iscaptain( kfnewmember->GetKeyID() == captainid ? 1 : 0 );
		ack.set_newadd( newadd );

		auto kfmember = kfmemberrecord->FirstData();
		while ( kfmember != nullptr )
		{
			if ( kfmember != kfnewmember )
			{
				_kf_kernel->SerializeToClient( kfmember, ack.add_pbmember() );
			}
			kfmember = kfmemberrecord->NextData();
		}
		SendMessageToMember( kfnewmember, KFMsg::S2S_ADD_MATCH_GROUP_MEMBER_ACK, &ack );
	}

	__KF_MESSAGE_FUNCTION__( KFGroupShardModule::HandleOffLineUpdateGroupReq )
	{
		__PROTO_PARSE__( KFMsg::S2SOffLineUpdateMatchGroupReq );

		auto strgroupid = __KF_STRING__( kfmsg.groupid() );
		KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u] offline update group[%s] req!",
			__FUNCTION__, kfmsg.playerid(), strgroupid.c_str() );

		auto kfgroup = _kf_component->FindEntity( kfmsg.groupid(), __FUNCTION_LINE__ );
		if ( kfgroup == nullptr )
		{
			return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] can't find!",
				__FUNCTION__, strgroupid.c_str() );
		}

		auto kfobject = kfgroup->GetData();
		auto captainid = kfobject->GetValue< uint32 >( KFField::_captain_id );
		if ( kfmsg.playerid() == captainid )
		{
			auto ok = ProcessChooseGroupCaptain( kfgroup );
			if ( !ok )
			{
				return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] disband!",
					__FUNCTION__, strgroupid.c_str() );
			}
		}
		
		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );
		auto kfmember = kfmemberrecord->FindData( kfmsg.playerid() );
		if ( kfmember == nullptr )
		{
			return KFLogger::LogLogic( KFLogger::Debug, "[%s] group[%s] can't find member[%u]!",
				__FUNCTION__, strgroupid.c_str(), kfmsg.playerid() );
		}

		// 离线更新数据
		static KFMsg::PBStrings pbstrings;
		if ( pbstrings.pbstring_size() == 0 )
		{
			auto pbstring = pbstrings.add_pbstring();
			pbstring->set_name( KFField::_server_id );
			pbstring->set_value( "0" );

			pbstring = pbstrings.add_pbstring();
			pbstring->set_name( KFField::_status );
			pbstring->set_value( KFUtility::ToString< uint32 >( KFMsg::StatusEnum::OfflineStatus ) );
			
			pbstring = pbstrings.add_pbstring();
			pbstring->set_name( KFField::_is_captain );
			pbstring->set_value( "0" );

			pbstring = pbstrings.add_pbstring();
			pbstring->set_name( KFField::_prepare );
			pbstring->set_value( "0" );
		}

		SendUpdateMemberToGroup( kfmsg.groupid(), kfmemberrecord, kfmember, KFField::_basic, pbstrings );
	}

	__KF_MESSAGE_FUNCTION__( KFGroupShardModule::HandleUpdateGroupMemberReq )
	{
		__PROTO_PARSE__( KFMsg::S2SUpdateGroupMemberReq );

		auto kfgroup = _kf_component->FindEntity( kfmsg.groupid(), __FUNCTION_LINE__ );
		if ( kfgroup == nullptr )
		{
			return;
		}

		auto kfobject = kfgroup->GetData();
		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );
		auto kfmember = kfmemberrecord->FindData( kfmsg.memberid() );
		if ( kfmember == nullptr )
		{
			return;
		}

		SendUpdateMemberToGroup( kfmsg.groupid(), kfmemberrecord, kfmember, kfmsg.dataname(), kfmsg.pbstrings() );
	}

	void KFGroupShardModule::SendUpdateMemberToGroup( uint64 groupid, KFData* kfmemberrecord, KFData* kfmember, const std::string& dataname, const KFMsg::PBStrings& pbstrings )
	{
		auto kfupdate = kfmember->FindData( dataname );
		if ( kfupdate == nullptr )
		{
			kfupdate = kfmember;
		}

		for ( auto i = 0; i < pbstrings.pbstring_size(); ++i )
		{
			auto pbstring = &pbstrings.pbstring( i );

			auto kfdata = kfupdate->FindData( pbstring->name() );
			if ( kfdata != nullptr )
			{
				kfdata->FromString( pbstring->name() );
			}
		}
				
		KFMsg::S2SUpdateGroupMemberAck ack;
		ack.set_memberid( kfmember->GetKeyID() );
		ack.set_dataname( dataname );
		ack.mutable_pbstrings()->CopyFrom( pbstrings );
		SendMessageToGroup( kfmemberrecord, kfmember->GetKeyID(), KFMsg::S2S_UPDATE_GROUP_MEMBER_ACK, &ack );
	}

	__KF_MESSAGE_FUNCTION__( KFGroupShardModule::HandleUpdateGroupMatchReq )
	{
		__PROTO_PARSE__( KFMsg::S2SUpdateGroupMatchReq );

		auto kfgroup = _kf_component->FindEntity( kfmsg.groupid(), __FUNCTION_LINE__ );
		if ( kfgroup == nullptr )
		{
			return;
		}

		auto kfobject = kfgroup->GetData();
		kfobject->SetValue< uint32 >( KFField::_match_id, kfmsg.matchid() );
		kfobject->SetValue< uint32 >( KFField::_max_count, kfmsg.maxcount() );

		// 同步给客户端
		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );

		KFMsg::S2SUpdateGroupMatchAck ack;
		ack.set_matchid( kfmsg.matchid() );
		ack.set_maxcount( kfmsg.maxcount() );
		SendMessageToGroup( kfmemberrecord, kfmsg.playerid(), KFMsg::S2S_UPDATE_GROUP_MATCH_ACK, &ack );
	}
}