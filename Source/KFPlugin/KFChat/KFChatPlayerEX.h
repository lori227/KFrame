#ifndef __KF_CHAT_PLAYER_EX_H__
#define __KF_CHAT_PLAYER_EX_H__

#include "KFChatInterface.h"

namespace KFrame
{
	class KFChatPlayerEX
	{
	public:
		KFChatPlayerEX();
		~KFChatPlayerEX();

		void SetPlayerId( uint32 playerid );
		uint32 GetPlayerId();
		void SetGateId( uint32 gateid );
		uint32 GetGateId();
		void SetChatType( uint32 chattype );
		uint32 GetChatType();
		void SetLastChatTime( uint64 lasttime );
		uint64 GetLastChatTime();

	private:
		//聊天室类型
		uint32 _chat_type;

		//playerid
		uint32 _player_id;

		//gateid
		uint32 _gate_id;

		//上次发言时间
		uint64 _last_chat_time;
	};


}


#endif
