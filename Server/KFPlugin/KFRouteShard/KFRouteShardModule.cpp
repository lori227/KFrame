#include "KFRouteShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRouteShardModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFRouteShardModule::OnServerLostRouteProxy );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_ALL_REQ, &KFRouteShardModule::HandleRouteMessageToAllReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &KFRouteShardModule::HandleRouteMessageToNameAllReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &KFRouteShardModule::HandleRouteMessageToNameRandReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &KFRouteShardModule::HandleRouteMessageToNameBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &KFRouteShardModule::HandleRouteMessageToNameObjectReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &KFRouteShardModule::HandleRouteMessageToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ, &KFRouteShardModule::HandleRouteMessageToPlayerReq );
    }

    void KFRouteShardModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_ALL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFRouteShardModule::OnServerLostRouteProxy )
    {

    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToAllReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToAllReq );

        KFMsg::S2SRouteMessageToClientAck ack;
        ack.set_sourceid( kfmsg.sourceid() );
        ack.set_msgid( kfmsg.msgid() );
        ack.set_msgdata( kfmsg.msgdata() );
        _kf_cluster_shard->SendToClient( KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameAllReq )
    {

    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameRandReq )
    {

    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameBalanceReq )
    {

    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameObjectReq )
    {

    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToServerReq );

        SendRouteMessage( kfmsg.targetid(), kfmsg.msgid(), kfmsg.msgdata(), kfmsg.sourceid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToPlayerReq );

        SendRouteMessage( kfmsg.targetid(), kfmsg.msgid(), kfmsg.msgdata(), kfmsg.sourceid(), kfmsg.playerid() );
    }

    void KFRouteShardModule::SendRouteMessage( uint64 clientid, uint32 msgid, const std::string& msgdata, uint64 sourceid, uint64 playerid /* = _invalid_int */ )
    {
        KFMsg::S2SRouteMessageToClientAck ack;
        ack.set_sourceid( sourceid );
        ack.set_playerid( playerid );
        ack.set_msgid( msgid );
        ack.set_msgdata( msgdata );
        _kf_cluster_shard->SendToClient( clientid, KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK, &ack );
    }
}
