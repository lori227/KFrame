#ifndef __KF_GATE_ROLE_H__
#define __KF_GATE_ROLE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRole
    {
    public:
        KFRole()
        {
            _role_id = 0;
            _game_id = 0;
            _session_id = 0;
        }

        virtual ~KFRole() = default;

        //////////////////////////////////////////////////////////////////////////////////////////
        // 发送消息到Client
        virtual bool SendMessageToClient( uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendMessageToClient( uint32 msgid, ::google::protobuf::Message* message ) = 0;

        // 发送消息到Game
        virtual bool SendMessageToGame( uint32 msgid, const char* data, uint32 length ) = 0;
        virtual bool SendMessageToGame( uint32 msgid, ::google::protobuf::Message* message ) = 0;

    public:
        // 角色id( playerid )
        uint32 _role_id;

        // game游戏服务器id
        uint32 _game_id;

        // 连接id
        uint32 _session_id;

        // 登录token
        std::string _token;

        // ip地址
        std::string _ip;
    };
}



#endif