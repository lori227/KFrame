#include "KFProxyEx.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    KFProxyEx::KFProxyEx()
    {
        _player_id = 0;
        _game_id = 0;
    }

    KFProxyEx::~KFProxyEx()
    {

    }

    // useid
    void KFProxyEx::SetPlayerId( uint32 playerid )
    {
        _player_id = playerid;
    }

    uint32 KFProxyEx::GetPlayerId()
    {
        return _player_id;
    }

    // gameid
    void KFProxyEx::SetGameId( uint32 gameid )
    {
        _game_id = gameid;
    }

    uint32 KFProxyEx::GetGameId()
    {
        return _game_id;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool KFProxyEx::SendMessageToClient( uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_server->SendNetMessage( _player_id, msgid, data, length );
    }

    bool KFProxyEx::SendMessageToClient( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( _player_id, msgid, message );
    }

    bool KFProxyEx::SendMessageToGame( uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_client->SendNetMessage( _game_id, _player_id, msgid, data, length );
    }

    bool KFProxyEx::SendMessageToGame( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_client->SendNetMessage( _game_id, _player_id, msgid, message );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

}