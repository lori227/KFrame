#include "KFProxyModule.h"
#include "KFProxyEx.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
	KFProxyModule::KFProxyModule()
	{
	}

	KFProxyModule::~KFProxyModule()
	{

	}

	void KFProxyModule::InitModule()
	{

	}

	void KFProxyModule::BeforeRun()
	{
		_kf_tcp_server->RegisterLostFunction( this, &KFProxyModule::OnPlayerDisconnection );
		_kf_tcp_client->RegisterTransmitFunction( this, &KFProxyModule::SendMessageToClient );
		_kf_tcp_server->RegisterTransmitFunction( this, &KFProxyModule::SendMessageToGame );

		__REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_MESSAGE_REQ, &KFProxyModule::HandleBroadcastMessageReq );
		__REGISTER_MESSAGE__( KFMsg::S2S_KICK_PROXY_PLAYER_REQ, &KFProxyModule::HandleKickProxyPlayerReq );
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	void KFProxyModule::BeforeShut()
	{
		_kf_tcp_server->UnRegisterLostFunction( this );
		_kf_tcp_client->UnRegisterTransmitFunction( this );
		_kf_tcp_server->UnRegisterTransmitFunction( this );
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_MESSAGE_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_PROXY_PLAYER_REQ );
	}

	KFProxy* KFProxyModule::CreateProxy( uint32 playerid )
	{
		auto kfproxy = __KF_CREATE__( KFProxyEx );
		kfproxy->SetPlayerId( playerid );

		_proxy_list.Insert( playerid, kfproxy );
		return kfproxy;
	}

	KFProxy* KFProxyModule::FindProxy( uint32 playerid )
	{
		return _proxy_list.Find( playerid );
	}

	bool KFProxyModule::RemoveProxy( uint32 playerid )
	{
		return _proxy_list.Remove( playerid );
	}

	__KF_SERVER_LOST_FUNCTION__( KFProxyModule::OnPlayerDisconnection )
	{
		KFLogger::LogLogin( KFLogger::Info, "[%s] client[%u] disconnection!", 
			__FUNCTION__, handleid );

		KFMsg::S2SPlayerDisconnectionReq req;
		req.set_playerid( handleid );

		auto kfproxy = FindProxy( handleid );
		if ( kfproxy == nullptr )
		{
			return;
		}

		kfproxy->SendMessageToGame( KFMsg::S2S_PLAYER_DISCONNECTION_REQ, &req );

		// 删除玩家
		RemoveProxy( handleid );
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_MAX_CLIENT_MSG_ID__ 10000
	__KF_TRANSMIT_FUNCTION__( KFProxyModule::SendMessageToClient )
	{
		auto playerid = __KF_DATA_ID__( kfguid );
		if ( msgid == _invalid_int || msgid >= __KF_MAX_CLIENT_MSG_ID__ )
		{
			KFLogger::LogLogic( KFLogger::Error, "[%s] client[%u] msgid[%u] length[%u] error!",
				__FUNCTION__, playerid, msgid, length );
			return false;
		}

		auto kfproxy = FindProxy( playerid );
		if ( kfproxy != nullptr )
		{
			kfproxy->SendMessageToClient( msgid, data, length );
		}

		return true;
	}

	__KF_TRANSMIT_FUNCTION__( KFProxyModule::SendMessageToGame )
	{
		auto playerid = __KF_HEAD_ID__( kfguid );
		if ( msgid == _invalid_int || msgid >= __KF_MAX_CLIENT_MSG_ID__ )
		{
			KFLogger::LogLogic( KFLogger::Error, "[%s] client[%u] msgid[%u] length[%u] error!", 
				__FUNCTION__, playerid, msgid, length );
			return false;
		}

		auto kfproxy = FindProxy( playerid );
		if ( kfproxy != nullptr )
		{
			kfproxy->SendMessageToGame( msgid, data, length );
		}

		return true;
	}

	__KF_MESSAGE_FUNCTION__( KFProxyModule::HandleBroadcastMessageReq )
	{
		__PROTO_PARSE__( KFMsg::S2SBroadcastMessageReq );

		auto msgid = kfmsg.msgid();
		auto& msgdata = kfmsg.msgdata();

		for ( auto& iter : _proxy_list._objects )
		{
			auto kfproxy = iter.second;
			kfproxy->SendMessageToClient( msgid, msgdata.data(), static_cast< uint32 >( msgdata.length() ) );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFProxyModule::HandleKickProxyPlayerReq )
	{
		__PROTO_PARSE__( KFMsg::S2SKickProxyPlayerReq );

		auto playerid = kfmsg.playerid();
		auto kfproxy = FindProxy( playerid );
		if ( kfproxy == nullptr )
		{
			return;
		}

		// 通知客户端你被踢了
		KFMsg::MsgTellBeKick kick;
		kick.set_type( kfmsg.type() );
		kfproxy->SendMessageToClient( KFMsg::MSG_TELL_BE_KICK, &kick );

		// 删除内存
		RemoveProxy( playerid );

		// 删除链接
		_kf_gate->RemoveConnection( playerid, 100, __FUNCTION_LINE__ );
	}
}
