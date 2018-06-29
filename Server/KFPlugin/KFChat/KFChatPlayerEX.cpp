#include "KFChatPlayerEX.h"

namespace KFrame
{
    KFChatPlayerEX::KFChatPlayerEX()
    {
        _chat_type = 0;
        _player_id = 0;
        _gate_id = 0;
        _last_chat_time = 0;
    }

    KFChatPlayerEX::~KFChatPlayerEX()
    {
    }

    void KFChatPlayerEX::SetPlayerId( uint32 playerid )
    {
        _player_id = playerid;
    }

    uint32 KFChatPlayerEX::GetPlayerId()
    {
        return _player_id;
    }

    void KFChatPlayerEX::SetGateId( uint32 gateid )
    {
        _gate_id = gateid;
    }

    uint32 KFChatPlayerEX::GetGateId()
    {
        return _gate_id;
    }

    void KFChatPlayerEX::SetChatType( uint32 chattype )
    {
        _chat_type = chattype;
    }

    uint32 KFChatPlayerEX::GetChatType()
    {
        return _chat_type;
    }

    void KFChatPlayerEX::SetLastChatTime( uint64 lasttime )
    {
        _last_chat_time = lasttime;
    }

    uint64 KFChatPlayerEX::GetLastChatTime()
    {
        return _last_chat_time;
    }
}
