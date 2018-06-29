#ifndef __KF_ONLINE_EX_H__
#define __KF_ONLINE_EX_H__

#include "KFWorldInterface.h"

namespace KFrame
{
    class KFOnlineEx : public KFOnline
    {
    public:
        KFOnlineEx();
        virtual ~KFOnlineEx();

        // useid
        virtual void SetPlayerId( uint32 playerid );
        virtual uint32 GetPlayerId();

        // gameid
        virtual void SetGameId( uint32 gameid );
        virtual uint32 GetGameId();

        // accountid
        virtual void SetAccountId( uint32 accountid );
        virtual uint32 GetAccountId();
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息到Game
        virtual void SendMessageToOnline( uint32 msgid, const char* data, uint32 length );
        virtual void SendMessageToOnline( uint32 msgid, ::google::protobuf::Message* message );
    private:
        // playerid
        uint32 _player_id;

        // gameid
        uint32 _game_id;

        // accountid
        uint32 _account_id;
    };
}


#endif