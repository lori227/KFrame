#ifndef __KF_GATE_ROLE_H__
#define __KF_GATE_ROLE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRole
    {
    public:
        KFRole() = default;
        ~KFRole() = default;

        //////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息到Client
        bool SendToClient( uint32 msgid, const char* data, uint32 length );
        bool SendToClient( uint32 msgid, ::google::protobuf::Message* message );

        // 发送消息到Game
        bool SendToGame( uint32 msgid, const char* data, uint32 length );
        bool SendToGame( uint32 msgid, ::google::protobuf::Message* message );
    public:
        // 角色id( playerid )
        uint64 _id = _invalid_int;

        // game游戏服务器id
        uint64 _game_id = _invalid_int;

        // 连接id
        uint64 _session_id = _invalid_int;

        // 账号id
        uint64 _account_id = _invalid_int;

        // 登录token
        std::string _token;

        // ip地址
        std::string _ip;
    };
}



#endif