#include "KFMatchClientModule.h"
#include "KFMatchClientConfig.h"

namespace KFrame
{
	KFMatchClientModule::KFMatchClientModule()
	{
	}

	KFMatchClientModule::~KFMatchClientModule()
	{
	}

	void KFMatchClientModule::InitModule()
	{
		_kf_config->AddConfig( _kf_match_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
		///////////////////////////////////////////////////////////////////////////////
	}

	void KFMatchClientModule::BeforeRun()
	{
		_kf_player->RegisterEnterFunction( this, &KFMatchClientModule::OnEnterQueryMatchRoom );
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__REGISTER_MESSAGE__( KFMsg::MSG_START_MATCH_REQ, &KFMatchClientModule::HandleStartMatchReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_MATCH_TO_CLIENT_ACK, &KFMatchClientModule::HanldeMatchToClientAck );
		__REGISTER_MESSAGE__( KFMsg::MSG_CANCEL_MATCH_REQ, &KFMatchClientModule::HandleCancelMatchReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_QUERY_MATCH_ROOM_ACK, &KFMatchClientModule::HandleQueryMatchRoomAck );
		
	}

	void KFMatchClientModule::OnceRun()
	{

	}

	void KFMatchClientModule::BeforeShut()
	{
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
		_kf_player->UnRegisterEnterFunction( this );
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__UNREGISTER_MESSAGE__( KFMsg::MSG_START_MATCH_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_MATCH_TO_CLIENT_ACK );
		__UNREGISTER_MESSAGE__( KFMsg::MSG_CANCEL_MATCH_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_MATCH_ROOM_ACK );
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool KFMatchClientModule::SendMessageToMatch( uint32 msgid, ::google::protobuf::Message* message )
	{
		return _kf_cluster->SendMessageToShard( KFField::_match, msgid, message );
	}

	uint32 KFMatchClientModule::GetMatchMaxCount( uint32 matchid )
	{
		// 判断匹配是否存在
		auto kfsetting = _kf_match_config->FindMatchSetting( matchid );
		if ( kfsetting == nullptr )
		{
			return 1;
		}

		return kfsetting->_max_group_player_count;
	}

	__KF_MESSAGE_FUNCTION__( KFMatchClientModule::HandleStartMatchReq )
	{
		__CLIENT_PROTO_PARSE__( KFMsg::MsgStartMatchReq );

		// 开始匹配
		auto result = ProcessStartMatch( player, kfmsg.matchid(), kfmsg.allowgroup() );
		if ( result != KFMsg::MatchRequestSuccess )
		{
			_kf_display->SendToGroup( player, result );
		}
	}

	bool KFMatchClientModule::IsAllGroupMemberPrepare( KFData* kfmemberrecord )
	{
		auto kfmember = kfmemberrecord->FirstData();
		while ( kfmember != nullptr )
		{
			auto isparpre = kfmember->GetValue< uint32 >( KFField::_prepare );
			if ( isparpre == _invalid_int )
			{
				return false;
			}

			kfmember = kfmemberrecord->NextData();
		}

		return true;
	}

	uint32 KFMatchClientModule::ProcessStartMatch( KFEntity* player, uint32 matchid, bool allowgroup )
	{
		// 判断匹配是否存在
		auto kfsetting = _kf_match_config->FindMatchSetting( matchid );
		if ( kfsetting == nullptr )
		{
			return KFMsg::MatchCanNotFindMatch;
		}

		// 是否正在匹配中
		auto kfobject = player->GetData();
		auto waitmatchid = kfobject->GetValue< uint32 >( KFField::_match_id );
		if ( waitmatchid != _invalid_int )
		{
			return KFMsg::MatchAlreadyWait;
		}

		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );
		auto membercount = kfmemberrecord->Size() + 1;

		// 判断该模式的人数限定
		if ( kfsetting->_min_group_player_count > membercount ||
			kfsetting->_max_group_player_count < membercount )
		{
			return KFMsg::MatchGroupPlayerLimit;
		}
		
		// 判断是否都准备
		if ( !IsAllGroupMemberPrepare( kfmemberrecord ) )
		{
			return KFMsg::MatchMustPrepare;
		}

		// 发送给匹配集群， 进行匹配
		KFMsg::S2SMatchToProxyReq req;
		req.set_matchid( matchid );
		req.set_allowgroup( allowgroup );
		req.set_playerid( player->GetKeyID() );
		req.set_serverid( KFGlobal::Instance()->_app_id );
		FormatMatchGroup( player, req.mutable_pbgroup() );
		auto ok = SendMessageToMatch( KFMsg::S2S_MATCH_TO_PROXY_REQ, &req );
		if ( !ok )
		{
			return KFMsg::MatchClusterBusy;
		}

		// 设置正在等待匹配中, 防止客户端请求多次
		kfobject->SetValue< uint32 >( KFField::_match_id, matchid );
		return KFMsg::MatchRequestSuccess;
	}

	void KFMatchClientModule::FormatMatchGroup( KFEntity* player, KFMsg::PBMatchGroup* pbgroup )
	{
		auto kfobject = player->GetData();
		auto groupid = kfobject->GetValue< uint64 >( KFField::_basic, KFField::_group_id );
		if ( groupid == _invalid_int )
		{
			groupid = kfobject->GetKeyID();
		}
		pbgroup->set_groupid( groupid );

		// 自己的属性
		FormatMatchPlayer( kfobject, pbgroup->add_pbplayer() );

		// 队员的属性
		auto kfmemberrecord = kfobject->FindData( KFField::_group_member );
		auto kfmember = kfmemberrecord->FirstData();
		while ( kfmember != nullptr )
		{
			FormatMatchPlayer( kfmember, pbgroup->add_pbplayer() );
			kfmember = kfmemberrecord->NextData();
		}
	}
	
	void KFMatchClientModule::FormatMatchPlayer( KFData* kfobject, KFMsg::PBBattlePlayer* pbplayer )
	{
		auto kfbasic = kfobject->FindData( KFField::_basic );
		pbplayer->set_serverid( kfbasic->GetValue< uint32 >( KFField::_server_id ) );
		pbplayer->set_playerid( kfbasic->GetValue< uint32 >( KFField::_id ) );

		pbplayer->set_sex( kfbasic->GetValue< uint32 >( KFField::_sex ) );
		pbplayer->set_name( kfbasic->GetValue< std::string >( KFField::_name ) );
		pbplayer->set_icon( kfbasic->GetValue< std::string >( KFField::_icon ) );
		pbplayer->set_iconbox( kfbasic->GetValue< std::string >( KFField::_icon_box ) );
		pbplayer->set_grading( kfbasic->GetValue< uint32 >( KFField::_grading ) );
		pbplayer->set_groupid( kfbasic->GetValue< uint32 >( KFField::_group_id ) );

		// 模型, 时装
		pbplayer->set_modelid( kfobject->GetValue< uint32 >( KFField::_model_id ) );
		pbplayer->set_clothesid( kfobject->GetValue< uint32 >( KFField::_clothes_id ) );
	}

	__KF_MESSAGE_FUNCTION__( KFMatchClientModule::HandleCancelMatchReq )
	{
		__CLIENT_PROTO_PARSE__( KFMsg::MsgCancelMatchReq );

		auto kfobject = player->GetData();
		uint32 matchid = kfobject->GetValue< uint32 >( KFField::_match_id );
		if ( matchid == _invalid_int )
		{
			return _kf_display->SendToClient( player, KFMsg::MatchNotInMatch );
		}

		player->UpdateData( KFField::_match_id, KFOperateEnum::Set, _invalid_int );
		
		{
			_kf_display->SendToClient( player, KFMsg::MatchCancelSuccess );

			//// 发送给队伍
			//KFMsg::MsgCancelMatchAck ack;
			//ack.set_matchid( matchid );
			//_kf_player->SendMessageToGroup( player, KFMsg::MSG_CANCEL_MATCH_ACK, &ack );
		}

		{
			// 发送取消到匹配集群中
			KFMsg::S2SCancelMatchToProxyReq req;
			req.set_matchid( matchid );
			req.set_playerid( playerid );
			SendMessageToMatch( KFMsg::S2S_CANCEL_MATCH_TO_PROXY_REQ, &req );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFMatchClientModule::HanldeMatchToClientAck )
	{
		__SERVER_PROTO_PARSE__( KFMsg::S2SMatchToClientAck );

		_kf_display->SendToGroup( player, kfmsg.result() );
		if ( kfmsg.result() != KFMsg::MatchRequestSuccess )
		{
			player->UpdateData( KFField::_match_id, KFOperateEnum::Set, _invalid_int );
		}

		//KFMsg::MsgStartMatchAck ack;
		//ack.set_matchid( kfmsg.matchid() );
		//_kf_player->SendMessageToGroup( player, KFMsg::MSG_START_MATCH_ACK, &ack );
	}
	
	void KFMatchClientModule::OnEnterQueryMatchRoom( KFEntity* player )
	{
		auto kfobject = player->GetData();
		auto matchid = kfobject->GetValue< uint32 >( KFField::_match_id );
		if ( matchid == _invalid_int )
		{
			return;
		}

		auto roomid = kfobject->GetValue< uint64 >( KFField::_room_id );
		if ( roomid != _invalid_int )
		{
			return;
		}

		KFMsg::S2SQueryMatchRoomReq req;
		req.set_matchid( matchid );
		req.set_playerid( player->GetKeyID() );
		req.set_serverid( KFGlobal::Instance()->_app_id );
		SendMessageToMatch( KFMsg::S2S_QUERY_MATCH_ROOM_REQ, &req );
	
		kfobject->SetValue< uint32 >( KFField::_match_id, _invalid_int );
	}

	__KF_MESSAGE_FUNCTION__( KFMatchClientModule::HandleQueryMatchRoomAck )
	{
		__SERVER_PROTO_PARSE__( KFMsg::S2SQueryMatchRoomAck );

		player->UpdateData( KFField::_match_id, KFOperateEnum::Set, kfmsg.matchid() );
	}
}