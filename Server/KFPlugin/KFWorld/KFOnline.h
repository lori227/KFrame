#ifndef __KF_WORLD_ONLINE_H__
#define __KF_WORLD_ONLINE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFOnline
    {
    public:
        KFOnline()
        {
            _game_id = 0;
            _player_id = 0;
            _account_id = 0;
        }

        virtual ~KFOnline() = default;

        // 发送消息到Game
        virtual void SendToOnline( uint32 msgid, const char* data, uint32 length ) = 0;
        virtual void SendToOnline( uint32 msgid, ::google::protobuf::Message* message ) = 0;

    public:
        // playerid
        uint64 _player_id;

        // gameid
        uint64 _game_id;

        // accountid
        uint64 _account_id;
    };
}

#endif