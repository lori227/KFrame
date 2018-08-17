#include "KFGameModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFGameModule::KFGameModule()
    {
        _world_server_id = _invalid_int;
    }

    KFGameModule::~KFGameModule()
    {

    }

    void KFGameModule::InitModule()
    {
    }

    void KFGameModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFGameModule::OnClientLostServer );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFGameModule::OnClientConnectionServer );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_MESSAGE_REQ, &KFGameModule::HandleBroadcastMessageReq );

    }

    void KFGameModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_MESSAGE_REQ );
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

    bool KFGameModule::SendMessageToWorld( uint32 msgid, ::google::protobuf::Message* message )
    {
        if ( _world_server_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_client->SendNetMessage( _world_server_id, msgid, message );
    }

    void KFGameModule::SendMessageToGate( uint32 msgid, ::google::protobuf::Message* message )
    {
        _kf_tcp_server->SendNetMessage( msgid, message );
    }

    // 发送消息到Gate服务器
    bool KFGameModule::SendMessageToGate( uint32 gateid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, msgid, message );
    }

    // 发送消息到客户端
    bool KFGameModule::SendMessageToClient( uint32 gateid, uint32 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gateid, playerid, msgid, message );
    }

    bool KFGameModule::SendMessageToClient( uint32 gateid, uint32 playerid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_tcp_server->SendNetMessage( gateid, playerid, msgid, data, length );
    }

    bool KFGameModule::BroadcastMessageToClient( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastMessageReq req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_MESSAGE_REQ, &req );
        return true;
    }

    bool KFGameModule::BroadcastMessageToWorld( uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2SBroadcastMessageReq req;
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        SendMessageToWorld( KFMsg::S2S_BROADCAST_MESSAGE_REQ, &req );
        return true;
    }

    __KF_MESSAGE_FUNCTION__( KFGameModule::HandleBroadcastMessageReq )
    {
        __PROTO_PARSE__( KFMsg::S2SBroadcastMessageReq );
        _kf_tcp_server->SendNetMessage( KFMsg::S2S_BROADCAST_MESSAGE_REQ, &kfmsg );
    }

    bool KFGameModule::SendMessageToTransmit( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        KFMsg::S2STransmitMessageReq req;
        req.set_playerid( playerid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendMessageToWorld( KFMsg::S2S_TRANSMIT_MESSAGE_REQ, &req );
    }


}
