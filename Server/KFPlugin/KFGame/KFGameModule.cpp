#include "KFGameModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFGameModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFGameModule::OnClientLostServer );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFGameModule::OnClientConnectionServer );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME, &KFGameModule::HandleBroadcastMessageReq );

    }

    void KFGameModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GAME );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFGameModule::OnClientConnectionServer )
    {
        if ( servertype == __KF_STRING__( world ) )
        {
            _world_server_id = serverid;
        }
    }

    __KF_CLIENT_LOST_FUNCTION__( KFGameModule::OnClientLostServer )
    {
        if ( servertype == __KF_STRING__( world ) )
        {
            _world_server_id = _invalid_int;
        }
    }

    bool KFGameModule::SendToWorld( uint32 msgid, ::google::protobuf::Message* message )
    {
        if ( _world_server_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _world_server_id, msgid, message );
    }

    void KFGameModule::SendToGate( uint32 msgid, ::google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msgid, message );
    }

    // 发送消息到Gate服务器
    bool KFGameModule::SendToGate( uint64 gateid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, msgid, message );
    }

    // 发送消息到客户端
    bool KFGameModule::SendToClient( uint64 gateid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, playerid, msgid, message );
    }

    bool KFGameModule::SendToClient( uint64 gateid, uint64 playerid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_server->SendNetMessage( gateid, playerid, msgid, data, length );
    }

    bool KFGameModule::BroadcastToGate( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToGate req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_TO_GATE, &req );
        return true;
    }

    bool KFGameModule::BroadcastToWorld( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastToWord req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        SendToWorld( KFMsg::S2S_BROADCAST_TO_WORLD, &req );
        return true;
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleBroadcastMessageReq )
    {
        __PROTO_PARSE__( KFMsg::S2SBroadcastToGame );

        KFMsg::S2SBroadcastToGate req;
        req.set_msgid( kfmsg.msgid() );
        req.set_msgdata( kfmsg.msgdata() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_TO_GATE, &req );
    }

    bool KFGameModule::TransmitToPlayer( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2STransmitToPlayer req;
        req.set_playerid( playerid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendToWorld( KFMsg::S2S_TRANSMIT_TO_PLAYER, &req );
    }

    // 转发服务器
    bool KFGameModule::TransmitToServer( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2STransmitToServer req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendToWorld( KFMsg::S2S_TRANSMIT_TO_SERVER, &req );
    }

}
