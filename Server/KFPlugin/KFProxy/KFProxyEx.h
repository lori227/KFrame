#ifndef __KF_PROXY_EX_H__
#define __KF_PROXY_EX_H__

#include "KFProxyInterface.h"

namespace KFrame
{
	class KFProxyEx : public KFProxy
	{
	public:
		KFProxyEx();
		virtual ~KFProxyEx();
	
		// useid
		virtual void SetPlayerId( uint32 playerid );
		virtual uint32 GetPlayerId();

		// gameid
		virtual void SetGameId( uint32 gameid );
		virtual uint32 GetGameId();

		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////

		// 发送消息到Client
		virtual bool SendMessageToClient( uint32 msgid, const char* data, uint32 length );
		virtual bool SendMessageToClient( uint32 msgid, ::google::protobuf::Message* message );

		// 发送消息到Game
		virtual bool SendMessageToGame( uint32 msgid, const char* data, uint32 length );
		virtual bool SendMessageToGame( uint32 msgid, ::google::protobuf::Message* message );
	private:
		// playerid
		uint32 _player_id;

		// gameid
		uint32 _game_id;

	};
}


#endif