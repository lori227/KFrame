#include "KFRoleEx.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"

namespace KFrame
{
    // 发送消息到Client
    bool KFRoleEx::SendToClient( uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_server->SendNetMessage( _session_id, msgid, data, length );
    }

    bool KFRoleEx::SendToClient( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( _session_id, msgid, message );
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // 发送消息到Game
    bool KFRoleEx::SendMessageToGame( uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_client->SendNetMessage( _game_id, _role_id, msgid, data, length );
    }

    bool KFRoleEx::SendMessageToGame( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_client->SendNetMessage( _game_id, _role_id, msgid, message );
    }
}

