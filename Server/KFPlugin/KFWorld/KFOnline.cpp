﻿#include "KFOnline.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    void KFOnline::SendToOnline( uint32 msgid, const char* data, uint32 length )
    {
        _kf_tcp_server->SendNetMessage( _game_id, _player_id, msgid, data, length );
    }

    void KFOnline::SendToOnline( uint32 msgid, ::google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( _game_id, _player_id, msgid, message );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

}