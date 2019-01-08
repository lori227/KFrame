#include "KFRouteShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRouteShardModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &KFRouteShardModule::HandleRouteMessageToNameAllReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &KFRouteShardModule::HandleRouteMessageToNameRandReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &KFRouteShardModule::HandleRouteMessageToNameBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &KFRouteShardModule::HandleRouteMessageToNameObjectReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &KFRouteShardModule::HandleRouteMessageToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ, &KFRouteShardModule::HandleRouteMessageToPlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_CLIENT_LOST_TO_SHARD_REQ, &KFRouteShardModule::HandleRouteClientLostToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_SHARD_REQ, &KFRouteShardModule::HandleRouteSyncObjectToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_SHARD_REQ, &KFRouteShardModule::HandleRouteAddObjectToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_SHARD_REQ, &KFRouteShardModule::HandleRouteRemoveObjectToShardReq );

    }

    void KFRouteShardModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_CLIENT_LOST_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_SHARD_REQ );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __FIND_ROUTE_SERVICE__( name )\
    auto routeservice = _route_service_list.Find( name );\
    if ( routeservice == nullptr )\
    {\
        return __LOG_ERROR__( "can't find service[{}]", name );\
    }\

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameAllReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToNameAllReq );

        __FIND_ROUTE_SERVICE__( kfmsg.name() );
        for ( auto& iter : routeservice->_server_object_count_list )
        {
            SendRouteMessage( iter.first, kfmsg.msgid(), kfmsg.msgdata(), kfmsg.sourceid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameRandReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToNameRandReq );

        __FIND_ROUTE_SERVICE__( kfmsg.name() );
        auto serverid = routeservice->RandServer( kfmsg.sourceid() );
        if ( serverid == _invalid_int )
        {
            return __LOG_ERROR__( "service[{}] no server!", kfmsg.name() );
        }

        SendRouteMessage( serverid, kfmsg.msgid(), kfmsg.msgdata(), kfmsg.sourceid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameBalanceReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToNameBalanceReq );

        __FIND_ROUTE_SERVICE__( kfmsg.name() );
        auto serverid = routeservice->BalanceServer();
        if ( serverid == _invalid_int )
        {
            return __LOG_ERROR__( "service[{}] no server!", kfmsg.name() );
        }

        SendRouteMessage( serverid, kfmsg.msgid(), kfmsg.msgdata(), kfmsg.sourceid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameObjectReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteMessageToNameObjectReq );

        __FIND_ROUTE_SERVICE__( kfmsg.name() );
        auto serverid = routeservice->ObjectServer( kfmsg.objectid() );
        if ( serverid == _invalid_int )
        {
            return __LOG_ERROR__( "service[{}] no server!", kfmsg.name() );
        }

        SendRouteMessage( serverid, kfmsg.msgid(), kfmsg.msgdata(), kfmsg.sourceid(), kfmsg.objectid() );
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

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteSyncObjectToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteSyncObjectToShardReq );

        auto routeservice = _route_service_list.Create( kfmsg.name() );
        routeservice->_name = kfmsg.name();

        routeservice->RemoveServer( kfmsg.clientid() );
        routeservice->AddServer( kfmsg.clientid() );

        for ( auto i = 0; i < kfmsg.objectid_size(); ++i )
        {
            routeservice->AddObject( kfmsg.clientid(), kfmsg.objectid( i ) );
        }

        __LOG_INFO__( "service[{}:{}] register ok!", kfmsg.name(), KFAppID::ToString( kfmsg.clientid() ) );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteAddObjectToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteAddObjectToShardReq );

        auto routeservice = _route_service_list.Find( kfmsg.name() );
        if ( routeservice == nullptr )
        {
            return __LOG_ERROR__( "service[{}:{}] not find!", kfmsg.name(), KFAppID::ToString( kfmsg.clientid() ) );
        }

        routeservice->AddObject( kfmsg.clientid(), kfmsg.objectid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteRemoveObjectToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteRemoveObjectToShardReq );

        auto routeservice = _route_service_list.Find( kfmsg.name() );
        if ( routeservice == nullptr )
        {
            return __LOG_ERROR__( "service[{}:{}] not find!", kfmsg.name(), KFAppID::ToString( kfmsg.clientid() ) );
        }

        routeservice->RemoveObject( kfmsg.clientid(), kfmsg.objectid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteClientLostToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRouteClientLostToShardReq );

        __LOG_ERROR__( "route client[{}:{}] lost!", kfmsg.name(), KFAppID::ToString( kfmsg.clientid() ) );

        auto routeservice = _route_service_list.Find( kfmsg.name() );
        if ( routeservice == nullptr )
        {
            return;
        }

        routeservice->RemoveServer( kfmsg.clientid() );
    }
}
