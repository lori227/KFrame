#ifndef __KF_WORLD_ONLINE_H__
#define __KF_WORLD_ONLINE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFOnline
    {
    public:
        KFOnline() = default;
        ~KFOnline() = default;

        // 发送消息到Game
        void SendToOnline( uint32 msgid, const char* data, uint32 length );
        void SendToOnline( uint32 msgid, ::google::protobuf::Message* message );

    public:
        // playerid
        uint64 _player_id{ 0 };

        // gameid
        uint64 _game_id{ 0 };

        // accountid
        uint64 _account_id{ 0 };
    };
}

#endif