#include "KFRouteShardModule.hpp"

namespace KFrame
{
    void KFMessageData::AddNetMessage( uint32 msg_id, std::shared_ptr<const google::protobuf::Message> message )
    {
        auto data = message->SerializeAsString();
        auto net_message = __MAKE_SHARED__( KFNetMessage, data.length() );
        net_message->_header._msg_id = msg_id;
        net_message->CopyData( data.data(), data.length() );
        _message_list.push_back( net_message );
    }

    void KFMessageData::SendNetMessage()
    {
        if ( _message_list.empty() )
        {
            return;
        }

        for ( auto net_message : _message_list )
        {
            __HANDLE_MESSAGE__( net_message->_header._route, net_message->_header._msg_id, net_message->_data, net_message->_header._length );
        }
        _message_list.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRouteShardModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ, &KFRouteShardModule::HandleRouteMessageToNameAllReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ, &KFRouteShardModule::HandleRouteMessageToNameRandReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ, &KFRouteShardModule::HandleRouteMessageToNameBalanceReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ, &KFRouteShardModule::HandleRouteMessageToNameObjectReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ, &KFRouteShardModule::HandleRouteMessageToServerReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_MESSAGE_TO_ENTITY_REQ, &KFRouteShardModule::HandleRouteMessageToEntityReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_CLIENT_LOST_TO_SHARD_REQ, &KFRouteShardModule::HandleRouteClientLostToShardReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_SHARD_REQ, &KFRouteShardModule::HandleRouteSyncObjectToShardReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_ADD_OBJECT_TO_SHARD_REQ, &KFRouteShardModule::HandleRouteAddObjectToShardReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_SHARD_REQ, &KFRouteShardModule::HandleRouteRemoveObjectToShardReq );
    }

    void KFRouteShardModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_ALL_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_RAND_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_NAME_OBJECT_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_SERVER_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_MESSAGE_TO_ENTITY_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_CLIENT_LOST_TO_SHARD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_SYNC_OBJECT_TO_SHARD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_ADD_OBJECT_TO_SHARD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ROUTE_REMOVE_OBJECT_TO_SHARD_REQ );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __FIND_ROUTE_SERVICE__( name )\
    auto route_service = _route_service_list.Find( name );\
    if ( route_service == nullptr )\
    {\
        AddRouteFailedMessage( name, msg_id, kfmsg );\
        return __LOG_ERROR__( "can't find service[{}]", name );\
    }\

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameAllReq, KFMsg::S2SRouteMessageToNameAllReq )
    {
        __FIND_ROUTE_SERVICE__( kfmsg->name() );

        auto pb_route = &kfmsg->pbroute();
        for ( auto& iter : route_service->_server_object_count_list )
        {
            SendRouteMessage( iter.first, pb_route, kfmsg->msgid(), kfmsg->msgdata() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameRandReq, KFMsg::S2SRouteMessageToNameRandReq )
    {
        __FIND_ROUTE_SERVICE__( kfmsg->name() );

        auto pb_route = &kfmsg->pbroute();
        auto rand_id = ( pb_route->sendid() != 0u ? pb_route->sendid() : pb_route->serverid() );
        auto server_id = route_service->RandServer( rand_id );
        if ( server_id == _invalid_int )
        {
            return __LOG_ERROR__( "service[{}] no server", kfmsg->name() );
        }

        SendRouteMessage( server_id, pb_route, kfmsg->msgid(), kfmsg->msgdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameBalanceReq, KFMsg::S2SRouteMessageToNameBalanceReq )
    {
        __FIND_ROUTE_SERVICE__( kfmsg->name() );
        auto server_id = route_service->BalanceServer();
        if ( server_id == _invalid_int )
        {
            return __LOG_ERROR__( "service[{}] no server", kfmsg->name() );
        }

        auto pb_route = &kfmsg->pbroute();
        SendRouteMessage( server_id, pb_route, kfmsg->msgid(), kfmsg->msgdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToNameObjectReq, KFMsg::S2SRouteMessageToNameObjectReq )
    {
        __FIND_ROUTE_SERVICE__( kfmsg->name() );

        auto pb_route = &kfmsg->pbroute();
        auto server_id = route_service->ObjectServer( pb_route->recvid() );
        if ( server_id == _invalid_int )
        {
            return __LOG_ERROR__( "service[{}] no server", kfmsg->name() );
        }

        SendRouteMessage( server_id, pb_route, kfmsg->msgid(), kfmsg->msgdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToServerReq, KFMsg::S2SRouteMessageToServerReq )
    {
        auto pb_route = &kfmsg->pbroute();
        SendRouteMessage( kfmsg->targetid(), pb_route, kfmsg->msgid(), kfmsg->msgdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteMessageToEntityReq, KFMsg::S2SRouteMessageToEntityReq )
    {
        auto pb_route = &kfmsg->pbroute();
        SendRouteMessage( kfmsg->targetid(), pb_route, kfmsg->msgid(), kfmsg->msgdata() );
    }

    void KFRouteShardModule::AddRouteFailedMessage( const std::string& name, uint32 msg_id, std::shared_ptr<const google::protobuf::Message> message )
    {
        auto message_data = _route_message_list.Create( name );
        message_data->AddNetMessage( msg_id, message );
    }

    void KFRouteShardModule::SendRouteFailedMessage( const std::string& name )
    {
        auto message_data = _route_message_list.Find( name );
        if ( message_data != nullptr )
        {
            message_data->SendNetMessage();
        }
    }

    void KFRouteShardModule::SendRouteMessage( uint64 client_id, const KFMsg::PBRoute* pb_route, uint32 msg_id, const std::string& msg_data )
    {
        KFMsg::S2SRouteMessageToClientAck ack;
        ack.set_msgid( msg_id );
        ack.set_msgdata( msg_data );
        ack.mutable_pbroute()->CopyFrom( *pb_route );
        auto ok = _kf_cluster_shard->SendToClient( client_id, KFMsg::S2S_ROUTE_MESSAGE_TO_CLIENT_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "send=[{}] msg[{}] to recv=[{}] failed", KFAppId::ToString( pb_route->serverid() ), msg_id, KFAppId::ToString( client_id ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteSyncObjectToShardReq, KFMsg::S2SRouteSyncObjectToShardReq )
    {
        auto route_service = _route_service_list.Create( kfmsg->name() );
        route_service->_name = kfmsg->name();
        route_service->RemoveServer( kfmsg->clientid() );
        route_service->AddServer( kfmsg->clientid() );

        for ( auto i = 0; i < kfmsg->objectid_size(); ++i )
        {
            route_service->AddObject( kfmsg->clientid(), kfmsg->objectid( i ), kfmsg->objectid_size() );
        }

        SendRouteFailedMessage( kfmsg->name() );
        __LOG_INFO__( "service[{}:{}] register ok", kfmsg->name(), KFAppId::ToString( kfmsg->clientid() ) );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteAddObjectToShardReq, KFMsg::S2SRouteAddObjectToShardReq )
    {
        auto route_service = _route_service_list.Find( kfmsg->name() );
        if ( route_service == nullptr )
        {
            return __LOG_ERROR__( "service[{}:{}] not find", kfmsg->name(), KFAppId::ToString( kfmsg->clientid() ) );
        }
        route_service->AddObject( kfmsg->clientid(), kfmsg->objectid(), kfmsg->objectcount() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteRemoveObjectToShardReq, KFMsg::S2SRouteRemoveObjectToShardReq )
    {
        auto route_service = _route_service_list.Find( kfmsg->name() );
        if ( route_service == nullptr )
        {
            return __LOG_ERROR__( "service[{}:{}] not find", kfmsg->name(), KFAppId::ToString( kfmsg->clientid() ) );
        }

        route_service->RemoveObject( kfmsg->clientid(), kfmsg->objectid(), kfmsg->objectcount() );
    }

    __KF_MESSAGE_FUNCTION__( KFRouteShardModule::HandleRouteClientLostToShardReq, KFMsg::S2SRouteClientLostToShardReq )
    {
        __LOG_WARN__( "route client[{}:{}] lost", kfmsg->name(), KFAppId::ToString( kfmsg->clientid() ) );

        auto route_service = _route_service_list.Find( kfmsg->name() );
        if ( route_service == nullptr )
        {
            return;
        }

        route_service->RemoveServer( kfmsg->clientid() );
    }
}
