#include "KFOnlineEx.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
	KFOnlineEx::KFOnlineEx()
	{
		_player_id = 0;
		_game_id = 0;
		_account_id = 0;
	}

	KFOnlineEx::~KFOnlineEx()
	{

	}

	// useid
	void KFOnlineEx::SetPlayerId( uint32 playerid )
	{
		_player_id = playerid;
	}

	uint32 KFOnlineEx::GetPlayerId()
	{
		return _player_id;
	}

	// gameid
	void KFOnlineEx::SetGameId( uint32 gameid )
	{
		_game_id = gameid;
	}

	uint32 KFOnlineEx::GetGameId()
	{
		return _game_id;
	}

	void KFOnlineEx::SetAccountId( uint32 accountid )
	{
		_account_id = accountid;
	}

	uint32 KFOnlineEx::GetAccountId()
	{
		return _account_id;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////
	void KFOnlineEx::SendMessageToOnline( uint32 msgid, const char* data, uint32 length )
	{
		_kf_tcp_server->SendNetMessage( _game_id, _player_id, msgid, data, length );
	}

	void KFOnlineEx::SendMessageToOnline( uint32 msgid, ::google::protobuf::Message* message )
	{
		_kf_tcp_server->SendNetMessage( _game_id, _player_id, msgid, message );
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////

}